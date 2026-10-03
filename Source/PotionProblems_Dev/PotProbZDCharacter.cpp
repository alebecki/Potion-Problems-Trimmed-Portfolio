// Fill out your copyright notice in the Description page of Project Settings.


#include "PotProbZDCharacter.h"

#include "ProxyChatWidget.h"
#include "PaperFlipbookComponent.h"
#include "PaperZDAnimationComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "PotProbGameState.h"
#include "Camera/CameraComponent.h"
#include "PlayerNameTagWidget.h"
#include "PotProbPlayerController.h"
#include "PotProbPlayerState.h"
#include "FOVComponent.h"
#include "HUDWidget.h"
#include "PotionStatusWidget.h"
#include "ROVComponent.h"
#include "ROVRoomTrigger.h"
#include "VectorTypes.h"
#include "Components/CapsuleComponent.h"
#include "Components/Image.h"
#include "Components/PostProcessComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/VerticalBox.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

APotProbZDCharacter::APotProbZDCharacter()
{
	bReplicates = true;
	SetReplicateMovement(true);
	IngredientSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("IngredientSpriteComponent"));
	IngredientSprite->SetupAttachment(GetComponentByClass<UPaperFlipbookComponent>());
	IngredientSprite->SetIsReplicated(true);

	PlayerText = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerNameTagWidget"));
	PlayerText->SetupAttachment(RootComponent);
	PlayerText->SetIsReplicated(false);
	PlayerText->SetCastShadow(false);

	EgoPlayerNameTag = CreateDefaultSubobject<UWidgetComponent>(TEXT("EgoPlayerNameTagWidget"));
	EgoPlayerNameTag->SetupAttachment(RootComponent);
	EgoPlayerNameTag->SetIsReplicated(false);
	EgoPlayerNameTag->SetCastShadow(false);
	
	ProxyChatComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ProxyChatWidget"));
	ProxyChatComponent->SetupAttachment(RootComponent);
	ProxyChatComponent->SetIsReplicated(false);
	ProxyChatComponent->SetCastShadow(false);
}

void APotProbZDCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		// Assign model owner ID
		static int32 OwnerId = 0;
		ModelOwnerId = OwnerId;
		OwnerId++;
		
		if (APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
		{
			//if the game has already started, return the player to lobby
			if (GameState->CurrentPhase != EPotProbPhases::PHASE_LOBBY)
			{
				GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
				{
					if (APlayerController* PC = Cast<APlayerController>(GetController()))
					{
						if (APotProbPlayerController* PotPC = Cast<APotProbPlayerController>(PC))
						{
							// Use the player controller to return to main menu
							PotPC->Client_ReturnToMainMenu();
						}
						else
						{
							// Generic fallback if custom controller isn't available
							PC->ConsoleCommand("Disconnect");
						}
					}
				});
				return;
			}
			GameState->Players.Add(this);
			GameState->AssignRandomModel(this);
			GameState->OnRep_NewPlayer();
		}
		//MovementSpeed = DefaultMovementSpeed;
		//OnRep_MovementSpeed();
	}

	if (UCameraComponent* Camera = FindComponentByClass<UCameraComponent>())
	{
		DefaultCameraFov = Camera->OrthoWidth;
		Camera->FieldOfView = DefaultCameraFov;
	}
	
	// Proxy Chat
	UProxyChatWidget* ProxyChat = Cast<UProxyChatWidget>(ProxyChatComponent->GetWidget());
	if (!ProxyChat)
	{
		ProxyChatComponent->SetWidgetClass(ProxyChatWidgetClass);
		ProxyChatComponent->InitWidget();
		ProxyChat = Cast<UProxyChatWidget>(PlayerText->GetWidget());
	}

	// Disable Post Process Component While In the Lobby
	if (UPostProcessComponent* PostProcessComponent = GetComponentByClass<UPostProcessComponent>())
	{
		PostProcessComponent->bEnabled = false;
	}

	// Initial position tracking
	LastPosition = GetActorLocation();
}

void APotProbZDCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetLocalRole() == ROLE_Authority)
	{
		if (APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
		{
			GameState->ReleaseModel(this);
			GameState->Players.Remove(this);
			GameState->OnRep_NewPlayer();
		}
	}
}


void APotProbZDCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Fix for listen server jittery animations
	if (GetMesh())
	{
		GetMesh()->bOnlyAllowAutonomousTickPose = false;
	}
}

void APotProbZDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
	// Only update render priority if position changed significantly
	FVector CurrentLocation = GetActorLocation();
	if (!LastPosition.Equals(CurrentLocation, 1.0f))
	{
		LastPosition = CurrentLocation;
		UpdateRenderPriorityBasedOnPosition();
	}

	int32 CurrentModel = static_cast<int32>(GetPlayerDisplayModel());
	if (CurrentModel != PreviousModel)
	{
		// Animation model updated since last frame. Update the anim component.
		if(UPaperZDAnimationComponent* AnimComponent = GetAnimationComponent())
		{
			AnimComponent->SetAnimInstanceClass(Anims[CurrentModel]);
		}

		// Notify player controller and associated UI of model change too.
		if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetController()))
		{
			PC->UpdateLocalCharCustomUI();
		}
	}
	PreviousModel = CurrentModel;
}
void APotProbZDCharacter::UpdateRenderPriorityBasedOnPosition()
{
    // Get this character's Y position (depth in the scene)
    float ThisY = GetActorLocation().Y;
    
    // Check if this is the local player
    bool bIsLocalPlayer = false;
    
    // Check if this is a locally controlled character on a client
    if (IsLocallyControlled() && !HasAuthority())
    {
        bIsLocalPlayer = true;
    }
    
    // Check if this is the server's character on a listen server
    if (HasAuthority() && IsPlayerControlled())
    {
        APlayerController* PC = Cast<APlayerController>(GetController());
        if (PC && PC->IsLocalController())
        {
            bIsLocalPlayer = true;
        }
    }
    
    // Find if there are any characters at almost the same Y position
    const float OverlapThreshold = 1.0f;
    bool bHasOverlappingCharacters = false;
    
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APotProbZDCharacter::StaticClass(), FoundActors);
    
    for (AActor* Actor : FoundActors)
    {
        if (Actor != this)
        {
            float OtherY = Actor->GetActorLocation().Y;
            if (FMath::Abs(ThisY - OtherY) <= OverlapThreshold)
            {
                bHasOverlappingCharacters = true;
                break;
            }
        }
    }
    
    // Get the sprite component
    UPaperFlipbookComponent* FlipbookComponent = GetSprite();
    if (!FlipbookComponent)
        return;
    
    // Get the current relative location of the sprite
    FVector SpriteLocation = FlipbookComponent->GetRelativeLocation();
    
    // Calculate Z offset based on Y position
    // Characters with higher Y (further back) should have lower Z values
    // This will make characters with lower Y (closer to camera) render on top
    float ZOffset = abs(ThisY) * 0.01f; // Scale factor to keep Z changes small but effective
    
    // If this is the local player and it's overlapping with another character,
    // give it a small positive Z boost to ensure it renders on top
    if (bIsLocalPlayer && bHasOverlappingCharacters)
    {
        ZOffset += 10.0f; // Boost local player above others at same Y
        
    }
    
    // Apply the Z offset to all components
    // Main sprite
    SpriteLocation.Z = ZOffset;
    FlipbookComponent->SetRelativeLocation(SpriteLocation);
    
    // Ingredient sprite
    if (IngredientSprite)
    {
        FVector IngredientLoc = IngredientSprite->GetRelativeLocation();
        IngredientLoc.Z = 0.1f; // Slightly in front of main sprite
        IngredientSprite->SetRelativeLocation(IngredientLoc);
    }
    
    // UI elements should always be in front
    if (PlayerText)
    {
        FVector TextLoc = PlayerText->GetRelativeLocation();
        TextLoc.Z = ZOffset + 0.2f;
        PlayerText->SetRelativeLocation(TextLoc);
    }
    
    if (EgoPlayerNameTag)
    {
        FVector NameLoc = EgoPlayerNameTag->GetRelativeLocation();
        NameLoc.Z = ZOffset + 0.2f;
        EgoPlayerNameTag->SetRelativeLocation(NameLoc);
    }
    
    if (ProxyChatComponent)
    {
        FVector ChatLoc = ProxyChatComponent->GetRelativeLocation();
        ChatLoc.Z = ZOffset + 0.3f;
        ProxyChatComponent->SetRelativeLocation(ChatLoc);
    }
    
}


void APotProbZDCharacter::SetPlayerText_Implementation(const FString& NewText)
{
	PlayerNameTag = NewText;
	if (OriginalPlayerName == "")
	{
		OriginalPlayerName = NewText;
	}
	OnRep_PlayerNameTag();
}

void APotProbZDCharacter::EgoOverridePlayerText(const FString& PlayerNameText, bool bIsUsingEgo)
{
	bHidePlayerText = !bIsUsingEgo;
	PlayerText->SetVisibility(!bIsUsingEgo);
	EgoPlayerNameTag->SetVisibility(bIsUsingEgo);
	UPlayerNameTagWidget* EgoPlayerTag = Cast<UPlayerNameTagWidget>(EgoPlayerNameTag->GetWidget());
	if(!EgoPlayerTag)
	{
		EgoPlayerNameTag->SetWidgetClass(PlayerNametagClass);
		EgoPlayerNameTag->InitWidget();
		EgoPlayerTag = Cast<UPlayerNameTagWidget>(EgoPlayerNameTag->GetWidget());
	}
	
	EgoPlayerTag->SetNameText(PlayerNameText);
}

void APotProbZDCharacter::ToggleFrogProperties()
{
	// TODO: rename function for frog tunnels
	TArray<AActor*> FrogDoors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("FrogTunnel"), FrogDoors);

	for (AActor* FrogDoor : FrogDoors)
	{
		FrogDoor->SetActorEnableCollision(false);
	}
}

void APotProbZDCharacter::MulticastEnablePostProcessComponent_Implementation()
{
	if (IsLocallyControlled())
	{
		EnablePostProcessComponent();
	}
}

void APotProbZDCharacter::EnablePostProcessComponent()
{
	if (UPostProcessComponent* PostProcessComponent = GetComponentByClass<UPostProcessComponent>())
	{
		PostProcessComponent->bEnabled = true;
	}
}

void APotProbZDCharacter::HidePlayerComponents()
{
	PlayerText->SetVisibility(false);
	EgoPlayerNameTag->SetVisibility(false);
	ProxyChatComponent->SetVisibility(false);
}

void APotProbZDCharacter::RevealPlayerComponents()
{
	if (bHidePlayerText)
	{
		EgoPlayerNameTag->SetVisibility(true);
	}
	else
	{
		PlayerText->SetVisibility(true);
	}
	ProxyChatComponent->SetVisibility(true);
}

void APotProbZDCharacter::ClientToggleCootiesEffects_Implementation(bool HasCooties)
{
	if (HasCooties)
	{
		ServerChangeMovementSpeed(LovePotionPlayerSpeed);
		SetCameraFov(LovePotionCameraFov);
	}
	else
	{
		ResetCameraFov();
		ServerChangeMovementSpeed(1.0f);
	}
}

void APotProbZDCharacter::ClientUpdateDrinkText_Implementation(bool bEffectActive)
{
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetController()))
	{
		if (PC->GetHUDWidgetInstance())
		{
			PC->GetHUDWidgetInstance()->ToggleDrinkText(bEffectActive);
		}
	}
}


void APotProbZDCharacter::ToggleChillyBoost(bool bNewStatus)
{
	if (!bIsChillyBoosted && bNewStatus)
	{
		GetWorld()->GetTimerManager().ClearTimer(ChillyBoostTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(ChillyBoostTimerHandle, this, &APotProbZDCharacter::HandleSpawnChillyBoost, ChillyBoostSpawnTime, true);
		HandleSpawnChillyBoost();

		ServerChangeMovementSpeed(ChillyBoostMultiplier);
	}
	else if (bIsChillyBoosted && !bNewStatus)
	{
		// Stop looping boost trigger spawner
		GetWorld()->GetTimerManager().ClearTimer(ChillyBoostTimerHandle);
		ServerChangeMovementSpeed(1.0f / ChillyBoostMultiplier);
	}
	bIsChillyBoosted = bNewStatus;
	OnRep_MovementSpeed();
}

void APotProbZDCharacter::ActivateChillyMiniBoost()
{
	if (!bIsChillyBoosted)
	{
		bIsChillyBoosted = true;
		ServerChangeMovementSpeed(ChillyBoostMultiplier);
		OnRep_MovementSpeed();
		GetWorld()->GetTimerManager().SetTimer(ChillyBoostTimerHandle, this, &APotProbZDCharacter::HandleMiniChillyBoost, ChillyBoostDecayTime, false);
	}
}

void APotProbZDCharacter::HandleSpawnChillyBoost()
{
	if (ChillyBoostTriggerClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (FVector::Dist(LastPosition, GetActorLocation()) >= MinimumBoostSpawnDist)
		{
			GetWorld()->SpawnActor<AActor>(ChillyBoostTriggerClass, GetActorLocation(), GetActorRotation(), SpawnParams);
			LastPosition = GetActorLocation();
		}
	}
}

void APotProbZDCharacter::HandleMiniChillyBoost()
{
	bIsChillyBoosted = false;
	
	ServerChangeMovementSpeed(1.0f / ChillyBoostMultiplier);
	OnRep_MovementSpeed();
}

void APotProbZDCharacter::ServerProxyMessage_Implementation(const FString& Message)
{
	ClientProxyMessage(Message);
}

void APotProbZDCharacter::ClientProxyMessage_Implementation(const FString& Message)
{
	if (UProxyChatWidget* ChatWidget = Cast<UProxyChatWidget>(ProxyChatComponent->GetWidget()))
	{
		ChatWidget->AddChatMessage(Message);
	}
}

void APotProbZDCharacter::ServerChangeMovementSpeed_Implementation(float Val)
{
	MovementSpeed = MovementSpeed * Val;

	if (Val == 1.0f)
	{
		MovementSpeed = DefaultMovementSpeed;
	}
	
	OnRep_MovementSpeed();
}

void APotProbZDCharacter::OnRep_MovementSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void APotProbZDCharacter::ServerTransformIntoWerefrog_Implementation()
{
	if (bIsWerefrog)
	{
		GetWorldTimerManager().ClearTimer(WerefrogTimer);
		GetWorldTimerManager().SetTimer(WerefrogTimer, this, &APotProbZDCharacter::ServerUntransformIntoWerefrog, FrogPotionDuration);
		ClientUpdateWerefrogStatusOnHUD(false);
		return;
	}

	SetPlayerText(TEXT("???"));
	ServerChangeMovementSpeed(FrogPotionSpeedMult);
	bIsWerefrog = true;
	ClientUpdateWerefrogStatusOnHUD(true);
	ClientUpdateDrinkText(true);
	GetWorldTimerManager().SetTimer(WerefrogTimer, this, &APotProbZDCharacter::ServerUntransformIntoWerefrog, FrogPotionDuration);

	AlertDrinkFrogPotion();
}

void APotProbZDCharacter::ClientUpdateWerefrogStatusOnHUD_Implementation(bool bCreateNew)
{
	APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetController());
	if (!PC || !PC->IsLocalController())
		return;

	if (!bCreateNew && WerefrogStatusWidgetInstance)
	{
		if (UPotionStatusWidget* StatusWidget = Cast<UPotionStatusWidget>(WerefrogStatusWidgetInstance))
		{
			StatusWidget->ResetTimer();
			return;
		}
	}
	else
	{
		if (PotionStatusWidgetClass)
		{
			if (WerefrogStatusWidgetInstance)
			{
				WerefrogStatusWidgetInstance->RemoveFromParent();
				WerefrogStatusWidgetInstance = nullptr;
			}

			// Create and initialize the new widget
			WerefrogStatusWidgetInstance = CreateWidget<UUserWidget>(PC, PotionStatusWidgetClass);
			if (WerefrogStatusWidgetInstance)
			{
				// Add to the status effect box
				PC->HUDWidgetInstance->StatusEffectVerticalBox->AddChild(WerefrogStatusWidgetInstance);

				//Set Icon
				if (UPotionStatusWidget* StatusWidget = Cast<UPotionStatusWidget>(WerefrogStatusWidgetInstance))
				{
					if (WerefrogStatusIconTexture)
					{
						FSlateBrush Brush;
						Brush.SetResourceObject(WerefrogStatusIconTexture);
						StatusWidget->CurrentPotionEffectImage->SetBrush(Brush);
					}
					StatusWidget->SetInitialStatusTime(68.0f);
				}
			}
		}
	}
}


void APotProbZDCharacter::AlertDrinkFrogPotion()
{
	APotProbPlayerState* PS = Cast<APotProbPlayerState>(GetPlayerState());
	if (PS)
	{
		PS->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "Catch an Apprentice and press [E] to frog them!");
		PS->ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Catch an Apprentice and press [E] to frog them!");
		PS->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "Catch an Apprentice!\nPress [E] to frog them!");
		PS->ServerReceiveAlertMessage(EPotProbAlertTypes::HIGH_PRIORITY, "A Troublemaker has turned into a Werefrog!\nDon't let them catch you!", false);
	}
}

void APotProbZDCharacter::ServerUntransformIntoWerefrog_Implementation()
{
	GetWorldTimerManager().ClearTimer(WerefrogTimer);
	// no longer delay canceling effects
	//GetWorldTimerManager().SetTimer(WerefrogTimer, this, &APotProbZDCharacter::EndWerefrogTime, 8.0f);
	EndWerefrogTime();
	bIsWerefrog = false;
	ClientUpdateDrinkText(false);

	if (APotProbPlayerState* PS = Cast<APotProbPlayerState>(GetPlayerState()))
	{
		PS->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "Great job! Now hide somewhere to avoid getting caught!");
		PS->ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Great job! Now hide somewhere to avoid getting caught!");
		PS->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "Great job! Now hide somewhere \nto avoid getting caught!");
	}
}

void APotProbZDCharacter::SetIsHoldingMove(bool Status)
{
	bIsHoldingMove = Status;
}


void APotProbZDCharacter::SetCharacterDirectionality(FVector2D Direction)
{
	Directionality.X = Direction.X;
	Directionality.Y = Direction.Y;
}

void APotProbZDCharacter::ServerClarivoyanceStart_Implementation(bool bVal)
{
	if (!HasAuthority())
	{
		return;
	}
	bIsClarivoyant = bVal;

	// must call since on server
	OnRep_IsClarivoyant();

	ClientClairvoyanceStart(bVal);
	ServerChangeMovementSpeed(ClairvoyancePotionPlayerSpeed);
	
	GetWorldTimerManager().SetTimer(ClarivoyanceTimer, this, &APotProbZDCharacter::ServerEndClarivoyance, ClairvoyancePotionDuration);
}

void APotProbZDCharacter::ServerEndClarivoyance_Implementation()
{
	if (HasAuthority())
	{
		GetWorldTimerManager().ClearTimer(ClarivoyanceTimer);
		bIsClarivoyant = false;
		
		// must call since on server
		OnRep_IsClarivoyant();

		// Reset movement speed change
		ServerChangeMovementSpeed(1.0f);
		ClientEndClairvoyance();

		if (APotProbPlayerState* PS = Cast<APotProbPlayerState>(GetPlayerState()))
		{
			PS->ServerCleanupPotionActivation();
		}
	}
}

void APotProbZDCharacter::ClientClairvoyanceStart_Implementation(bool bVal)
{
	APotProbPlayerState* PS = Cast<APotProbPlayerState>(GetPlayerState());
	if (PS)
	{
		ClairvoyanceStatusWidgetInstance = PS->AddPotionEffectIconToHUD(ClairvoyanceStatusIconTexture, ClairvoyancePotionDuration);

		if (APotProbPlayerController* PotProbPlayerController = Cast<APotProbPlayerController>(PS->GetOwningController()))
		{
			PotProbPlayerController->HiddenActors.Empty();
			
			ClientUpdateDrinkText(true);
		}
	}
}

void APotProbZDCharacter::ClientEndClairvoyance_Implementation()
{
	APotProbPlayerState* PS = Cast<APotProbPlayerState>(GetPlayerState());
	if (PS)
	{
		PS->RemovePotionEffectIconFromHUD(ClairvoyanceStatusWidgetInstance);

		// add all characters to be hidden
		if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(PS->GetOwningController()))
		{
			// add all characters to be hidden
			if (APotProbGameState* GS = Cast<APotProbGameState>(GetWorld()->GetGameState()))
			{
				for (AActor* OtherActor : GS->Players)
				{
					if (OtherActor != this)
					{
						PC->HiddenActors.Add(OtherActor);
					}
				}
			}

			ClientUpdateDrinkText(false);
		}

		// Find the room the player is currently in
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("ROV"), FoundActors);
		
		// remove players currentlty in the same room
		for (AActor* ActorRoom : FoundActors)
		{
			if (UROVComponent* RC = ActorRoom->GetComponentByClass<UROVComponent>())
			{
				if (RC->GetContainsLocalPlayer())
				{
					for (APotProbZDCharacter* OtherCharacter : RC->PlayersWithin)
					{
						if (APotProbPlayerController* PotProbPlayerController = Cast<APotProbPlayerController>(PS->GetOwningController()))
						{
							PotProbPlayerController->HiddenActors.Remove(OtherCharacter);
						}
					}
				}
			}
		}
	}
}



void APotProbZDCharacter::ServerUpdateCapsuleRadius_Implementation(float Radius)
{
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	if(!CapsuleComp)
	{
		return;
	}

	CapsuleComp->SetCapsuleRadius(Radius);
}

void APotProbZDCharacter::EndCamouflaged_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	GetWorldTimerManager().ClearTimer(CamouflageTimer);
	bIsCamouflaged = false;
	SetPlayerText(OriginalPlayerName);
	ClientUpdateDrinkText(false);
	ClientEndCamouflaged();
	Server_ClearModelOverride();
	if (IsLocallyControlled())
	{
		OnRep_IsCamouflaged();
	}
}

void APotProbZDCharacter::ClientEndCamouflaged_Implementation()
{
	APotProbPlayerState* PS = Cast<APotProbPlayerState>(GetPlayerState());
	if (PS)
	{
		PS->RemovePotionEffectIconFromHUD(CamoStatusWidgetInstance);
	}
}


void APotProbZDCharacter::SetIsCamouflaged_Implementation(bool bVal)
{
	if(!HasAuthority())
	{
		return;
	}
	bIsCamouflaged = bVal;
	if (IsLocallyControlled())
	{
		OnRep_IsCamouflaged();
	}
	
	ClientUpdateDrinkText(bVal);
	
	if(bIsCamouflaged)
	{
		SetPlayerText("  ");
		UPaperZDAnimationComponent* AnimComponent = GetAnimationComponent();
		if(!AnimComponent)
		{
			return;
		}
		int32 Start = (int32)EAnimModels::CAMO_MODEL_0;
		int32 End = (int32)EAnimModels::CAMO_MODEL_2;
		int32 RandCamo = FMath::RandRange(Start, End);
		Server_SetModelOverride(static_cast<EAnimModels>(RandCamo));

		// alert player
		APotProbPlayerState* PS = Cast<APotProbPlayerState>(GetPlayerState());
		if (PS)
		{
			PS->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "You've transformed into \nan inconspicuous object!");
		}
		ClientSetIsCamouflaged(bVal);
	}
	GetWorldTimerManager().SetTimer(CamouflageTimer, this, &APotProbZDCharacter::EndCamouflaged,
	                                CamouflagePotionDuration);
}

void APotProbZDCharacter::ClientSetIsCamouflaged_Implementation(bool bVal)
{
	APotProbPlayerState* PS = Cast<APotProbPlayerState>(GetPlayerState());
	if (PS)
	{
		CamoStatusWidgetInstance = PS->AddPotionEffectIconToHUD(CamoStatusIconTexture, CamouflagePotionDuration);
	}
}


void APotProbZDCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APotProbZDCharacter, Directionality);
	DOREPLIFETIME(APotProbZDCharacter, bIsHoldingMove);
	DOREPLIFETIME(APotProbZDCharacter, bIsCamouflaged);
	DOREPLIFETIME(APotProbZDCharacter, IngredientSprite);
	DOREPLIFETIME(APotProbZDCharacter, CurrentIngredientSprite);
	DOREPLIFETIME(APotProbZDCharacter, PlayerNameTag);
	DOREPLIFETIME(APotProbZDCharacter, MovementSpeed);
	DOREPLIFETIME(APotProbZDCharacter, bIsWerefrog);
	DOREPLIFETIME(APotProbZDCharacter, bIsClarivoyant);
	DOREPLIFETIME(APotProbZDCharacter, bIsShrinking);
	DOREPLIFETIME(APotProbZDCharacter, ModelOverride);
	DOREPLIFETIME(APotProbZDCharacter, ModelOwnerId);
}

void APotProbZDCharacter::UpdateSpriteColor_Implementation(FLinearColor Color) const
{
	if (UPaperFlipbookComponent* SpriteComp = GetSprite())
	{
		SpriteComp->SetSpriteColor(Color);
	}
}

void APotProbZDCharacter::UpdateSpritePosition_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Update Sprite Position"));

	// get random other player
	TArray<TObjectPtr<APlayerState>> PlayerArray = GetWorld()->GetGameState<APotProbGameState>()->PlayerArray;
	PlayerArray.Remove(GetPlayerState());

	int32 RandomIndex = FMath::RandRange(0, PlayerArray.Num() - 1);
	TObjectPtr<APlayerState> RandomPlayer = PlayerArray[RandomIndex];

	// swap locations with random player
	FVector SourceActorLocation = GetActorLocation();
	FVector TargetActorLocation = RandomPlayer->GetPawn()->GetActorLocation();

	SetActorLocation(TargetActorLocation);
	RandomPlayer->GetPawn()->SetActorLocation(SourceActorLocation);
	APlayerController* PlayerController = GetController<APlayerController>();
}

void APotProbZDCharacter::OnRep_IsCamouflaged()
{
	APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetController());
	if (!PC)
	{
		return;
	}
	ServerChangeMovementSpeed(bIsCamouflaged ? CamouflagePotionPlayerSpeed : 1.0f);

	if (bIsCamouflaged)
	{
		if (APotProbPlayerState* PS = PC->GetPlayerState<APotProbPlayerState>())
		{
			PC->Server_WipeIngredientName(PS);
		}
		SetCameraFov(CamouflagePotionCameraFov);
	}
	else
	{
		ResetCameraFov();
	}
}

void APotProbZDCharacter::OnRep_IsClarivoyant()
{
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetController()))
	{
		if (IsLocallyControlled())
		{
			if (UPostProcessComponent* PostProcessComp = GetComponentByClass<UPostProcessComponent>())
			{
				PostProcessComp->bHiddenInGame = bIsClarivoyant;
			}
		}

		if (bIsClarivoyant)
		{
			SetCameraFov(ClairvoyancePotionCameraFov);
		}
		else
		{
			ResetCameraFov();
		}
	}
}

void APotProbZDCharacter::OnRep_IsShrinking()
{
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetController()))
	{
		ToggleShrinkCamera(bIsShrinking);
	}
	
	if (UPaperFlipbookComponent* PlayerSprite = GetComponentByClass<UPaperFlipbookComponent>())
	{
		if (bIsShrinking)
		{
			PlayerSprite->SetRelativeScale3D(FVector(ShrinkPotionPlayerScale, ShrinkPotionPlayerScale, ShrinkPotionPlayerScale));
		}
		else
		{
			PlayerSprite->SetRelativeScale3D(FVector3d::OneVector);
		}
	}
}

void APotProbZDCharacter::SetIsShrinking(bool isShrinking)
{
	bIsShrinking = isShrinking;

	if (isShrinking)
	{
		ClientSetIsShrinking();
		GetWorld()->GetTimerManager().SetTimer(ShrinkingPotionTimer, this, &APotProbZDCharacter::ClearShrinkingPotionEffects, 180.0f, false);
	}
	if(HasAuthority())
	{
		OnRep_IsShrinking();
	}
}

void APotProbZDCharacter::ClearShrinkingPotionEffects_Implementation()
{
	SetIsShrinking(false);
	ServerUpdateCapsuleRadius(UnfroggedRadius);
	ServerChangeMovementSpeed(1.0f);
}

void APotProbZDCharacter::ClientSetIsShrinking_Implementation()
{
	APotProbPlayerState* PS = Cast<APotProbPlayerState>(GetPlayerState());
	if (PS)
	{
		PS->AddPotionEffectIconToHUD(ShrinkingStatusIconTexture, ShrinkPotionDuration);
	}
}


void APotProbZDCharacter::EndWerefrogTime()
{
	if(!HasAuthority())
	{
		return;
	}
	if (WerefrogStatusWidgetInstance)
	{
		WerefrogStatusWidgetInstance->RemoveFromParent();
		WerefrogStatusWidgetInstance = nullptr;
	}
	SetPlayerText(GetPlayerState()->GetPlayerName());
	ServerChangeMovementSpeed(1.0f);
	GetWorldTimerManager().ClearTimer(WerefrogTimer);

	APotProbPlayerState* PS = Cast<APotProbPlayerState>(GetPlayerState());
	if (PS)
	{
		PS->ServerReceiveAlertMessage(EPotProbAlertTypes::HIGH_PRIORITY, "The Troublemaker's Werefrog potion has worn off.\nDiscover who the mischief maker is!", false);
		if (PS->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
		{
			PS->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "Keep frogging apprentices to defeat them!");
			PS->ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Keep frogging apprentices to defeat them!");
			PS->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "Keep frogging apprentices \nto defeat them!");
		}
	}
}

void APotProbZDCharacter::OnRep_IngredientSprite()
{
	if (IngredientSprite)
	{
		IngredientSprite->SetSprite(CurrentIngredientSprite);
	}
}

void APotProbZDCharacter::OnRep_PlayerNameTag()
{
	UPlayerNameTagWidget* PlayerTag = Cast<UPlayerNameTagWidget>(PlayerText->GetWidget());
	if (!PlayerTag)
	{
		PlayerText->SetWidgetClass(PlayerNametagClass);
		PlayerText->InitWidget();
		PlayerTag = Cast<UPlayerNameTagWidget>(PlayerText->GetWidget());
	}
	if (PlayerTag)
	{
		PlayerTag->SetNameText(PlayerNameTag);
	}
}

void APotProbZDCharacter::UpdatePlayerNameTagColor_Multicast_Implementation()
{
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
	{
		if (APotProbPlayerState* PS = PC->GetPlayerState<APotProbPlayerState>())
		{
			if (PS->PotProbRole != EPotProbRoles::ROLE_TROUBLEMAKER)
			{
				return;
			}
		} else
		{
			return;
		}
	} else {
		return;
	}

	UPlayerNameTagWidget* PlayerTag = Cast<UPlayerNameTagWidget>(PlayerText->GetWidget());
	if (PlayerTag)
	{
		if (APotProbPlayerState* PS = GetPlayerState<APotProbPlayerState>())
		{
			if (PS->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
			{
				PlayerTag->SetColor(TroublemakerNametagColor);
			}
		}
	}
}

void APotProbZDCharacter::SetCameraFov(float FOV)
{
	//Function to update camera zoom in Client
	UCameraComponent* CameraComp = FindComponentByClass<UCameraComponent>();
	
	CameraComp->SetOrthoWidth(FOV);
	CameraComp->SetFieldOfView(FOV);
}

void APotProbZDCharacter::ResetCameraFov()
{
	SetCameraFov(DefaultCameraFov);
}


void APotProbZDCharacter::ToggleShrinkCamera(bool shrunk)
{
	//Function to update camera zoom in Client
	UCameraComponent* CameraComp = FindComponentByClass<UCameraComponent>();
	if (shrunk)
	{
		SetCameraFov(ShrinkPotionCameraFov);
	}
	else
	{
		ResetCameraFov();
	}
}

void APotProbZDCharacter::Server_SetIngredientSprite_Implementation(UPaperSprite* NewSprite)
{
	if (HasAuthority())
	{
		CurrentIngredientSprite = NewSprite;

		if (IngredientSprite)
		{
			IngredientSprite->SetSprite(CurrentIngredientSprite);
		}
	}
}

void APotProbZDCharacter::Server_SetModelOverride_Implementation(EAnimModels Model)
{
	ModelOverride = static_cast<int32>(Model);
}

void APotProbZDCharacter::Server_ClearModelOverride_Implementation()
{
	ModelOverride = -1;
}

EAnimModels APotProbZDCharacter::GetPlayerSelectedModel() const
{
	if (const APotProbGameState* GameState = GetWorld() != nullptr ? GetWorld()->GetGameState<APotProbGameState>() : nullptr)
	{
		int32 ModelIndex = GameState->GetCharacterModelIndex(this);
		if (ModelIndex >= 0)
		{
			return static_cast<EAnimModels>(ModelIndex);
		}
	}

	// Unable to find our character model. Return a default.
	return EAnimModels::CHAR_MODEL_0;
}

EAnimModels APotProbZDCharacter::GetPlayerDisplayModel() const
{
	if (APotProbPlayerState* PS = Cast<APotProbPlayerState>(GetPlayerState()))
	{
		if (bIsWerefrog)
		{
			return EAnimModels::WEREFROG_MODEL;
		}
		
		if (PS->bIsFrogged)
		{
			return EAnimModels::FROGGED_MODEL;
		}
	}
	
	if (ModelOverride >= 0)
	{
		// Model override (set by potions). This bypasses server authority over model ownership and allows
		// a character to appear as any model.
		return static_cast<EAnimModels>(ModelOverride);
	}

	return GetPlayerSelectedModel();
}

void APotProbZDCharacter::SetIngredientSprite(UPaperSprite* NewSprite)
{
	if (HasAuthority())
	{
		Server_SetIngredientSprite(NewSprite);
	}
	else
	{
		Server_SetIngredientSprite(NewSprite);
	}
}
