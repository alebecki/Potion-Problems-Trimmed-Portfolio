// Fill out your copyright notice in the Description page of Project Settings.


#include "PotProbPlayerState.h"
#include "FrogDiscoveredWidget.h"
#include "CauldronActor.h"
#include "DisguiseHUDWidget.h"
#include "PotProbGameState.h"
#include "HUDWidget.h"
#include "MinigameInitActor.h"
#include "PaperSprite.h"
#include "PotProbPlayerController.h"
#include "PotProbZDCharacter.h"
#include "Net/UnrealNetwork.h"
#include "PotionObject.h"
#include "PotProbGameMode.h"
#include "WorldSpaceWidgetActor.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetComponent.h"
#include "ObservatoryWorldActor.h"
#include "VoteWidget.h"
#include "SpyglassMinigameWidget.h"
#include "Camera/CameraComponent.h"
#include "PotionStatusWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PotProbSettingsSubsystem.h"
#include "PotProbSettingsSaveGame.h"

APotProbPlayerState::APotProbPlayerState()
{
	NetUpdateFrequency = 50.0f;
	NetPriority = 10.0f;
	bReplicateUsingRegisteredSubObjectList = true;
}

/** DEPRECATED - Use minigame world implementation **/
void APotProbPlayerState::ClientActivateInteractedMinigame_Implementation(
	APawn* InstigatorPawn, TSubclassOf<UUserWidget> MinigameWidgetClass, FName Name)
{
	APotProbPlayerController* PC = Cast<APotProbPlayerController>(InstigatorPawn->GetController());
	if (!IsValid(PC) || !IsValid(MinigameWidgetClass))
	{
		return;
	}

	// Check if a recipe is selected
	if (!GetCurrentRecipe().Potion)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot enter minigame without selecting a recipe."));
		return;
	}

	TObjectPtr<UUserWidget> MinigameWidget = Cast<UUserWidget>(CreateWidget(PC, MinigameWidgetClass));

	if (!IsValid(MinigameWidget))
	{
		return;
	}

	//check if character is the InstigatorPawn and also if its holding ingredient
	APotProbPlayerState* PotionPlayerState = Cast<APotProbPlayerState>(InstigatorPawn->GetPlayerState());
	if (!PotionPlayerState)
	{
		return;
	}
	if (!PotionPlayerState->GetIsHoldingIngredient())
	{
		//logic for spyglass puzzle
		MinigameWidget->SetIsFocusable(true);
		MinigameWidget->AddToViewport();
		MinigameWidget->SetKeyboardFocus();

		FInputModeGameAndUI Mode = FInputModeGameAndUI();
		Mode.SetWidgetToFocus(MinigameWidget->TakeWidget());

		PC->SetInputMode(Mode);
		PC->Server_ChangeIngredientStatus(Name, true);
	}
}

void APotProbPlayerState::ClientActivateInteractedMinigameWorld_Implementation(
	APawn* InstigatorPawn, AMinigameInitActor* InitActor, TSubclassOf<AWorldSpaceWidgetActor> MinigameWidgetActorClass,
	FVector SpawnLocation, FRotator SpawnRotation, FName Name)
{
	// check if unfrogged troublemaker
    if (PotProbRole != EPotProbRoles::ROLE_TROUBLEMAKER || bIsFrogged)
    {
    	ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "You probably shouldn't \nmess with that...");
        return;
    }

	// check if holding the right ingredient for interact
	if (!bHoldingIngredient || IngredientName != InitActor->GetIngredientName())
	{
		return;
	}
        
	UE_LOG(LogTemp, Warning, TEXT("ClientActivateInteractedMinigameWorld Current local role: %d"), GetLocalRole());
    
	APotProbPlayerController* PC = Cast<APotProbPlayerController>(InstigatorPawn->GetController());
	if (!IsValid(PC) || !IsValid(MinigameWidgetActorClass))
	{
		return;
	}

	// Define the spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; // Set the owner, if needed
	SpawnParams.Instigator = GetInstigator(); // Set the instigator, if needed

	// Spawn the actor
	AWorldSpaceWidgetActor* MinigameWidgetActor = GetWorld()->SpawnActor<AWorldSpaceWidgetActor>(
		MinigameWidgetActorClass, SpawnLocation, SpawnRotation, SpawnParams);
	UUserWidget* MinigameWidget = MinigameWidgetActor->FindComponentByClass<UWidgetComponent>()->GetUserWidgetObject();

	if (!IsValid(MinigameWidgetActor) && !IsValid(MinigameWidget))
	{
		UE_LOG(LogTemp, Warning, TEXT("Minigame not valid"));
		return;
	}

	// telescope specific logic
	SpyglassWidget = Cast<USpyglassMinigameWidget>(MinigameWidget);
	AObservatoryWorldActor* ObservatoryActor = Cast<AObservatoryWorldActor>(InitActor->ParentWidgetActor);
	if (IsValid(SpyglassWidget) && IsValid(ObservatoryActor))
	{
		SpyglassWidget->OwningWorldActor = MinigameWidgetActor;
		SpyglassWidget->ObservatoryWorldActor = ObservatoryActor;
		SpyglassWidget->ParentInitActor = InitActor;
	}
	
	// consume ingredient
	PC->Server_ChangeIngredientStatus(NAME_None, false);

	// tutorial alert
	if (!bPlayedMinigame)
	{
		ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Use WASD to move the target to the star!");
		bPlayedMinigame = true;
	}
}


void APotProbPlayerState::ResetStarMinigame_Implementation(AObservatoryWorldActor* ObservatoryActor)
{
	ObservatoryActor->ResetStar();
}

void APotProbPlayerState::Client_RemoveMinigameInputBindings_Implementation()
{
	if (IsValid(SpyglassWidget))
	{
		SpyglassWidget->RemoveInputBindings();
	}
}


void APotProbPlayerState::ReduceNumPotionCharges()
{
	if (!HasAuthority())
	{
		return;
	}
	if (NumPotionCharges == 0)
	{
		CurrentPotionChargeType = EPotionChargeType::CHARGE_NONE;
		return;
	}

	--NumPotionCharges;
}

void APotProbPlayerState::ServerCleanupPotionActivation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	// Reset name of character
	/*if (APlayerController* PC = GetPlayerController())
	{
		if (APotProbZDCharacter* PotPlayerCharacter = Cast<APotProbZDCharacter>(PC->GetCharacter()))
		{
			PotPlayerCharacter->SetPlayerText(PotPlayerCharacter->GetOriginalPlayerName());
		}
	}*/

	// we set back to false so that we can no longer activate potion
	bHasPotionBeenActivated = false;
	// Not to be confused with bCanPotionBeActivated. This is to ensure that UI Interact is drawn properly
	bShouldDisplayPotionAsActive = false;
	if (PlayerPotion)
	{
		RemoveReplicatedSubObject(PlayerPotion);
	}

	PlayerPotion = nullptr;
	// We want to call OnRep_PlayerPotion() each time the server updates the potion
	if (GetPlayerController()->IsLocalController())
	{
		OnRep_PlayerPotion();
	}
}


void APotProbPlayerState::ServerReceivePotion(TSubclassOf<UPotionObject> PotionType)
{
	// Alerts
	ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "You've Bottled a Potion!");
	ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Great Job! You just brewed a potion!\nPress R to drink it.");
	if (PotProbRole == EPotProbRoles::ROLE_APPRENTICE)
	{
		ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Different ingredient combos\nmake different potions!");
		ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Keep crafting potions to\ndefeat the troublemakers!");
	}
	
	// Audio alert
	APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController());
	if (PC)
	{
		PC->PlayPotionSound();
	}
	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("Local Role is: %d. Granting Player State Unique Net ID: %s a potion."), GetLocalRole(),
	       *GetUniqueId().GetUniqueNetId()->ToString());
	PlayerPotion = NewObject<UPotionObject>(this, PotionType);

	AddReplicatedSubObject(PlayerPotion);

	if (APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(GetPlayerController()))
	{
		if (PlayerController->IsLocalController())
		{
			OnRep_PlayerPotion();
		}
	}
}

void APotProbPlayerState::ServerGivePlayerPotionCharges()
{
	if (!HasAuthority())
	{
		return;
	}
	if (PlayerPotion->GetPotionCharges() != 0)
	{
		NumPotionCharges = PlayerPotion->GetPotionCharges();
	}
	if (PlayerPotion->GetPotionChargeType() != EPotionChargeType::CHARGE_NONE)
	{
		CurrentPotionChargeType = PlayerPotion->GetPotionChargeType();
	}
}

void APotProbPlayerState::SetIsFrogged_Implementation(bool status, bool bDiscoverable)
{
	bIsFrogged = status;
	bCanFroggedBeDiscovered = bDiscoverable;

	// Reduce capsule Collisions to fit through frog doors
	if  (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(GetPlayerController()->GetCharacter()))
	{
		Character->ToggleFrogProperties();
	}
	
	if (APotProbGameState* CurrGameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
	{
		if (PotProbRole == EPotProbRoles::ROLE_APPRENTICE)
		{
			CurrGameState->numApprenticeFrogged++;
		}
		else if (PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
		{
			CurrGameState->numTroublemakerFrogged++;
		}
	}
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController()))
	{
		// If frogged, show them the frogged tutorial
		if (status)
		{
			//PC->Client_ShowPlayerFroggedTutorial();
			PC->PlayFrogSound();
			// show frogged alert
			ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "You've been Frogged!");
		}
	}
	
	Server_CheckWinCon();
}

//Same as SetIsFrogged but does not check the win. Should be used in voting since we check the win there anyways and can better time when we call the win check. 
void APotProbPlayerState::SetIsFroggedVoting_Implementation(bool status, bool bDiscoverable)
{
	bIsFrogged = status;
	bCanFroggedBeDiscovered = bDiscoverable;

	// Reduce capsule Collisions to fit through frog doors
	if  (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(GetPlayerController()->GetCharacter()))
	{
		Character->ToggleFrogProperties();
	}
	
	if (APotProbGameState* CurrGameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
	{
		if (PotProbRole == EPotProbRoles::ROLE_APPRENTICE)
		{
			CurrGameState->numApprenticeFrogged++;
		}
		else if (PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
		{
			CurrGameState->numTroublemakerFrogged++;
		}
	}
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController()))
	{
		// If frogged, show them the frogged tutorial
		if (status)
		{
			//PC->Client_ShowPlayerFroggedTutorial();
			PC->PlayFrogSound();
			// show frogged alert
			ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "You've been Frogged!");
		}
	}
	
	//Server_CheckWinCon();
}

void APotProbPlayerState::SetVotingPhase_Implementation(bool status)
{
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController()))
	{
		if (status)
		{
			PC->Client_ToggleVotingUI(true);
		}
		else
		{
			PC->Client_ToggleVotingUI(false);
		}
	}
}


void APotProbPlayerState::OnFroggedDiscovered_Implementation(TSubclassOf<UFrogDiscoveredWidget> FrogDiscoveredWidgetClass)
{
	if (APotProbGameState* CurrGameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
	{
		// CurrentPhase should be in game to trigger the voting phase
		// Also a werefrog shouldn't be able to report someone
		
		if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController()))
		{
			if (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(PC->GetCharacter()))
			{
				if (CurrGameState->CurrentPhase == EPotProbPhases::PHASE_INGAME
					&& bIsFrogged
					&& bCanFroggedBeDiscovered
					&& !Character->GetIsWerefrog()
					&& Character->GetPlayerDisplayModel() != (EAnimModels::WEREFROG_MODEL))
				{
					bCanFroggedBeDiscovered = false;

					// alert everyone
					FString AlertMessage = FString::Printf(TEXT("A fresh frog has been discovered!\nVoting starts in %i seconds!"), FrogDiscoveredCountdown);
					ServerReceiveAlertMessage(EPotProbAlertTypes::HIGH_PRIORITY, AlertMessage);

					// timer to show ui popup
					FTimerHandle FrogDiscoveredHandle;
					FTimerDelegate FrogDiscoveredDelegate;
					FrogDiscoveredDelegate.BindUFunction(this, FName("MulticastShowFrogDiscoveredPopUp"), FrogDiscoveredWidgetClass);
					GetWorld()->GetTimerManager().SetTimer(FrogDiscoveredHandle, FrogDiscoveredDelegate, static_cast<float>(FrogDiscoveredCountdown), false);
				}
			}
		}
	}
}

void APotProbPlayerState::Server_SetReady_Implementation(bool status)
{
	bIsReady = status;
	if (GetLocalRole() == ROLE_Authority)
	{
		OnRep_IsReady();
		
		if (APotProbPlayerController* PC = Cast<APotProbPlayerController>( UGameplayStatics::GetPlayerControllerFromID(GetWorld(),0) ))
		{
			PC->Client_UpdateLocalLobbyStatusUI();
		}
	}
}

void APotProbPlayerState::Server_Switcharoo_Implementation()
{
	/*TArray<ACauldronActor*> CaulAct = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode())->GetCauldrons();
	ACauldronActor* ChosenCaul = nullptr;
	ACauldronActor* EmptyCaul = nullptr;
	float MaxDist = TNumericLimits<float>::Max();
	APotProbPlayerController* PPC = Cast<APotProbPlayerController>(GetPlayerController());

	if (PPC)
	{
		APotProbZDCharacter* PC = Cast<APotProbZDCharacter>(PPC->GetCharacter());

		for (const auto& Caul : CaulAct)
		{
			if (TObjectPtr<APotProbZDCharacter> CaulProp = Cast<APotProbZDCharacter>(Caul->GetCauldronProprietor()))
			{
				if (CaulProp != PC)
				{
					float Dist = FVector::Distance(PC->GetActorLocation(), Caul->GetActorLocation());
					if (Dist < MaxDist)
					{
						MaxDist = Dist;
						ChosenCaul = Caul;
					}
				}
			}
			else
			{
				bool bRand = FMath::RandBool();
				if (bRand || !EmptyCaul)
				{
					EmptyCaul = Caul;
				}
			}
		}

		if (ChosenCaul && EmptyCaul)
		{
			TObjectPtr<APotProbZDCharacter> CaulProp = Cast<APotProbZDCharacter>(ChosenCaul->GetCauldronProprietor());
			APotProbPlayerController* CaulPropController = Cast<APotProbPlayerController>(CaulProp->GetController());
			APotProbPlayerState* OtherPlayerState = Cast<APotProbPlayerState>(CaulProp->GetPlayerState());
			OtherPlayerState->Client_DisplaySwitcharooMessage(CaulPropController);
			//Client_DisplaySwitcharooMessage_Implementation(CaulPropController);
			Multicast_SwitchPlacesCauldron(ChosenCaul, EmptyCaul);
		}
	}*/
}

void APotProbPlayerState::Multicast_SwitchPlacesCauldron_Implementation(ACauldronActor* Cal1, ACauldronActor* Cal2)
{
	FVector TempPos = Cal1->GetActorLocation();
	Cal1->SetActorLocation(Cal2->GetActorLocation());
	Cal2->SetActorLocation(TempPos);
}

void APotProbPlayerState::Client_DisplaySwitcharooMessage_Implementation(class APotProbPlayerController* PC)
{
	PC->Client_DisplaySwitcharooMessage_Implementation();
}

void APotProbPlayerState::OnRep_IsReady()
{
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController()))
	{
		PC->Client_UpdateLocalLobbyStatusUI();
	}
}

void APotProbPlayerState::Server_SetIsPlayingAgain_Implementation(bool Status)
{
	// Update flag so on leaving the player is also removed from num looping
	bPlayAgain = Status;

	// TODO: Race Condition!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Update GameMode Number of Looping Players
	if (APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
	{
		int NewNumLooping = Status ? GameState->GetNumLoopingPlayers() + 1 : GameState->GetNumLoopingPlayers() - 1;
		GameState->SetNumLoopingPlayers(NewNumLooping);
		if (HasAuthority())
		{
			GameState->OnRep_LoopingStatus();
		}
	}
}

void APotProbPlayerState::OnRep_Vote()
{
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController()))
	{
		PC->Server_UpdateAllVoteWidgets();
	}
}

void APotProbPlayerState::SkipVote_Implementation()
{
	bHasVoted = true;
	if (HasAuthority())
	{
		OnRep_Vote();
	}
}


void APotProbPlayerState::Server_CheckWinCon_Implementation()
{
	CheckWinCon();
}

void APotProbPlayerState::CheckWinCon()
{
	APotProbGameMode* GameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->CheckWin();
	}
}

void APotProbPlayerState::GivePlayerStatePotionRecipes_Implementation(const TArray<FRecipeStruct>& DistributedRecipes)
{
	APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController());
	if (!PC)
	{
		return;
	}
	//PC->HUDWidgetInstance->PotionRecipeInfo->SetVisibility(ESlateVisibility::Collapsed);
	//PC->FilloutPotionDistributionUI(DistributedRecipes);
	//Server_SetFrogRecipe();
	if (PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
	{
		PC->HUDWidgetInstance->AddFrogRecipeInformationToHUD(FrogRecipe);
	}
}

void APotProbPlayerState::OnRep_PlayerPotion()
{
	APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController());
	if (!PC)
	{
		return;
	}
	if (!PC->HUDWidgetInstance)
	{
		return;
	}

	if (IsValid(PlayerPotion))
	{
		PC->HUDWidgetInstance->CreatePotionWidgetPopup(PlayerPotion);
		
		PC->HUDWidgetInstance->DrinkText->SetVisibility(ESlateVisibility::HitTestInvisible);
		PC->HUDWidgetInstance->DrinkPotionBanner->SetVisibility(ESlateVisibility::HitTestInvisible);
		PC->HUDWidgetInstance->DrinkPotionCircle->SetVisibility(ESlateVisibility::HitTestInvisible);
		PC->HUDWidgetInstance->DrinkPotionBind->SetVisibility(ESlateVisibility::HitTestInvisible);
		PC->HUDWidgetInstance->PotionNameText->SetText(FText::FromString(FString::Printf(TEXT("You made a %s"), *PlayerPotion->GetPotionName())));
		PC->HUDWidgetInstance->PotionNameText->SetVisibility(ESlateVisibility::HitTestInvisible);
		PC->HUDWidgetInstance->PotionSlotImage->SetBrushFromTexture(PlayerPotion->GetPotionSprite()->GetBakedTexture());
		PC->HUDWidgetInstance->PotionHeldNameText->SetText(FText::FromString(*PlayerPotion->GetPotionName()));

		if (PlayerPotion->GetCanSplash())
		{
			PC->HUDWidgetInstance->SplashText->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
	else
	{
		PC->HUDWidgetInstance->DrinkText->SetVisibility(ESlateVisibility::Hidden);
		PC->HUDWidgetInstance->DrinkPotionBanner->SetVisibility(ESlateVisibility::Hidden);
		PC->HUDWidgetInstance->DrinkPotionCircle->SetVisibility(ESlateVisibility::Hidden);
		PC->HUDWidgetInstance->DrinkPotionBind->SetVisibility(ESlateVisibility::Hidden);
		PC->HUDWidgetInstance->SplashText->SetVisibility(ESlateVisibility::Hidden);
		PC->HUDWidgetInstance->PotionNameText->SetVisibility(ESlateVisibility::Hidden);
		PC->HUDWidgetInstance->PotionSlotImage->SetBrushFromTexture(PC->HUDWidgetInstance->DefaultPotionSlotImage);
		PC->HUDWidgetInstance->PotionHeldNameText->SetText(FText::FromString(FString::Printf(TEXT("Empty"))));
	}
}

void APotProbPlayerState::SetCanInteractWithObjects(bool Input)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	bCanInteractWithObjects = Input;
}

void APotProbPlayerState::OnRep_IngredientName()
{
	APotProbPlayerController* PotionController = Cast<APotProbPlayerController>(GetPlayerController());

	if (!PotionController)
	{
		return;
	}

	/*for (int32 I = 0; I < CurrentRecipe.Ingredients.Num(); ++I)
	{
		if (IngredientName == CurrentRecipe.Ingredients[I] && !PotionController->HUDWidgetInstance->PotionRecipeInfo->
			GetIsObtained(I))
		{
			PotionController->HUDWidgetInstance->PotionRecipeInfo->MarkIngredientAsSelected(I);
			break;
		}
	}*/

	/*if (PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
	{
		for (int32 I = 0; I < FrogRecipe.Ingredients.Num(); ++I)
		{
			if (IngredientName == FrogRecipe.Ingredients[I] && !PotionController->HUDWidgetInstance->FrogRecipeInfo->
				GetIsObtained(I))
			{
				PotionController->HUDWidgetInstance->FrogRecipeInfo->MarkIngredientAsSelected(I);
				break;
			}
		}
	}*/
}

void APotProbPlayerState::SetIngredientName(const FName NewIngredientName)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	IngredientName = NewIngredientName;
}

void APotProbPlayerState::SetIsHoldingIngredient(const bool Input)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	bHoldingIngredient = Input;
	if (bHoldingIngredient)
	{
		bHasPickedUpFirstIngredient = true;
	}
}

void APotProbPlayerState::OnRep_IngredientIcon()
{
	APotProbPlayerController* PotionController = Cast<APotProbPlayerController>(GetPlayerController());

	if (!PotionController)
	{
		return;
	}
	APotProbZDCharacter* CharPtr = static_cast<APotProbZDCharacter*>(PotionController->GetCharacter());
	if (!CharPtr)
	{
		return;
	}
	if (IngredientIcon == nullptr)
	{
		CharPtr->SetIngredientSprite(IngredientIcon);
		CharPtr->OnIngredientPickup.Broadcast(false);
	}
	else
	{
		CharPtr->SetIngredientSprite(IngredientIcon);
		CharPtr->OnIngredientPickup.Broadcast(true);
	}
}

void APotProbPlayerState::SetIngredientIcon(UPaperSprite* Texture)
{
	IngredientIcon = Texture;
}

void APotProbPlayerState::Server_SetCurrentRecipe_Implementation(const FRecipeStruct& Recipe)
{
	CurrentRecipe = Recipe;
	ResetRerolls();
	Client_SetCurrentRecipe(Recipe);
}

void APotProbPlayerState::Client_SetCurrentRecipe_Implementation(const FRecipeStruct& Recipe)
{
	APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController());
	PC->HUDWidgetInstance->AddRecipeInformationToHUD(Recipe);
	// set frog recipe if troublemaker
	Server_SetFrogRecipe();
	if (PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
	{
		PC->HUDWidgetInstance->AddFrogRecipeInformationToHUD(FrogRecipe);
	}
}

void APotProbPlayerState::Server_SetFrogRecipe_Implementation()
{
	if (FrogRecipe.Potion == nullptr)
	{
		Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode())->SetPlayerStateFrogRecipe();
	}
}

FRecipeStruct APotProbPlayerState::GetCurrentRecipe()
{
	return CurrentRecipe;
}


void APotProbPlayerState::ServerRerollPotions_Implementation()
{
	// To be implemented fully

	APotProbGameMode* PotionGameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode());
	if (!PotionGameMode)
	{
		return;
	}
	// For now just like reroll
	//add reroll logic here from the potion recipe widget
	numRerolls++;
	TArray<FRecipeStruct> LocalRecipes = PotionGameMode->GenerateSubsetOfPotionRecipes(this);
	TArray<FRecipeStruct> modifiedRecipes = LocalRecipes;
	if (numRerolls == 2) 
	{
		//int counter = 0;
		for (int i = 0; i < LocalRecipes.Num(); i++)
		{
			if (LocalRecipes[i].Potion.GetDefaultObject()->GetPotionName() != "Frog Potion")
			{
				int32 randomNum = FMath::RandRange(0, LocalRecipes[i].Ingredients.Num() - 1);

				modifiedRecipes[i].Ingredients.Add(LocalRecipes[i].Ingredients[randomNum]);
				modifiedRecipes[i].IngredientSprites.Add(LocalRecipes[i].IngredientSprites[randomNum]);

				//LocalRecipes[counter] = recipe;
			}
			//counter++;
		}
	}
	else if (numRerolls >= 3) 
	{
		for (int i = 0; i < LocalRecipes.Num(); i++)
		{
			if (LocalRecipes[i].Potion.GetDefaultObject()->GetPotionName() != "Frog Potion")
			{
				int32 randomNum = FMath::RandRange(0, LocalRecipes[i].Ingredients.Num() - 1);

				modifiedRecipes[i].Ingredients.Add(LocalRecipes[i].Ingredients[randomNum]);
				modifiedRecipes[i].IngredientSprites.Add(LocalRecipes[i].IngredientSprites[randomNum]);

				randomNum = FMath::RandRange(0, LocalRecipes[i].Ingredients.Num() - 1);

				modifiedRecipes[i].Ingredients.Add(LocalRecipes[i].Ingredients[randomNum]);
				modifiedRecipes[i].IngredientSprites.Add(LocalRecipes[i].IngredientSprites[randomNum]);

				//LocalRecipes[counter] = recipe;
			}
			//counter++;
		}
	}
	LocalRecipes = modifiedRecipes;
	GivePlayerStatePotionRecipes(LocalRecipes);
}

void APotProbPlayerState::OnRep_PotProbRole() const
{
	APotProbPlayerController* PotionController = Cast<APotProbPlayerController>(GetPlayerController());
	if (!PotionController)
	{
		return;
	}

	if (PotProbRole != EPotProbRoles::ROLE_UNASSIGNED)
	{
		PotionController->Client_ShowPlayerStartRole(PotProbRole, Cast<APotProbGameState>(GetWorld()->GetGameState())->NumTroublemakers);
	}
	
	//Set Player Role string
	FString PlayerRoleString = "Role: ";
	bool bIsTroubleMaker = false;
	switch (PotProbRole)
	{
	// Unknown if we want a playerstate with ROLE_NONE to do anything at the moment so we can print an error for now. 
	case EPotProbRoles::ROLE_UNASSIGNED:
		PlayerRoleString.Append("Unassigned");
		break;
	case EPotProbRoles::ROLE_APPRENTICE:
		PlayerRoleString.Append("Apprentice");
		break;
	case EPotProbRoles::ROLE_TROUBLEMAKER:
		bIsTroubleMaker = true;
		PlayerRoleString.Append("Troublemaker");
		break;
	default:
		{
			UE_LOG(LogTemp, Error, TEXT("Warning: Player State has an invalid PotProbRole"));
			return;
		}
	}
	if (PotionController->HUDWidgetInstance)
	{
		PotionController->HUDWidgetInstance->PlayerRole->SetText(FText::FromString(PlayerRoleString));
		PotionController->HUDWidgetInstance->SetHUDPlayerType(bIsTroubleMaker);
	}
}

void APotProbPlayerState::Server_SetRole_Implementation(EPotProbRoles NewRole)
{
	PotProbRole = NewRole;
	OnRep_PotProbRole();
}

void APotProbPlayerState::OnRep_IsFrogged()
{
	APotProbPlayerController* PotionController = Cast<APotProbPlayerController>(GetPlayerController());
	if (!PotionController)
	{
		return;
	}
	if (bIsFrogged && PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
	{
		// If we are a frogged troublemaker, then we reroll and delete frog potion if we have one
		if (PlayerPotion)
		{
			if (PlayerPotion.Get()->GetPotionName() == "Frog Potion")
			{
				UE_LOG(LogTemp, Error, TEXT("Frogged troublemaker. Removing Frog Potion from player"));
				ServerCleanupPotionActivation();
			}
		}
		UE_LOG(LogTemp, Error, TEXT("Frogged troublemaker. Rerolling Potions"));
		ServerRerollPotions();
	}

	//Set Player Frogged state
	FString PlayerFroggedString = "Frogged: ";
	if (bIsFrogged)
	{
		PlayerFroggedString.Append("TRUE");
	}
	else
	{
		PlayerFroggedString.Append("FALSE");
	}
	PotionController->HUDWidgetInstance->PlayerFrogged->SetText(FText::FromString(PlayerFroggedString));
}

void APotProbPlayerState::MulticastShowFrogDiscoveredPopUp_Implementation(TSubclassOf<UFrogDiscoveredWidget> FrogDiscoveredWidgetClass)
{
	APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetWorld()->GetFirstPlayerController());
	if (UFrogDiscoveredWidget* Widget = CreateWidget<UFrogDiscoveredWidget>(PC, FrogDiscoveredWidgetClass))
	{
		Widget->UpdateDiscoveredText(GetPlayerName());
		Widget->AddToViewport(100);
	}
}

void APotProbPlayerState::Server_SetHasCooties_Implementation(bool bValue)
{
	APotProbGameMode* PotionGameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode());

	if (!PotionGameMode)
	{
		return;
	}
	
	if (PotionGameMode->DetermineIfEndCooties())
	{
		bHasCooties = false;

		// call manually on server
		OnRep_HasCooties();
		return;
	}

	bHasCooties = bValue;
	if (bHasCooties)
	{
		bHasCootiesBefore = true;
		// alert has cooties
		ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "You have Cooties! \nGive them to other players.");
	}
	
	Client_SetHasCooties(bValue);

	// call manually on server
	OnRep_HasCooties();
}

void APotProbPlayerState::Client_SetHasCooties_Implementation(bool bValue)
{
	if (APotProbZDCharacter* CharPtr = Cast<APotProbZDCharacter>(GetPlayerController()->GetCharacter()))
	{
		CharPtr->ClientToggleCootiesEffects(bValue);
	}
	
	if (bValue)
	{
		if (APotProbZDCharacter* CharPtr = Cast<APotProbZDCharacter>(GetPlayerController()->GetCharacter()))
		{
			Server_GetCootiesTimerHandle();
			CootiesStatusIcon = AddPotionEffectIconToHUD(CharPtr->CootiesStatusIconTexture, CootiesTimeLeft);
		}
	}
	else
	{
		if (CootiesStatusIcon && IsValid(CootiesStatusIcon))
		{
			RemovePotionEffectIconFromHUD(CootiesStatusIcon);
			CootiesStatusIcon = nullptr;
		}
	}
}

void APotProbPlayerState::Server_GetCootiesTimerHandle_Implementation()
{
	if (APotProbGameMode* PotionGameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode()))
	{
		CootiesTimeLeft = PotionGameMode->GetCootiesTimeRemaining();
		CootiesTimeLeft = FMath::Max(0.0, CootiesTimeLeft);
	}
}

void APotProbPlayerState::ClearCooties()
{
	if (!HasAuthority())
	{
		return;
	}

	bHasCooties = false;
	bHasCootiesBefore = false;
	bIsCootiesInstigator = false;

	Client_SetHasCooties(bHasCooties);
	OnRep_HasCooties();
}

void APotProbPlayerState::SetIsCootiesInstigator(bool bValue)
{
	if (!HasAuthority())
	{
		return;
	}
	APotProbGameMode* GameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
	{
		return;
	}

	if (APotProbZDCharacter* CharPtr = Cast<APotProbZDCharacter>(GetPlayerController()->GetCharacter()))
	{
		GameMode->StartTimerForCooties(CharPtr->LovePotionDuration);
		bIsCootiesInstigator = bValue;
	}
}

void APotProbPlayerState::ClearQuestionmarkPotionEffects_Implementation()
{
	
	bQuestionmarkPotionActive = false;
	GetWorld()->GetTimerManager().ClearTimer(QuestionmarkPotionTimer);
	UE_LOG(LogTemp, Log, TEXT("Questionmark Potion Deactivation: Clearing Questionmark Potion Effects"));
	ClearPotionEffects();
}

void APotProbPlayerState::StartQuestionmarkPotion_Implementation()
{
	if (bUnstablePotionActive)
	{
		UE_LOG(LogTemp, Log, TEXT("Overriding questionmark potion effects"));
		ClearQuestionmarkPotionEffects();
	}
	
	ActivatePotionEffectsFromBucket();

	bQuestionmarkPotionActive = true;
	UE_LOG(LogTemp, Log, TEXT("Activated questionmark potion effects"));
	ClientStartUnstablePotion();
	GetWorld()->GetTimerManager().SetTimer(QuestionmarkPotionTimer, this, &APotProbPlayerState::ClearQuestionmarkPotionEffects,
										   120.0f, false);
}

void APotProbPlayerState::ActivatePotionEffectsFromBucket_Implementation()
{	if (APotProbZDCharacter* CharPtr = Cast<APotProbZDCharacter>(GetPlayerController()->GetCharacter()))
	{
		if (APotProbGameMode* PotProbGameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode()))
		{
			for (auto EffectBuckets = PotProbGameMode->GetUnstablePotionEffectBuckets(); auto& Bucket : EffectBuckets)
			{
				auto& EffectArray = Bucket.Effects;
				EPotProbEffects SelectedEffect = EffectArray[FMath::RandRange(0, EffectArray.Num() - 1)];
				ActivatedEffects.Enqueue(SelectedEffect);
				bool bEffectIsPositive = FMath::RandBool();
				if (SelectedEffect == EPotProbEffects::PLAYER_SPEED)
				{
					UE_LOG(LogTemp, Log, TEXT("Unstable Potion Activation: Player Speed Effect"));
					// Increase or decrease player speed by percentage
					if (bEffectIsPositive)
					{
						CharPtr->ServerChangeMovementSpeed(1.5f);
					}
					else
					{
						CharPtr->ServerChangeMovementSpeed(0.667f);
					}
				}
				else if (SelectedEffect == EPotProbEffects::PLAYER_FOV)
				{
					UE_LOG(LogTemp, Log, TEXT("Unstable Potion Activation: FOV Effect"));
					if (UCameraComponent* CameraComponent = CharPtr->FindComponentByClass<UCameraComponent>())
					{
						if (bEffectIsPositive)
						{
							// hard coded for now, to be discussed with designers
							CameraComponent->SetFieldOfView(105.0f);
						}
						else
						{
							CameraComponent->SetFieldOfView(75.0f);
						}
					}
				}
				else if (SelectedEffect == EPotProbEffects::SPRITE_SIZE)
				{
					UE_LOG(LogTemp, Log, TEXT("Unstable Potion Activation: Sprite Size Change"));
					if (UPaperFlipbookComponent* SpriteComp = CharPtr->FindComponentByClass<UPaperFlipbookComponent>())
					{
						if (bEffectIsPositive)
						{
							// hard coded for now, to be discussed with designers
							SpriteComp->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
						}
						else
						{
							SpriteComp->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
						}
					}
				}
				else if (SelectedEffect == EPotProbEffects::SPRITE_SKIN)
				{
					UE_LOG(LogTemp, Log, TEXT("Unstable Potion Activation: Sprite Skin Change"));
					int32 Start = (int32)EAnimModels::CHAR_MODEL_0;
					int32 End = (int32)EAnimModels::CHAR_MODEL_9;
					int32 RandCamo = FMath::RandRange(Start, End);
					CharPtr->Server_SetModelOverride(static_cast<EAnimModels>(RandCamo));
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Unstable Potion Activation: Invalid PotProbEffect"));
				}
			}
		}
	}
}

void APotProbPlayerState::ClearPotionEffects_Implementation()
{
	if (APotProbZDCharacter* CharPtr = Cast<APotProbZDCharacter>(GetPlayerController()->GetCharacter()))
	{
		while (!ActivatedEffects.IsEmpty())
		{
			EPotProbEffects Effect;
			ActivatedEffects.Dequeue(Effect);
			if (Effect == EPotProbEffects::PLAYER_SPEED)
			{
				UE_LOG(LogTemp, Log, TEXT("Unstable Potion Deactivation: Player Speed Effect"));
				CharPtr->ServerChangeMovementSpeed(1.0f);
			}
			else if (Effect == EPotProbEffects::PLAYER_FOV)
			{
				UE_LOG(LogTemp, Log, TEXT("Unstable Potion Deactivation: FOV Effect"));
				if (UCameraComponent* CameraComponent = CharPtr->FindComponentByClass<UCameraComponent>())
				{
					CameraComponent->SetFieldOfView(90.0f);
				}
			}
			else if (Effect == EPotProbEffects::SPRITE_SIZE)
			{
				UE_LOG(LogTemp, Log, TEXT("Unstable Potion Deactivation: Sprite Size Change"));
				if (UPaperFlipbookComponent* SpriteComp = CharPtr->FindComponentByClass<UPaperFlipbookComponent>())
				{
					SpriteComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
				}
			}
			else if (Effect == EPotProbEffects::SPRITE_SKIN)
			{
				UE_LOG(LogTemp, Log, TEXT("Unstable Potion Activation: Sprite Skin Change"));
				CharPtr->Server_ClearModelOverride();
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Unstable Potion Deactivation: Invalid PotProbEffect"));
			}
		}
	}
}

void APotProbPlayerState::OnRep_IsDisguised()
{
	APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController());
	if(!PC)
	{
		return;
	}
	if (PC->DisguiseHUDWidgetInstance)
	{
		if(bIsDisguised)
		{
			PC->DisguiseHUDWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			PC->DisguiseHUDWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void APotProbPlayerState::ServerSetIsDisguised_Implementation(bool Input)
{
	bIsDisguised = Input;
	if (bIsDisguised)
	{
		ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "Select a player to assume\ntheir appearance!");
	}

	ClientSetIsDisguised(Input);
}

void APotProbPlayerState::ClientSetIsDisguised_Implementation(bool Input)
{
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController()))
	{
		if (PC->GetHUDWidgetInstance())
		{
			PC->GetHUDWidgetInstance()->ToggleDrinkText(Input);
		}
	}
}

void APotProbPlayerState::StopDisguise()
{
	if(!HasAuthority())
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(DisguisePotionHandle);
	APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(GetPlayerController()->GetCharacter());
	if(!Character)
	{
		return;
	}
	
	ClientStopDisguise();
	Character->Server_ClearModelOverride();
	Character->SetPlayerText(GetPlayerName());
	ServerCleanupPotionActivation();
}

void APotProbPlayerState::ClientStopDisguise_Implementation()
{
	if (DisguiseStatusIcon && IsValid(DisguiseStatusIcon))
	{
		RemovePotionEffectIconFromHUD(DisguiseStatusIcon);
		DisguiseStatusIcon = nullptr;
	}
}


void APotProbPlayerState::ServerStartEgo(float TimerDuration)
{
	if(!HasAuthority())
	{
		return;
	}
	bIsUsingEgo = true;
	APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState());
	if(!GameState)
	{
		return;
	}
	APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(GetPlayerController()->GetPawn());
	if(!Character)
	{
		return;
	}
	
	EAnimModels SpriteModel = Character->GetPlayerSelectedModel(); 
	for(const auto& Iter : GameState->PlayerArray)
	{
		if(!Iter.Get())
		{
			continue;
		}

		APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(Iter.Get());
		if(!PlayerState)
		{
			continue;
		}

		APotProbZDCharacter* OtherCharacter = Cast<APotProbZDCharacter>(Iter->GetPlayerController()->GetPawn());
		if (!OtherCharacter)
        {
            continue;
        }
		
		if(PlayerState->GetIsUsingEgo() || PlayerState->bIsFrogged || OtherCharacter->GetIsWerefrog())
		{
			continue;
		}

		APotProbZDCharacter* NewCharacter = Cast<APotProbZDCharacter>(Iter->GetPlayerController()->GetPawn());
		if(!NewCharacter)
		{
			continue;
		}
		NewCharacter->Server_SetModelOverride(SpriteModel);
		NewCharacter->SetPlayerText(GetPlayerName());

		PlayerState->ClientAddEgoPotionEffectToHUD(TimerDuration);
		PlayerState->ClientOverridePlayerText(true);
	}
	GetWorld()->GetTimerManager().ClearTimer(EgoPotionHandle);
	GetWorld()->GetTimerManager().SetTimer(EgoPotionHandle, this,
										   &APotProbPlayerState::EndEgo, TimerDuration);

	// alert everyone
	ServerReceiveAlertMessage(EPotProbAlertTypes::HIGH_PRIORITY, GetPlayerName() + " \nused the Ego potion! \nNow everyone looks like them!");

	// The caller player state needs to clean up the potion after activating it. Avoid spamming
	ServerCleanupPotionActivation();
}

void APotProbPlayerState::ClientAddEgoPotionEffectToHUD_Implementation(float EffectTime)
{
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController()))
	{
		if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(PC->GetCharacter()))
		{
			if (EgoPotionStatusIcon && IsValid(EgoPotionStatusIcon))
			{
				RemovePotionEffectIconFromHUD(EgoPotionStatusIcon);
				EgoPotionStatusIcon = nullptr;
			}
			EgoPotionStatusIcon = AddPotionEffectIconToHUD(PlayerCharacter->EgoStatusIconTexture, EffectTime);
		}
	}
}


void APotProbPlayerState::EndEgo()
{
	if(!HasAuthority())
	{
		return;
	}

	APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState());
	if(!GameState)
	{
		return;
	}
	for(const auto& Iter : GameState->PlayerArray)
	{
		if(!Iter.Get())
		{
			continue;
		}

		APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(Iter.Get());
		if(!PlayerState)
		{
			continue;
		}

		/*if (PlayerState->EgoPotionStatusIcon && IsValid(PlayerState->EgoPotionStatusIcon))
		{
			PlayerState->RemovePotionEffectIconFromHUD(PlayerState->EgoPotionStatusIcon);
			PlayerState->EgoPotionStatusIcon = nullptr;
		}*/

		if(PlayerState->GetIsUsingEgo())
		{
			PlayerState->SetIsUsingEgo(false);
			continue;
		}

		APotProbZDCharacter* NewCharacter = Cast<APotProbZDCharacter>(Iter->GetPlayerController()->GetPawn());
		if(!NewCharacter)
		{
			continue;
		}
		NewCharacter->Server_ClearModelOverride();
		NewCharacter->SetPlayerText(PlayerState->GetPlayerName());
	}
	ClientOverridePlayerText(false);
	GetWorld()->GetTimerManager().ClearTimer(EgoPotionHandle);
	ServerCleanupPotionActivation();
}

void APotProbPlayerState::ClientOverridePlayerText_Implementation(bool bIsEgoActive)
{
	APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState());
	if(!GameState)
	{
		return;
	}

	for(const auto& Iter : GameState->PlayerArray)
	{
		if(!Iter.Get())
		{
			continue;
		}
		APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(Iter.Get());
		if(!PlayerState)
		{
			continue;
		}

		if(PlayerState->GetIsUsingEgo())
		{
			continue;
		}
		APotProbZDCharacter* NewCharacter = Cast<APotProbZDCharacter>(PlayerState->GetPawn());
	
		if(!NewCharacter)
		{
			continue;
		}
		NewCharacter->EgoOverridePlayerText(PlayerState->GetPlayerName(), bIsEgoActive);
	}
}

void APotProbPlayerState::ServerStartDisguise_Implementation(float TimerDuration)
{
	GetWorld()->GetTimerManager().ClearTimer(DisguisePotionHandle);
	GetWorld()->GetTimerManager().SetTimer(DisguisePotionHandle, this,
										   &APotProbPlayerState::StopDisguise, TimerDuration);
	ClientStartDisguise(TimerDuration);
}

void APotProbPlayerState::ClientStartDisguise_Implementation(float TimerDuration)
{
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController()))
	{
		if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(PC->GetCharacter()))
		{
			DisguiseStatusIcon = AddPotionEffectIconToHUD(PlayerCharacter->DisguiseStatusIconTexture, TimerDuration);
		}
	}
}

void APotProbPlayerState::TeleportToRandomPlayer()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APotProbPlayerController::StaticClass(), FoundActors);

	int Size = FoundActors.Num();
	while (true)
	{
		int RandomIndex = FMath::RandRange(0, Size -1);

		if (APotProbPlayerController* PotProbController = Cast<APotProbPlayerController>(FoundActors[RandomIndex]))
		{
			if (PotProbController != Cast<APotProbPlayerController>(GetPlayerController()))
			{
				if (PotProbController->GetPawn())
				{
					GetPawn()->SetActorLocation(PotProbController->GetPawn()->GetActorLocation());

					// alert both players
					ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "You've been translocated \nto another location!");
					APotProbPlayerState* OtherPS = Cast<APotProbPlayerState>(PotProbController->PlayerState);
					if (OtherPS)
					{
						OtherPS->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "You've been translocated \nto another location!");
					}
					
					break;
				}
			}
		}
	}
}

void APotProbPlayerState::SetTranslocationTimerActive(float TimeValue)
{
	if(!HasAuthority())
	{
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(TranslocationPotionTimer);
	GetWorld()->GetTimerManager().SetTimer(TranslocationPotionTimer, this,
	                                       &APotProbPlayerState::EndTranslocationPotionTimer, TimeValue);
}

void APotProbPlayerState::DoWheelOfFortune()
{
	APotProbGameMode* PotionGameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode());
	if (!PotionGameMode)
	{
		return;
	}
	TArray<FRecipeStruct> RareSubset;
	for (FRecipeStruct Recipe : PotionGameMode->GetPotionRecipes()) {
		if (Recipe.Potion.GetDefaultObject()->GetPotionRarity() == "Rare" && Recipe.Potion.GetDefaultObject()->GetPotionName() != "WheelOfFortunePotion_BP_C") {
			RareSubset.Add(Recipe);
		}
	}

	int RandomIndex = FMath::RandRange(0, RareSubset.Num() - 1);
	APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController());
	if (PC)
	{
		PC->PlayStopWheel();
	}
	this->ServerReceivePotion(RareSubset[RandomIndex].Potion);
}

UUserWidget* APotProbPlayerState::AddPotionEffectIconToHUD(UTexture2D* StatusIconTexture, float EffectTime)
{
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController()))
	{
		if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(PC->GetCharacter()))
		{
			if (PlayerCharacter->PotionStatusWidgetClass)
			{
				UUserWidget* StatusIconInstance = CreateWidget<UUserWidget>(PC, PlayerCharacter->PotionStatusWidgetClass);
				PC->HUDWidgetInstance->StatusEffectVerticalBox->AddChild(StatusIconInstance);

				if (UPotionStatusWidget* StatusWidget = Cast<UPotionStatusWidget>(StatusIconInstance))
				{
					if (StatusIconInstance)
					{
						FSlateBrush Brush;
						Brush.SetResourceObject(StatusIconTexture);
						StatusWidget->CurrentPotionEffectImage->SetBrush(Brush);
					}
					StatusWidget->SetInitialStatusTime(EffectTime);
					StatusWidget->SetTimRemaining(EffectTime);
				}

				return StatusIconInstance;
			}
		}
	}
	return nullptr;
}

void APotProbPlayerState::RemovePotionEffectIconFromHUD(UUserWidget* StatusIconInstance)
{
	if (StatusIconInstance && IsValid(StatusIconInstance))
	{
		//if (StatusIconInstance->IsInViewport())
		//{
		StatusIconInstance->RemoveFromParent();
		//}
		StatusIconInstance->Destruct();
	}
}


void APotProbPlayerState::ClearUnstablePotionEffects_Implementation()
{
	bUnstablePotionActive = false;
	GetWorld()->GetTimerManager().ClearTimer(UnstablePotionTimer);
	UE_LOG(LogTemp, Log, TEXT("Unstable Potion Deactivation: Clearing Unstable Potion Effects"));
	ClearPotionEffects();
	
}

void APotProbPlayerState::StartUnstablePotion_Implementation()
{
	if (bUnstablePotionActive)
	{
		UE_LOG(LogTemp, Log, TEXT("Overriding unstable potion effects"));
		ClearUnstablePotionEffects();
	}
	
	ActivatePotionEffectsFromBucket();

	ClientStartUnstablePotion();

	bUnstablePotionActive = true;
	UE_LOG(LogTemp, Log, TEXT("Activated unstable potion effects"));
	GetWorld()->GetTimerManager().SetTimer(UnstablePotionTimer, this, &APotProbPlayerState::ClearUnstablePotionEffects,
										   120.0f, false);
}

void APotProbPlayerState::ClientStartUnstablePotion_Implementation()
{
	if (APotProbZDCharacter* CharPtr = Cast<APotProbZDCharacter>(GetPlayerController()->GetCharacter()))
	{
		AddPotionEffectIconToHUD(CharPtr->UnstableStatusIconTexture, 120.0f);
	}
}


void APotProbPlayerState::Server_IncrementTelescopeSuccesses_GameState_Implementation()
{
	if (APotProbGameState* GS = Cast<APotProbGameState>(GetWorld()->GetGameState<APotProbGameState>()))
	{
		if (!GS->IncrementTelescopeSuccesses())
		{
			ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Great job! Align the rest of the\ntelescopes to finish casting the spell!");
		}
	}
}

void APotProbPlayerState::UpdateCauldronMapMarker_Implementation()
{
}

void APotProbPlayerState::ActivateSwitcherooWarning_Implementation()
{
	// get player controller
	APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetWorld()->GetFirstPlayerController());
	
	// show cauldron switcheroo warning for 3 seconds
	SwitcherooWarningWidget = CreateWidget(PC, PC->GetSwitcherooWarningWidgetClass());
	if (SwitcherooWarningWidget)
	{
		SwitcherooWarningWidget->AddToViewport();
	}
	FTimerHandle SwitcherooWidgetTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(SwitcherooWidgetTimerHandle, this, &APotProbPlayerState::RemoveSwitcherooWarning, 5.0f, false);
}

void APotProbPlayerState::RemoveSwitcherooWarning_Implementation()
{
    if (SwitcherooWarningWidget)
    {
        SwitcherooWarningWidget->RemoveFromParent();
        SwitcherooWarningWidget = nullptr;
    }
}

void APotProbPlayerState::ActivateWerefrogWinConCountdown_Implementation(TSubclassOf<UPotionObject> FrogPotionClass)
{
	if (PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
	{
		APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetWorld()->GetFirstPlayerController());
		
		// show countdown to werefrog
		WerefrogCountdownWidget = CreateWidget(PC, PC->GetWerefrogCountdownWidgetClass());
		if (WerefrogCountdownWidget)
		{
			WerefrogCountdownWidget->AddToViewport();
		}

		// start 10 second timer for werefrog transformation
		FTimerHandle WerefrogWidgetTimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("ActivateWerefrogWinConEffect"), FrogPotionClass);
		GetWorld()->GetTimerManager().SetTimer(WerefrogWidgetTimerHandle, TimerDelegate, 10.0f, false);

		// alert players
		ServerReceiveAlertMessage(EPotProbAlertTypes::HIGH_PRIORITY, "Troublemakers are transforming into\nWerefrogs in 10s!");
	}
}

void APotProbPlayerState::ActivateWerefrogWinConEffect_Implementation(TSubclassOf<UPotionObject> FrogPotionClass)
{
	if (APlayerController* PC = GetPlayerController())
	{
		if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(PC->GetCharacter()))
		{
			if (PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
			{
				ServerReceivePotion(FrogPotionClass);
				PlayerCharacter->ServerTransformIntoWerefrog();
			}
		}
	}
	
	// alert players
	ServerReceiveAlertMessage(EPotProbAlertTypes::HIGH_PRIORITY, "WATCH OUT FOR WEREFROGS!");
}

void APotProbPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APotProbPlayerState, PotProbRole);
	DOREPLIFETIME(APotProbPlayerState, bIsFrogged);
	DOREPLIFETIME(APotProbPlayerState, bCanFroggedBeDiscovered);
	DOREPLIFETIME(APotProbPlayerState, bHasVoted);
	DOREPLIFETIME(APotProbPlayerState, VoteCount);
	DOREPLIFETIME(APotProbPlayerState, PlayerPotion);
	DOREPLIFETIME(APotProbPlayerState, bHasPotionBeenActivated);
	DOREPLIFETIME(APotProbPlayerState, bCanInteractWithObjects);
	DOREPLIFETIME(APotProbPlayerState, IngredientName);
	DOREPLIFETIME(APotProbPlayerState, bHoldingIngredient);
	DOREPLIFETIME(APotProbPlayerState, IngredientIcon);
	DOREPLIFETIME_CONDITION_NOTIFY(APotProbPlayerState, bIsReady, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME(APotProbPlayerState, numRerolls);
	DOREPLIFETIME(APotProbPlayerState, CurrentRecipe);
	DOREPLIFETIME(APotProbPlayerState, bHasCooties);
	DOREPLIFETIME(APotProbPlayerState, bHasCootiesBefore);
	DOREPLIFETIME(APotProbPlayerState, bIsCootiesInstigator);
	DOREPLIFETIME(APotProbPlayerState, bUnstablePotionActive);
	DOREPLIFETIME(APotProbPlayerState, bQuestionmarkPotionActive);
	DOREPLIFETIME(APotProbPlayerState, CurrentPotionChargeType);
	DOREPLIFETIME(APotProbPlayerState, NumPotionCharges);
	DOREPLIFETIME(APotProbPlayerState, bIsDisguised);
	DOREPLIFETIME(APotProbPlayerState, bIsUsingEgo);
	DOREPLIFETIME(APotProbPlayerState, FrogRecipe);
	DOREPLIFETIME(APotProbPlayerState, NumTimesRecipeRerolled);
	DOREPLIFETIME(APotProbPlayerState, bCanInitVote);
	DOREPLIFETIME(APotProbPlayerState, bHasInitVote);
	DOREPLIFETIME(APotProbPlayerState, bPlayAgain);
	DOREPLIFETIME(APotProbPlayerState, NumApprenticesFrogged);
	DOREPLIFETIME(APotProbPlayerState, NumVotingRoundsSurvived);
	DOREPLIFETIME(APotProbPlayerState, NumPotionsCrafted);
	DOREPLIFETIME(APotProbPlayerState, NumTroublemakersVoted);
	DOREPLIFETIME(APotProbPlayerState, PlayersVotingHistory);
	DOREPLIFETIME(APotProbPlayerState, NumRoundVotedOutOn);
	DOREPLIFETIME(APotProbPlayerState, bWasVotedOut);
}

void APotProbPlayerState::EndTranslocationPotionTimer()
{
	if(!HasAuthority())
	{
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(TranslocationPotionTimer);
	NumPotionCharges = 0;
	CurrentPotionChargeType = EPotionChargeType::CHARGE_NONE;
}

void APotProbPlayerState::OnRep_HasCooties()
{
	APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetPlayerController());
	if (!PC)
	{
		return;
	}
	if (bHasCooties)
	{
		bHasCootiesBefore = true;
	}
	if (PC->HUDWidgetInstance)
	{
		PC->HUDWidgetInstance->DisplayCootiesData(bHasCooties);
	}
}

void APotProbPlayerState::ServerReceiveChatMessage_Implementation(const FString& MessageSenderName, EAnimModels Model,
	const FString& MessageContent)
{
	APotProbGameState* CurrGameState = Cast<APotProbGameState>(GetWorld()->GetGameState());
	for (const auto Iter : CurrGameState->PlayerArray)
	{
		if (Iter.Get())
		{
			APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(Iter.Get());
			if (!PlayerState)
			{
				continue;
			}

			PlayerState->ClientReceiveChatMessage(MessageSenderName, Model, MessageContent);
		}
	}
}

void APotProbPlayerState::ClientReceiveChatMessage_Implementation(const FString& MessageSenderName, EAnimModels Model,
	const FString& MessageContent)
{
	APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(GetPlayerController());

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Player Controller is not of type APotProbPlayerController, PotProbPlayerState."))
			return;
	}
	//PlayerController->HUDWidgetInstance->AddChatMessage(MessageSenderName, MessageContent);
	if (PlayerController->VoteWidgetInstance)
	{
		if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(GetPawn()))
		{
			PlayerController->VoteWidgetInstance->AddChatMessage(GetPlayerName(), MessageSenderName, Model, MessageContent);
		}
	}
}

void APotProbPlayerState::ServerReceiveAlertMessage_Implementation(const EPotProbAlertTypes& AlertType,
	const FString& MessageContent,
	bool bTriggerOnInstigator)
{
	APotProbGameState* CurrGameState = Cast<APotProbGameState>(GetWorld()->GetGameState());
	for (const auto Iter : CurrGameState->PlayerArray)
	{
		if (Iter.Get())
		{
			APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(Iter.Get());
			if (!PlayerState)
			{
				continue;
			}
			if (!bTriggerOnInstigator && PlayerState == this)
			{
				continue;
			}
			PlayerState->ClientReceiveAlertMessage(AlertType, MessageContent);
		}
	}
}

void APotProbPlayerState::ClientReceiveAlertMessage_Implementation(const EPotProbAlertTypes& AlertType,
	const FString& MessageContent)
{
	// don't show tutorial alerts if toggled off in settings
	if (AlertType == EPotProbAlertTypes::TUTORIAL_ALERT)
	{
		if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
		{
			if (!SettingsSubsystem->GetCurrentSettings()->DisplayTutorial)
			{
				return;
			}
		}
	}

	APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(GetPlayerController());
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Player Controller is not of type APotProbPlayerController, PotProbPlayerState."))
			return;
	}
	PlayerController->HUDWidgetInstance->AddAlertSignalMessage(AlertType, MessageContent);
	if (MessageContent == "A fresh frog has been discovered!\nVoting starts in %i seconds!") {
		PlayerController->ChangeMusic(EPotProbPhases::PHASE_VOTE);
	}
}