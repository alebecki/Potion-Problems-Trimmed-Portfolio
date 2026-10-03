// Fill out your copyright notice in the Description page of Project Settings.


#include "PotProbPlayerController.h"

#include "AITypes.h"
#include "Components/WidgetComponent.h"
#include "ProxyChatWidget.h"
#include "CharacterCustomizeWidget.h"
#include "CharacterData.h"
#include "DisguiseHUDWidget.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "PotProbZDCharacter.h"
#include "HUDWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "PotProbPlayerState.h"
#include "Net/VoiceConfig.h"
#include "PotProbGameState.h"
#include "VoteWidget.h"
#include "PotionObject.h"
#include "InteractSubsystem.h"
#include "PotProbGameMode.h"
#include "Components/Border.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AkGameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PlayerRoleWidget.h"
#include "PotionRecipeSelectionWidget.h"
#include "InteractComponent.h"
#include "MinigameInitActor.h"
#include "MinigameInitUIActor.h"
#include "IngredientActor.h"
#include "Mirror.h"
#include "NiagaraCommon.h"
#include "VoteCountdownWidget.h"
#include "NiagaraComponent.h"
#include "Pedestal.h"
#include "PotProbOnlineSubsystem.h"
#include "RecipeWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/UnrealType.h"
#include "ProgressBarWidget.h"
#include "GameFramework/HUD.h"

APotProbPlayerController::APotProbPlayerController()
{
}

void APotProbPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APotProbPlayerController, bVotingUIVisible);
	DOREPLIFETIME(APotProbPlayerController, InteractionDistance);
}

void APotProbPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!HUDWidgetInstance)
	{
		return;
	}
	if (!IsLocalController())
	{
		return;
	}
	if (!GetPlayerState<APotProbPlayerState>())
	{
		return;
	}
	int PingValue = GetPlayerState<APotProbPlayerState>()->GetPingInMilliseconds();
	HUDWidgetInstance->PingMS->SetText(FText::FromString(FString::FromInt(PingValue)));
}

TSubclassOf<UUserWidget> APotProbPlayerController::GetWerefrogCountdownWidgetClass()
{
	if (!WerefrogCountdownWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PotProbPlayerController: WerefrogCountdownWidgetClass is not set."));
		return nullptr;
	}

	return WerefrogCountdownWidgetClass;
}

TSubclassOf<UUserWidget> APotProbPlayerController::GetSwitcherooWarningWidgetClass()
{
	if (!SwitcherooWarningWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PotProbPlayerController: SwitcherooWarningWidgetClass is not set."));
		return nullptr;
	}

	return SwitcherooWarningWidgetClass;
}

void APotProbPlayerController::DisableTelescopeInitActor_Implementation(AMinigameInitActor* InitActor)
{
	InitActor->DeactivateTelescope();
}

void APotProbPlayerController::Client_UpdateLocalLobbyStatusUI_Implementation()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->UpdateLobbyStatus();
	}
}

void APotProbPlayerController::Client_UpdateLocalVoteWidget_Implementation(bool bUpdateVotingList, bool bCanVote, bool bIsTieBreaker, const TArray<APotProbPlayerState*>& TiedPlayers, bool bResetPlayerSelected)
{
	if (VoteWidgetInstance)
	{
		if (bResetPlayerSelected)
		{
			VoteWidgetInstance->UpdatePlayerSelectedText("N/A");
		}
		VoteWidgetInstance->UpdateVoteText();
		if (bUpdateVotingList)
		{
			if (!bCanVote) {
				if (VoteSound) {
					FOnAkPostEventCallback nullCallback;
					UAkGameplayStatics::PostEvent(VoteSound, this, int32(0), nullCallback);
				}
			}
			VoteWidgetInstance->BuildVotingList(bCanVote, bIsTieBreaker, TiedPlayers);
		}
	}
}

void APotProbPlayerController::Server_ChangeInteractDistance_Implementation(bool bIsWerefrog)
{
	InteractionDistance = bIsWerefrog ? DefaultInteractDistance : WerefrogInteractDistance;
}

void APotProbPlayerController::DEBUG_Server_StartGame_Implementation()
{
	if (APotProbGameMode* GameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->DEBUG_StartGame();
	}
}


void APotProbPlayerController::Client_ShowPlayerStartRole_Implementation(EPotProbRoles PlayerRole, int32 TroubleMakerCount)
{
	// don't make duplicates
	if (PlayerRoleWidgetInstance != nullptr)
	{
		return;
	}
	
	FString PlayerRoleText;
	FString ActionOneText;
	FString ActionTwoText;
	FString ActionThreeText;

	FString TroublemakerCountText = TroubleMakerCount == 1
		                                ? TEXT("There is ") + FString::FromInt(TroubleMakerCount) +
		                                TEXT(" Troublemaker in this game.")
		                                : TEXT("There are ") + FString::FromInt(TroubleMakerCount) + TEXT(
			                                " Troublemakers in this game.");

	switch (PlayerRole)
	{
	case EPotProbRoles::ROLE_APPRENTICE:
		PlayerRoleText = TEXT("You are an apprentice!");
		ActionOneText = TEXT("Find ingredients!");
		ActionTwoText = TEXT("Make potions!");
		ActionThreeText = TEXT("Vote out Troublemakers!");
		break;
	case EPotProbRoles::ROLE_TROUBLEMAKER:
		PlayerRoleText = TEXT("You are a troublemaker!");
		ActionOneText = TEXT("Craft secret frog potions!");
		ActionTwoText = TEXT("Turn Apprentices into frogs!");
		ActionThreeText = TEXT("Don't get caught!");
		break;
	default:
		break;
	}
	
	PlayerRoleWidgetInstance = CreateWidget<UPlayerRoleWidget>(this, PlayerRoleClass);
	if (!PlayerRoleWidgetInstance)
	{
		return;
	}
	
	// alert on first role popup
    PlayerRoleWidgetInstance->bAlertOnClose = bFirstRoleWidgetInstance;
    bFirstRoleWidgetInstance = false;
    
	PlayerRoleWidgetInstance->SetPlayerInformation(PlayerRoleText, ActionOneText, ActionTwoText, ActionThreeText,
	                                       TroublemakerCountText);
	PlayerRoleWidgetInstance->AddToViewport();
}

void APotProbPlayerController::Client_ToggleVotingUI_Implementation(bool Visibility)
{
	bVotingUIVisible = Visibility;
	if (VoteWidgetInstance)
	{
		if (bVotingUIVisible)
		{
			VoteWidgetInstance->SetVisibility(ESlateVisibility::Visible);
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(VoteWidgetInstance->TakeWidget());
			SetInputMode(InputMode);
			SetShowMouseCursor(true);
			Client_UpdateLocalVoteWidget(true, false, false, {}, true);
		}
		else
		{
			VoteWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
			bShowMouseCursor = false;
			SetInputMode(FInputModeGameAndUI());
		}
	}
}

void APotProbPlayerController::Server_StartVotingPhase_Implementation()
{
	if (APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
	{
		GameState->StartVotingPhase();

		if (APotProbPlayerState* PS = GetPlayerState<APotProbPlayerState>())
		{
			PS->Client_RemoveMinigameInputBindings();
		}
	}
}


void APotProbPlayerController::Server_ProcessVote_Implementation(const FString& PlayerName, const FString& VoterName)
{
	if (APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
	{
		GameState->ProcessVote(PlayerName, VoterName);
	}
}

void APotProbPlayerController::Server_UpdateAllVoteWidgets_Implementation()
{
	if (APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
	{
		
		GameState->UpdateVotingStatus(true, GameState->bCanVote);
	}
}

void APotProbPlayerController::Server_StartVoteCountdown_Implementation()
{
	if (VoteCountdownWidgetClass)
	{
		Multicast_VoteCountdownWidgetPopUp();
	}
}


void APotProbPlayerController::Multicast_VoteCountdownWidgetPopUp_Implementation()
{
	if (VoteCountdownWidgetClass && IsLocalController())
	{
		if (UVoteCountdownWidget* VoteCountdownWidget = CreateWidget<UVoteCountdownWidget>(this, VoteCountdownWidgetClass))
		{
			VoteCountdownWidget->AddToViewport(100);
		}
	}
}

void APotProbPlayerController::FilloutPotionDistributionUI(const TArray<FRecipeStruct>& DistributedRecipes)
{
	if (AssignmentSelectionInstance.Get())
	{
		AssignmentSelectionInstance->InitializeRecipes(DistributedRecipes);
	}
}

void APotProbPlayerController::AddBackInputComponent()
{
	SetupInputComponent();
}

void APotProbPlayerController::SelectCharacter(const FCharacterData& CharacterData)
{
	Server_RequestModelUpdate(static_cast<int32>(CharacterData.CharacterModel));
	UE_LOG(LogTemp, Log, TEXT("Player %s has requested skin change to model %d"), *CharacterData.CharacterName.ToString(),
		   static_cast<int32>(CharacterData.CharacterModel));
}

void APotProbPlayerController::Server_RequestModelUpdate_Implementation(int32 modelIndex)
{
	if (APotProbGameState* GameState = GetWorld() != nullptr ? GetWorld()->GetGameState<APotProbGameState>() : nullptr)
	{
		if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(GetCharacter()))
		{
			GameState->RequestModelUpdate(PlayerCharacter, modelIndex);
		}
	}
}

void APotProbPlayerController::LeaveSessionAndReturnToMenu_Implementation()
{
	IOnlineSessionPtr SessionInterface = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>()->SessionInterface;
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* Session = SessionInterface->GetNamedSession(NAME_GameSession);
		if (Session)
		{
			//Session->NumOpenPublicConnections++;
			//SessionInterface->UpdateSession(NAME_GameSession, Session->SessionSettings);
			// For host, first end the session
			SessionInterface->EndSession(NAME_GameSession);
                
			// Then destroy after a small delay
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle, 
				[this, SessionInterface]() {
					// Add delegate before destroying
					SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
						FOnDestroySessionCompleteDelegate::CreateUObject(this, &APotProbPlayerController::OnSessionDestroyed)
					);
					SessionInterface->DestroySession(NAME_GameSession);
				},
				0.5f, // Half second delay
				false
			);
		}
	}
}

void APotProbPlayerController::LeaveSessionAndCloseGame_Implementation()
{
	IOnlineSessionPtr SessionInterface = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>()->SessionInterface;
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* Session = SessionInterface->GetNamedSession(NAME_GameSession);
		if (Session)
		{
			//Session->NumOpenPublicConnections++;
			//SessionInterface->UpdateSession(NAME_GameSession, Session->SessionSettings);
			// For host, first end the session
			SessionInterface->EndSession(NAME_GameSession);
                
			// Then destroy after a small delay
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle, 
				[this, SessionInterface]() {
					// Add delegate before destroying
					SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
						FOnDestroySessionCompleteDelegate::CreateUObject(this, &APotProbPlayerController::OnSessionDestroyed)
					);
					SessionInterface->DestroySession(NAME_GameSession);
				},
				0.5f, // Half second delay
				false
			);
		}
	}
}


void APotProbPlayerController::OnSessionDestroyed(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		Client_ReturnToMainMenu();
	}
}

void APotProbPlayerController::OnSessionLeft(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		Client_ReturnToMainMenu();
	}
}

void APotProbPlayerController::OnSessionDestroyedQuitGame(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}


void APotProbPlayerController::Client_ReturnToMainMenu_Implementation()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
}


void APotProbPlayerController::UpdateLocalCharCustomUI()
{
	if (CharacterCustomizeWidgetInstance)
	{
		CharacterCustomizeWidgetInstance->UpdateCharCustomStatus();

		if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(GetCharacter()))
		{
			FCharacterData CharacterData = CharacterCustomizeWidgetInstance->GetCharacterDataFromModel(PlayerCharacter->GetPlayerSelectedModel());
			
			HUDWidgetInstance->UpdateCharacterSprite(CharacterData);
			CharacterCustomizeWidgetInstance->SelectCharacter(CharacterData);
		}
	}
}

void APotProbPlayerController::Client_UpdateLoopReason_Implementation(bool bKickAll)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->UpdateLoopReason(bKickAll);
	}
}

void APotProbPlayerController::Client_IncrementHUDGameState_Implementation()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->IncrementHUDGameState();
	}
}

void APotProbPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->ClearActionBindings();
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,
		                                   &APotProbPlayerController::OnMove, ETriggerEvent::Triggered);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this,
		                                   &APotProbPlayerController::MoveCompleted, ETriggerEvent::Completed);
		EnhancedInputComponent->BindAction(ProxyChatAction, ETriggerEvent::Triggered, this,
		                                   &APotProbPlayerController::OnProxyChatAction);
		EnhancedInputComponent->BindAction(DrinkPotionAction, ETriggerEvent::Triggered, this,
		                                   &APotProbPlayerController::OnDrinkPotion, ETriggerEvent::Triggered);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this,
		                                   &APotProbPlayerController::OnInteractAction, ETriggerEvent::Triggered);
		EnhancedInputComponent->BindAction(MapAction, ETriggerEvent::Triggered, this,
		                                   &APotProbPlayerController::OnMapAction, ETriggerEvent::Triggered);
		EnhancedInputComponent->BindAction(ChargeUseAction, ETriggerEvent::Triggered, this,
		                                   &APotProbPlayerController::OnUseChargesAction, ETriggerEvent::Triggered);
		EnhancedInputComponent->BindAction(InteractHeldAction, ETriggerEvent::Triggered, this,
											&APotProbPlayerController::OnInteractSuperHeld, ETriggerEvent::Triggered);
		EnhancedInputComponent->BindAction(InteractHeldAction, ETriggerEvent::Started, this,
											&APotProbPlayerController::OnInteractSuperHeldStarted, ETriggerEvent::Started);
		EnhancedInputComponent->BindAction(InteractHeldAction, ETriggerEvent::Ongoing, this,
											&APotProbPlayerController::OnInteractSuperHeldOngoing, ETriggerEvent::Ongoing);
		EnhancedInputComponent->BindAction(InteractHeldAction, ETriggerEvent::Canceled, this,
        											&APotProbPlayerController::OnInteractSuperHeldCanceled, ETriggerEvent::Canceled);
		EnhancedInputComponent->BindAction(BottleUpAction, ETriggerEvent::Triggered, this,
											&APotProbPlayerController::OnInteractHeld, ETriggerEvent::Triggered);
		                                  
										
		EnhancedInputComponent->BindAction(DiscardIngredientAction, ETriggerEvent::Triggered, this, &APotProbPlayerController::OnDiscardIngredientAction, ETriggerEvent::Triggered);
	}
}

void APotProbPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI inputMode;
	inputMode.SetHideCursorDuringCapture(false);
	SetInputMode(inputMode);

	
	if (IsLocalController())
	{
		if (HUDWidgetClass)
		{
			HUDWidgetInstance = NewObject<UHUDWidget>(this, HUDWidgetClass);
			HUDWidgetInstance->AddToViewport();
			HUDWidgetInstance->SetOwningPlayer(this);
			if(UPotProbOnlineSubsystem* Olss = GetWorld()->GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>())
			{
				if (Olss->bUseBigMap)
				{
					HUDWidgetInstance->UpdateMapText("Big Map");
				}
				else
				{
					HUDWidgetInstance->UpdateMapText("Small Map");
				}
			}
		}
		if (VoteWidgetClass)
		{
			VoteWidgetInstance = NewObject<UVoteWidget>(this, VoteWidgetClass);
			VoteWidgetInstance->AddToViewport();
			VoteWidgetInstance->SetOwningPlayer(this);
			VoteWidgetInstance->SetFocus();
			Client_ToggleVotingUI(false);
		}
		if (AssignmentSelectionClass)
		{
			AssignmentSelectionInstance = NewObject<UPotionRecipeSelectionWidget>(this, AssignmentSelectionClass);
			AssignmentSelectionInstance->AddToViewport();
			AssignmentSelectionInstance->SetOwningPlayer(this);
			AssignmentSelectionInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (CharacterCustomizeWidgetClass)
		{
			CharacterCustomizeWidgetInstance = NewObject<
				UCharacterCustomizeWidget>(this, CharacterCustomizeWidgetClass);
			CharacterCustomizeWidgetInstance->AddToViewport();
			CharacterCustomizeWidgetInstance->SetOwningPlayer(this);
			CharacterCustomizeWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (DisguiseHUDWidgetClass)
		{
			DisguiseHUDWidgetInstance = NewObject<UDisguiseHUDWidget>(this, DisguiseHUDWidgetClass);
			DisguiseHUDWidgetInstance->AddToViewport();
			DisguiseHUDWidgetInstance->SetOwningPlayer(this);
			DisguiseHUDWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	bShowMouseCursor = true;
}

void APotProbPlayerController::OnInputStarted()
{
}

void APotProbPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	TArray<FString> RecipeMessages;
	if (aPawn)
	{
		ACharacter* PossessedCharacter = Cast<ACharacter>(aPawn);
		if (PossessedCharacter)
		{
			UE_LOG(LogTemp, Log, TEXT("Possessed character: %s"), *PossessedCharacter->GetName());
			// Now you can safely access the character

			APotProbZDCharacter* Char = Cast<APotProbZDCharacter>(PossessedCharacter);
			Char->SetPlayerText(PlayerState->GetPlayerName());
		}
	}
	Client_OnPossess();
}

void APotProbPlayerController::Client_OnPossess_Implementation()
{
	if (!HUDWidgetClass)
	{
		UE_LOG(LogTemp, Error,
		       TEXT("Warning: PotProbPlayerController HUDWidgetClass not implemented. Check Blueprint for details."));
		return;
	}

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this);
}

void APotProbPlayerController::OnMove(const FInputActionInstance& Instance, const ETriggerEvent EventType)
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return;
	}

	APotProbZDCharacter* MyCharacter = Cast<APotProbZDCharacter>(GetCharacter());
	if (!MyCharacter)
	{
		return;
	}
	FVector2D MovementVector = Instance.GetValue().Get<FVector2D>();
	MyPawn->AddMovementInput(FVector::ForwardVector, MovementVector.X);
	MyPawn->AddMovementInput(-FVector::RightVector, MovementVector.Y);

	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->UpdatePlayerPositionIcon(MyCharacter->GetActorLocation());
	}

	Server_UpdateCharacterDirection(MovementVector);
	Server_UpdateIsHoldingMove(EventType);
}

void APotProbPlayerController::OnProxyChatAction(const FInputActionInstance& Instance)
{
	if (APotProbZDCharacter* PotCharacter = Cast<APotProbZDCharacter>(GetCharacter()))
	{
		if (UProxyChatWidget* ChatWidget = Cast<UProxyChatWidget>(PotCharacter->ProxyChatComponent->GetWidget()))
		{
			if (bCanProxyChat)
			{
				bCanProxyChat = false;
				ChatWidget->ProxyTextBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				ChatWidget->ProxyTextBox->SetFocus();
				
				FTimerHandle ProxyTimeoutHandle;
				GetWorld()->GetTimerManager().SetTimer(ProxyTimeoutHandle, this, &APotProbPlayerController::SetCanProxyChat, ProxyTimeoutTime );
			}
		}
	}
}

void APotProbPlayerController::MoveCompleted(const FInputActionValue& Value, const ETriggerEvent EventType)
{
	Server_UpdateIsHoldingMove(EventType);
}

void APotProbPlayerController::OnDrinkPotion(const FInputActionInstance& Instance, const ETriggerEvent EventType)
{
	Server_AttemptActivatePotion();
}

void APotProbPlayerController::OnInteractAction(const FInputActionInstance& Instance, const ETriggerEvent EventType)
{
	APotProbPlayerState* PotionPlayerState = GetPlayerState<APotProbPlayerState>();
	if (!PotionPlayerState)
	{
		return;
	}
	if (PotionPlayerState->GetCanInteractWithObjects())
	{
		APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(GetCharacter());
		if (!PlayerCharacter)
		{
			return;
		}

		// Remove Potion Widget Popup if it exits
		if (HUDWidgetInstance)
		{
			if (HUDWidgetInstance->RemovePotionWidget())
			{
				return;
			}
		}
		
		APawn* CharPawn = GetPawn();
		if (!CharPawn)
		{
			return;
		}
		if (UInteractSubsystem* InteractSubsystem = GetWorld()->GetSubsystem<UInteractSubsystem>())
		{
			UInteractComponent* InteractComponent = InteractSubsystem->GetBestCandidate().Get();
			if (!InteractComponent)
			{
				return;
			}

			// Handle Lobby interactable
			if (APedestal* Pedestal = Cast<APedestal>(InteractComponent->GetOwner()))
			{
				if (HUDWidgetInstance)
				{
					HUDWidgetInstance->AddTutorialFlipBookToViewport();
					return;
				}
			}
			if (AMirror* Mirror = Cast<AMirror>(InteractComponent->GetOwner()))
			{
				if (CharacterCustomizeWidgetInstance)
				{
					CharacterCustomizeWidgetInstance->SetVisibility(ESlateVisibility::Visible);
					return;
				}
			}
			
			Server_AttemptInteractWithObject(CharPawn, InteractComponent);
		}
	}
}

void APotProbPlayerController::OnMapAction(const FInputActionInstance& Instance, const ETriggerEvent EventType)
{
	if (HUDWidgetInstance->Map->GetVisibility() == ESlateVisibility::Hidden)
	{
		HUDWidgetInstance->OnMapButtonClicked();
	}
	else
	{
		HUDWidgetInstance->OnCloseButtonClicked();
	}
}

void APotProbPlayerController::OnUseChargesAction(const FInputActionInstance& Instance, const ETriggerEvent EventType)
{
	APotProbPlayerState* ProbPlayerState = GetPlayerState<APotProbPlayerState>();
	if (!ProbPlayerState)
	{
		return;
	}

	Server_AttemptUseCharges(ProbPlayerState->GetPotionChargesType(), ProbPlayerState->GetNumPotionCharges());
}

void APotProbPlayerController::OnInteractHeld(const FInputActionInstance& Instance, const ETriggerEvent EventType)
{
	//gotta add a check for if ive interacted recently, maybe in the ingredient actor???????
	APotProbPlayerState* PotionPlayerState = GetPlayerState<APotProbPlayerState>();
	if (!PotionPlayerState)
	{
		return;
	}
	if (PotionPlayerState->GetCanInteractWithObjects())
	{
		APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(GetCharacter());
		if (!PlayerCharacter)
		{
			return;
		}
		APawn* CharPawn = GetPawn();
		if (!CharPawn)
		{
			return;
		}
		if (UInteractSubsystem* InteractSubsystem = GetWorld()->GetSubsystem<UInteractSubsystem>())
		{
			UInteractComponent* InteractComponent = InteractSubsystem->GetBestCandidate().Get();
			if (!InteractComponent)
			{
				return;
			}
			Server_AttemptHoldInteractWithObject(CharPawn, InteractComponent);
		}
	}
}

void APotProbPlayerController::OnInteractSuperHeld(const FInputActionInstance& Instance, const ETriggerEvent EventType)
{
	// remove super held UI progress bar
	if (SuperHeldProgressBarWidget)
	{
		SuperHeldProgressBarWidget->RemoveFromParent();
		SuperHeldProgressBarWidget = nullptr;
	}
	
	APotProbPlayerState* PotionPlayerState = GetPlayerState<APotProbPlayerState>();
	if (!PotionPlayerState)
	{
		return;
	}
	if (PotionPlayerState->GetCanInteractWithObjects())
	{
		APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(GetCharacter());
		if (!PlayerCharacter)
		{
			return;
		}
		APawn* CharPawn = GetPawn();
		if (!CharPawn)
		{
			return;
		}
		if (UInteractSubsystem* InteractSubsystem = GetWorld()->GetSubsystem<UInteractSubsystem>())
		{
			UInteractComponent* InteractComponent = InteractSubsystem->GetBestCandidate().Get();
			if (!InteractComponent)
			{
				return;
			}
			Server_AttemptSuperHoldInteractWithObject(CharPawn, InteractComponent);
		}
	}
}

void APotProbPlayerController::OnInteractSuperHeldStarted(const FInputActionInstance& Instance, const ETriggerEvent EventType)
{
	if (SuperHeldProgressBarWidget != nullptr)
	{
		SuperHeldProgressBarWidget->RemoveFromParent();
		SuperHeldProgressBarWidget = nullptr;
	}

	if (!SuperHeldProgressBarWidgetClass)
	{
		return;
	}

	SuperHeldProgressBarWidget = Cast<UProgressBarWidget>(CreateWidget(this, SuperHeldProgressBarWidgetClass));
	SuperHeldProgressBarWidget->AddToViewport();
	
	// Get hold time threshold
	const UInputAction* InputAction = Instance.GetSourceAction();
    for (const UInputTrigger* Trigger : InputAction->Triggers)
    {
        if (const UInputTriggerHold* HoldTrigger = Cast<UInputTriggerHold>(Trigger))
        {
            SuperHeldTimeThreshold = HoldTrigger->HoldTimeThreshold;
            break;
        }
    }
}

void APotProbPlayerController::OnInteractSuperHeldOngoing(const FInputActionInstance& Instance, const ETriggerEvent EventType)
{
	if (!SuperHeldProgressBarWidget)
	{
		return;
	}

	// Update Percent in widget
	float ElapsedTime = Instance.GetElapsedTime();
	float Percent = ElapsedTime / SuperHeldTimeThreshold;
	SuperHeldProgressBarWidget->SetPercent(Percent);
}

void APotProbPlayerController::OnInteractSuperHeldCanceled(const FInputActionInstance& Instance, const ETriggerEvent EventType)
{
    // If held is cancelled, remove the progress bar
	if (SuperHeldProgressBarWidget != nullptr)
    {
        SuperHeldProgressBarWidget->RemoveFromParent();
        SuperHeldProgressBarWidget = nullptr;
    }
}

void APotProbPlayerController::OnDiscardIngredientAction(const FInputActionInstance& Instance,
	const ETriggerEvent EventType)
{
	if (APotProbPlayerState* PotProbPlayerState = Cast<APotProbPlayerState>(PlayerState))
	{
		Server_WipeIngredientName(PotProbPlayerState);
		Client_DisplayIngredientText(false);

		// Audio Alert Text
		// PotProbPlayerState->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "Dropped Ingredient");
		// PotProbPlayerState->ClientReceiveChatMessage("Alert", "Dropped Ingredient");
		FOnAkPostEventCallback nullCallback;
		UAkGameplayStatics::PostEvent(DropSound, this, int32(0), nullCallback);
	}
}

bool APotProbPlayerController::HandleOnInteractIngredientEffect(FName Ingredient)
{
	LastPickedUpIngredient = Ingredient;
	bool Result = false;
	if (APotProbZDCharacter* ZDCharacter = Cast<APotProbZDCharacter>(GetCharacter()))
	{
		/*if (Ingredient == FName("Chilly Pepper"))
		{
			ZDCharacter->ToggleChillyBoost(true);
			if (UNiagaraComponent* NiagaraComp = ZDCharacter->GetComponentByClass<UNiagaraComponent>())
			{
				NiagaraComp->Activate();
			}
			Result = true;
		}*/
	}
	return Result;
}

void APotProbPlayerController::HandleOnInteractPlaceEffect()
{
	if (APotProbZDCharacter* ZDCharacter = Cast<APotProbZDCharacter>(GetCharacter()))
	{
		/*if (LastPickedUpIngredient == FName("Chilly Pepper"))
		{
			ZDCharacter->ToggleChillyBoost(false);
			if (UNiagaraComponent* NiagaraComp = ZDCharacter->GetComponentByClass<UNiagaraComponent>())
			{
				NiagaraComp->Deactivate();
			}
		}*/
	}
}

void APotProbPlayerController::Client_DisplayIngredientText_Implementation(bool bVal)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->DisplayDropIngredientText(bVal);
	}
}

void APotProbPlayerController::Client_ClearHUDIngredientSelections_Implementation()
{
	if (HUDWidgetInstance)
	{
		//HUDWidgetInstance->PotionRecipeInfo->ClearIngredientSelections();
		if (APotProbPlayerState* PotProbPlayerState = Cast<APotProbPlayerState>(PlayerState))
		{
			// if troublemaker, try making frog potion too
			if (PotProbPlayerState->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
			{
				HUDWidgetInstance->FrogRecipeInfo->ClearIngredientSelections();
			}
		}
	}
}

void APotProbPlayerController::Server_WipeIngredientName_Implementation(class APotProbPlayerState* PlayerS)
{
	PlayerS->SetIsHoldingIngredient(false);
	PlayerS->SetIngredientIcon(nullptr);
	PlayerS->OnRep_IngredientIcon();
	PlayerS->SetIngredientName(NAME_None);

	HandleOnInteractPlaceEffect();
}

// We can add a cooldown to this if we would like
void APotProbPlayerController::Server_AttemptUseCharges_Implementation(EPotionChargeType PotionChargeType,
                                                                      int32 NumCharges)
{
	if (PotionChargeType == EPotionChargeType::CHARGE_NONE || NumCharges == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("You have no charges to use!"));
		return;
	}
	APotProbZDCharacter* ProbZdCharacter = Cast<APotProbZDCharacter>(GetPawn());
	if (!ProbZdCharacter)
	{
		return;
	}
	switch (PotionChargeType)
	{
	case EPotionChargeType::CHARGE_TRANSLOCATION:
		ProbZdCharacter->UpdateSpritePosition();
		break;
	default:
		return;
	}

	APotProbPlayerState* ProbPlayerState = GetPlayerState<APotProbPlayerState>();
	if (!ProbPlayerState)
	{
		return;
	}

	ProbPlayerState->ReduceNumPotionCharges();
}

void APotProbPlayerController::Client_DisplaySwitcharooMessage_Implementation()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->CauldronMessage();
	}
}

void APotProbPlayerController::Server_AttemptInteractWithObject_Implementation(
	APawn* InstigatorPawn, UInteractComponent* TargetInteract)
{
	APotProbPlayerState* PotionPlayerState = GetPlayerState<APotProbPlayerState>();
	ACauldronActor* Cauldron = Cast<ACauldronActor>(TargetInteract->GetOwner());


	// If you have cooties you cannot interact with microgames
	AMinigameInitActor* MinigameActor = Cast<AMinigameInitActor>(TargetInteract->GetOwner());
	AMinigameInitUIActor* MinigameUIActor = Cast<AMinigameInitUIActor>(TargetInteract->GetOwner());
	AIngredientActor* IngredientActor = Cast<AIngredientActor>(TargetInteract->GetOwner());
	if (PotionPlayerState->GetHasCooties() && (MinigameActor || MinigameUIActor || IngredientActor))
	{
		return;
	}
	if (UInteractSubsystem* InteractSubsystem = GetWorld()->GetSubsystem<UInteractSubsystem>())
	{

			InteractSubsystem->PerformInteract(InstigatorPawn, TargetInteract);
			Client_DisplayIngredientText(true);

	}
}

void APotProbPlayerController::Server_AttemptHoldInteractWithObject_Implementation(
	APawn* InstigatorPawn, UInteractComponent* TargetInteract)
{
	APotProbPlayerState* PotionPlayerState = GetPlayerState<APotProbPlayerState>();

	// If you have cooties you cannot interact with microgames
	AMinigameInitActor* MinigameActor = Cast<AMinigameInitActor>(TargetInteract->GetOwner());
	AMinigameInitUIActor* MinigameUIActor = Cast<AMinigameInitUIActor>(TargetInteract->GetOwner());
	AIngredientActor* IngredientActor = Cast<AIngredientActor>(TargetInteract->GetOwner());
	if (PotionPlayerState->GetHasCooties() && (MinigameActor || MinigameUIActor || IngredientActor))
	{
		return;
	}

	if (UInteractSubsystem* InteractSubsystem = GetWorld()->GetSubsystem<UInteractSubsystem>())
	{
		InteractSubsystem->PerformInteractHeld(InstigatorPawn, TargetInteract);
	}
}

void APotProbPlayerController::Server_AttemptSuperHoldInteractWithObject_Implementation(APawn* InstigatorPawn,
	UInteractComponent* TargetInteract)
{
	APotProbPlayerState* PotionPlayerState = GetPlayerState<APotProbPlayerState>();

	// If you have cooties you cannot interact with microgames
	AMinigameInitActor* MinigameActor = Cast<AMinigameInitActor>(TargetInteract->GetOwner());
	AMinigameInitUIActor* MinigameUIActor = Cast<AMinigameInitUIActor>(TargetInteract->GetOwner());
	AIngredientActor* IngredientActor = Cast<AIngredientActor>(TargetInteract->GetOwner());
	if (PotionPlayerState->GetHasCooties() && (MinigameActor || MinigameUIActor || IngredientActor))
	{
		return;
	}

	if (UInteractSubsystem* InteractSubsystem = GetWorld()->GetSubsystem<UInteractSubsystem>())
	{
		InteractSubsystem->PerformInteractSuperHeld(InstigatorPawn, TargetInteract);
	}
}

void APotProbPlayerController::Server_ChangeIngredientStatus_Implementation(const FName Name, bool bIsHoldingIngredient)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (APotProbPlayerState* PotionPlayerState = GetPlayerState<APotProbPlayerState>())
	{
		APotProbGameMode* PotionGameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode());
		if (!PotionGameMode)
		{
			return;
		}
		
		FName ServerVerifiedName = PotionGameMode->GetVerifiedIngredientName(Name);
		PotionPlayerState->SetIngredientName(ServerVerifiedName);
		PotionPlayerState->SetIngredientIcon(PotionGameMode->GetAssociatedIngredientSprite(ServerVerifiedName));
		if (ServerVerifiedName != NAME_None)
		{
			//HandleOnInteractIngredientEffect(ServerVerifiedName);
			PotionPlayerState->SetIsHoldingIngredient(bIsHoldingIngredient);
			// Audio Alert
			Cast<APotProbPlayerController>(PotionPlayerState->GetPlayerController())->PlayPickupNoise();
		}
		else
		{
			Server_WipeIngredientName(PotionPlayerState);
			// Old function used to remove player ingredient
			//PotionPlayerState->SetIsHoldingIngredient(false);
		}
		if (IsLocalController())
		{
			// Calling OnRep for update on the listen server
			PotionPlayerState->OnRep_IngredientName();
			PotionPlayerState->OnRep_IngredientIcon();
		}
	}
}

void APotProbPlayerController::Server_AttemptActivatePotion_Implementation()
{
	APotProbPlayerState* PotionPlayerState = GetPlayerState<APotProbPlayerState>();
	if (!PotionPlayerState)
	{
		return;
	}

	if (IsValid(PotionPlayerState->GetPlayerPotion()))
	{
		PotionPlayerState->SetPotionActivation(true);
		APotProbPlayerController* PC = Cast<APotProbPlayerController>(PotionPlayerState->GetPlayerController());
		if (PC) {
			PC->PlayPotionDrink(PotionPlayerState->GetPlayerPotion()->PotionSound);
		}
		PotionPlayerState->GetPlayerPotion()->ActivatePotionForCaller(PotionPlayerState);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player does not have a potion, this is either intentional or unintentional"));
	}
}

void APotProbPlayerController::Server_UpdateIsHoldingMove_Implementation(ETriggerEvent Status)
{
	APotProbZDCharacter* MyCharacter = Cast<APotProbZDCharacter>(GetCharacter());
	if (!MyCharacter)
	{
		return;
	}
	if (Status == ETriggerEvent::Triggered)
	{
		MyCharacter->SetIsHoldingMove(true);
	}
	else if (Status == ETriggerEvent::Completed)
	{
		MyCharacter->SetIsHoldingMove(false);
	}

	//DEBUG "VOTING" ONLY 
	// if (APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
	// {
	// 	GameState->StartVotingPhase();
	// }
	//END DEBUG "VOTING" ONLY
}

void APotProbPlayerController::Server_UpdateCharacterDirection_Implementation(FVector2D MovementVector)
{
	APotProbZDCharacter* MyCharacter = Cast<APotProbZDCharacter>(GetCharacter());
	if (!MyCharacter)
	{
		return;
	}
	MyCharacter->SetCharacterDirectionality(MovementVector);
}

UHUDWidget* APotProbPlayerController::GetHUDWidgetInstance()
{
	return HUDWidgetInstance;
}

void APotProbPlayerController::ToggleHUDVisibility()
{
    if(!HUDWidgetInstance)
    {
        return;
    }
      
    ESlateVisibility currVisibility = HUDWidgetInstance->GetVisibility();
    if (currVisibility == ESlateVisibility::Visible ||
        currVisibility == ESlateVisibility::HitTestInvisible ||
        currVisibility == ESlateVisibility::SelfHitTestInvisible)
    {
        HUDWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        HUDWidgetInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }
}

void APotProbPlayerController::Server_ToggleNametags_Implementation()
{
    TArray<APotProbZDCharacter*> AllCharacters; 
    
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APotProbPlayerController* PC = Cast<APotProbPlayerController>(Iterator->Get());
        if (APotProbZDCharacter* MyChar = Cast<APotProbZDCharacter>(PC->GetPawn()))
        {
            AllCharacters.Add(MyChar);
        }
    }
    
    Client_ToggleNametags(AllCharacters);
}

void APotProbPlayerController::Client_ToggleNametags_Implementation(const TArray<APotProbZDCharacter*>& AllCharacters)
{
    for (APotProbZDCharacter* MyChar : AllCharacters)
    {
        if (!bNametagsVisible)
        {
            if (APotProbPlayerState* PS = Cast<APotProbPlayerState>(MyChar->GetPlayerState()))
            {
                if (PS->GetIsUsingEgo())
                {
                    MyChar->PlayerText->SetVisibility(false);
                    MyChar->EgoPlayerNameTag->SetVisibility(true);
                }
                else
                {
                    MyChar->PlayerText->SetVisibility(true);
                    MyChar->EgoPlayerNameTag->SetVisibility(false);
                }
            }
        }
        else
        {
            MyChar->PlayerText->SetVisibility(false);
            MyChar->EgoPlayerNameTag->SetVisibility(false);
        }
    }
    bNametagsVisible = !bNametagsVisible;
}

void APotProbPlayerController::DEBUG_Server_AddApprenticeCraftedPotionByOne_Implementation()
{
	
	if (APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
	{
		GameState->SetCurrentlyCraftedNumApprenticePotions(GameState->GetCurrentlyCraftedNumApprenticePotions() + 1);
	}
}

void APotProbPlayerController::Client_ShowPlayerFroggedTutorial_Implementation()
{
	if (FroggedTutorialScreenWidgetClass)
	{
		if (UUserWidget* FroggedTutorial = CreateWidget<UUserWidget>(this, FroggedTutorialScreenWidgetClass))
		{
			FroggedTutorial->AddToViewport();
		}
	}
}


void APotProbPlayerController::PlayPotionSound_Implementation() {
	if (BottleEvent) {
		FOnAkPostEventCallback nullCallback;
		UAkGameplayStatics::PostEvent(BottleEvent, this, int32(0), nullCallback);
	}
}

void APotProbPlayerController::PlayFrogSound_Implementation() {
	if (FrogSound) {
		FOnAkPostEventCallback nullCallback;
		UAkGameplayStatics::PostEvent(FrogSound, this, int32(0), nullCallback);
	}
}

void APotProbPlayerController::PlayStartChime_Implementation() {
	if (StartChime) {
		FOnAkPostEventCallback nullCallback;
		UAkGameplayStatics::PostEvent(StartChime, nullptr, int32(0), nullCallback);
	}
}

void APotProbPlayerController::StopMusic_Implementation() {
	if (StopLobbyMusic) {
		FOnAkPostEventCallback nullCallback;
		UAkGameplayStatics::PostEvent(StopLobbyMusic, nullptr, int32(0), nullCallback);
	}
}

void APotProbPlayerController::PlayPickupNoise_Implementation() {
	if (PickupSound) {
		FOnAkPostEventCallback nullCallback;
		UAkGameplayStatics::PostEvent(PickupSound, this, int32(0), nullCallback);
	}
}

void APotProbPlayerController::PlayPotionDrink_Implementation(UAkAudioEvent* Drink)
{
	if (Drink) {
		FOnAkPostEventCallback nullCallback;
		UAkGameplayStatics::PostEvent(Drink, this, int32(0), nullCallback);
	}
}

void APotProbPlayerController::PlayVotedOutSFX_Implementation()
{
	if (VoteEndSound) {
		FOnAkPostEventCallback nullCallback;
		UAkGameplayStatics::PostEvent(VoteEndSound, nullptr, int32(0), nullCallback);
	}
}

void APotProbPlayerController::PlayReportStinger_Implementation()
{
	if (ReportStinger) {
		FOnAkPostEventCallback nullCallback;
		UAkGameplayStatics::PostEvent(ReportStinger, this, int32(0), nullCallback);
	}
}

void APotProbPlayerController::PlayStopWheel_Implementation()
{
	if (StopWheelOfFortuneLoop) {
		FOnAkPostEventCallback nullCallback;
		UAkGameplayStatics::PostEvent(StopWheelOfFortuneLoop, nullptr, int32(0), nullCallback);
	}
}

void APotProbPlayerController::PlayFootstep_Implementation()
{
	if (FootstepSound) {
		FOnAkPostEventCallback nullCallback;
		UAkGameplayStatics::PostEvent(FootstepSound, nullptr, int32(0), nullCallback);
	}
}

void APotProbPlayerController::PlayFrogstep_Implementation()
{
	if (FrogstepSound) {
		FOnAkPostEventCallback nullCallback;
		UAkGameplayStatics::PostEvent(FrogstepSound, nullptr, int32(0), nullCallback);
	}
}

void APotProbPlayerController::PlayGameOverSound_Implementation(bool ApprenticeWin)
{
	if (ApprenticeWin) {
		if (WinSoundApprentice) {
			FOnAkPostEventCallback nullCallback;
			UAkGameplayStatics::PostEvent(WinSoundApprentice, nullptr, int32(0), nullCallback);
		}
	}
	else {
		if (WinSoundTroublemaker) {
			FOnAkPostEventCallback nullCallback;
			UAkGameplayStatics::PostEvent(WinSoundTroublemaker, nullptr, int32(0), nullCallback);
		}
	}
}

void APotProbPlayerController::ChangeRoomNoise(FName Room)
{
	FAkAudioDevice* AudioDevice = FAkAudioDevice::Get();

	if (AudioDevice)
	{
		AudioDevice->SetSwitch(*FName("CurrentRoom").ToString(), *(Room).ToString(), nullptr);
	}
}

void APotProbPlayerController::ChangeMusic_Implementation(EPotProbPhases NewPhase)
{
	FAkAudioDevice* AudioDevice = FAkAudioDevice::Get();

	if (AudioDevice)
	{
		switch (NewPhase) 
		{
			case EPotProbPhases::PHASE_END:
				AudioDevice->SetState(*FName("GameMode").ToString(), *FName("GameEnd").ToString());
				break;
			case EPotProbPhases::PHASE_LOBBY:
				AudioDevice->SetState(*FName("GameMode").ToString(), *FName("Lobby").ToString());
				if (StopRoomNoise) {
					FOnAkPostEventCallback nullCallback;
					UAkGameplayStatics::PostEvent(StopRoomNoise, nullptr, int32(0), nullCallback);
				}
				break;
			case EPotProbPhases::PHASE_VOTE:
				AudioDevice->SetState(*FName("GameMode").ToString(), *FName("Voting").ToString());
				if (StopRoomNoise) {
					FOnAkPostEventCallback nullCallback;
					UAkGameplayStatics::PostEvent(StopRoomNoise, nullptr, int32(0), nullCallback);
				}
				break;
			case EPotProbPhases::PHASE_INGAME:
				AudioDevice->SetState(*FName("GameMode").ToString(), *FName("Playing").ToString());
				if (StartRoomNoise) {
					FOnAkPostEventCallback nullCallback;
					UAkGameplayStatics::PostEvent(StartRoomNoise, nullptr, int32(0), nullCallback);
				}
				break;
			case EPotProbPhases::PHASE_NONE:
				AudioDevice->SetState(*FName("GameMode").ToString(), *FName("Menu").ToString());
				break;
		}
	}
}

void APotProbPlayerController::Client_UpdateNumInGamePlayers_Implementation(int NumPlayers)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->UpdateNumInGamePlayers(NumPlayers);
	}
}

void APotProbPlayerController::Client_UpdateLocalLoopStatusUI_Implementation()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->UpdateNumLoopingPlayers();
	}
}

//###########################################################################
/*
 *	TODO: Debug Functions Need to be removed for Final Product
 */
//########################################################################### 

void APotProbPlayerController::DebugDisplayApprenticeHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->DebugChangeHUD("Apprentice");
	}
}

void APotProbPlayerController::DebugDisplayTroublemakerHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->DebugChangeHUD("Troublemaker");
	}
}

void APotProbPlayerController::DebugDisplayLobbyHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->DebugChangeHUD("Lobby");
	}
}

void APotProbPlayerController::DebugDisplayInGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->DebugChangeHUD("InGame");
	}
}

void APotProbPlayerController::DebugDisplayEndGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->DebugChangeHUD("EndGame");
	}
}

void APotProbPlayerController::DebugToggleHUD()
{
	ToggleHUDVisibility();
}

void APotProbPlayerController::DebugToggleNametags()
{
	Server_ToggleNametags();
}

void APotProbPlayerController::DebugGiveCamouflagePotion()
{
	DebugFindAndGivePotion("Camoflask");
}

void APotProbPlayerController::DebugGiveClairvoyancePotion()
{
	DebugFindAndGivePotion("Third-Eye Drops");
}

void APotProbPlayerController::DebugGiveDisguisePotion()
{
	DebugFindAndGivePotion("Poly Morphala");
}

void APotProbPlayerController::DebugGiveEgoPotion()
{
	DebugFindAndGivePotion("Ego Fizzicle");
}

void APotProbPlayerController::DebugGiveLovePotion()
{
	DebugFindAndGivePotion("Shot of Cupid");
}

void APotProbPlayerController::DebugGiveQuestionmarkPotion()
{
	DebugFindAndGivePotion("Mystery Milk");
}

void APotProbPlayerController::DebugGiveShrinkingPotion()
{
	DebugFindAndGivePotion("Gulliver's Tonic");
}

void APotProbPlayerController::DebugGiveTranslocationPotion()
{
	DebugFindAndGivePotion("Swap-o-soda");
}

void APotProbPlayerController::DebugGiveWheelOfFortunePotion()
{
	DebugFindAndGivePotion("Wine of Fortune");
}

void APotProbPlayerController::DebugGiveFrogPotion()
{
	DebugFindAndGivePotion("Frogger's Elixir");
}

void APotProbPlayerController::DebugGiveSunFriedEyeball()
{
	Server_ChangeIngredientStatus(FName("Sun Fried Eyeball"), true);
}

void APotProbPlayerController::DebugGivePhoenixClippings()
{
	Server_ChangeIngredientStatus(FName("Phoenix \"Clipping\""), true);
}

void APotProbPlayerController::DebugGiveJeff()
{
	Server_ChangeIngredientStatus(FName("Jeff"), true);
}

void APotProbPlayerController::DebugGiveNothingBagel()
{
	Server_ChangeIngredientStatus(FName("Nothing Bagel"), true);
}

void APotProbPlayerController::DebugGiveKidneyBean()
{
	Server_ChangeIngredientStatus(FName("Kidney Bean"), true);
}

void APotProbPlayerController::DebugGiveChillyPepper()
{
	Server_ChangeIngredientStatus(FName("Chilly Pepper"), true);
}

void APotProbPlayerController::DebugGiveSpirits()
{
	Server_ChangeIngredientStatus(FName("Spirits"), true);
}

void APotProbPlayerController::DebugGiveRomanceNovel()
{
	Server_ChangeIngredientStatus(FName("Romance Novel"), true);
}

void APotProbPlayerController::DebugFindAndGivePotion_Implementation(const FString& PotionName)
{
	// Find Potion by Name
	TSubclassOf<UPotionObject> PotionType = nullptr;
	if (APotProbGameMode* GameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode()))
	{
		for (TSubclassOf<UPotionObject> Potion : GameMode->GetPotionsArray())
		{
			if (Potion->GetDefaultObject<UPotionObject>()->GetPotionName() == PotionName)
			{
				PotionType = Potion;
				break;
			}
		}
	}

	// Give Player Potion
	if (PotionType)
	{
		if (APotProbPlayerState* PotProbPlayerState = GetPlayerState<APotProbPlayerState>())
		{
			PotProbPlayerState->ServerReceivePotion(PotionType);
		}
	}
}

void APotProbPlayerController::DebugStartVotingPhase()
{
	if (APotProbPlayerState* PS = GetPlayerState<APotProbPlayerState>())
	{
		if (PS->bCanInitVote && !PS->bHasInitVote)
		{
			PS->bHasInitVote = true;
			if (APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
			{
				for (APlayerState* CurrPS : GameState->PlayerArray)
				{
					if (APotProbPlayerController* CurrPC = Cast<APotProbPlayerController>(CurrPS->GetOwner()))
					{
						CurrPC->Server_StartVoteCountdown();
					}
				}
			}
		}
	}
}

