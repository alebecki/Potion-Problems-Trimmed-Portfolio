// Fill out your copyright notice in the Description page of Project Settings.


#include "SpyglassMinigameWidget.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "Components/CanvasPanelSlot.h"
#include "Logging/LogMacros.h"
#include "PotProbPlayerController.h"
#include "ObservatoryWorldActor.h"
#include "PotProbPlayerState.h"
#include "MinigameInitActor.h"


void USpyglassMinigameWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    SetupInputBindings();

    SpyGlassSlot = Cast<UCanvasPanelSlot>(SpyGlassWidget->Slot);
    CenterLocation = SpyGlassSlot->GetPosition();

    if (IsValid(ObservatoryWorldActor))
    {
        ObservatoryWidth = ObservatoryWorldActor->SkyWidth;
        ObservatoryHeight = ObservatoryWorldActor->SkyHeight;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SpyglassMinigameWidget: ObservatoryWorldActor is not set."));
    }
}

void USpyglassMinigameWidget::NativeDestruct()
{
    Super::NativeDestruct();
}

void USpyglassMinigameWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    CheckStarFound();
}

void USpyglassMinigameWidget::CheckStarFound()
{
    if (!IsValid(SpyGlassSlot))
    {
        UE_LOG(LogTemp, Warning, TEXT("Spyglass Slot isn't set"));
        return;
    }

    if (!IsValid(ObservatoryWorldActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("ObservatoryWorldActor isn't set"));
        return;
    }

    // when spyglass catches star
    if ((SpyGlassSlot->GetPosition() - ObservatoryWorldActor->GetStarLocation()).Size() < StarFindDistance)
    {
        if (APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(GetOwningPlayer()))
        {
            APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(PlayerController->PlayerState);
            
            // reset star - must call server rpc from a replicated player state
            PlayerState->ResetStarMinigame(ObservatoryWorldActor);
            
            // deactivate telescope
            if (IsValid(ParentInitActor))
            {
                PlayerController->DisableTelescopeInitActor(ParentInitActor);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("SpyglassMinigameWidget: ParentInitActor is invalid"));
            }

            // increment telescope successes for troublemaker win con
            PlayerState->Server_IncrementTelescopeSuccesses_GameState();
        }
        
        RemoveInputBindings();
    }
}

void USpyglassMinigameWidget::OnMove (const FInputActionInstance& ActionInstance)
{
    if (!IsValid(SpyGlassSlot))
    {
        return;
    }
    
    FVector2D MovementVector = ActionInstance.GetValue().Get<FVector2D>();
    MovementVector.X *= -1.0f;

    // clamp the position so the spyglass doesn't go off of sky
    FVector2D NewLocation = SpyGlassSlot->GetPosition() + MovementVector * InputMoveSpeed;
    NewLocation.X = FMath::Clamp(NewLocation.X, CenterLocation.X - ObservatoryWidth / 2.0f, CenterLocation.X + ObservatoryWidth / 2.0f);
    NewLocation.Y = FMath::Clamp(NewLocation.Y, CenterLocation.Y - ObservatoryHeight / 2.0f, CenterLocation.Y + ObservatoryHeight / 2.0f);
    SpyGlassSlot->SetPosition(NewLocation);
}

void USpyglassMinigameWidget::RemoveInputBindings()
{
    APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(GetOwningPlayerState());
    APotProbPlayerController* PC = Cast<APotProbPlayerController>(PlayerState->GetPlayerController());
    if (IsValid(PC))
    {
        UEnhancedInputLocalPlayerSubsystem* Subsystem = 
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
        
        if (Subsystem)
        {
            Subsystem->ClearAllMappings();
            Subsystem->AddMappingContext(PlayerGameMappingContext, 0); // Priority 0
            PC->AddBackInputComponent();
        }
    }
    
    PC->SetInputMode(FInputModeGameAndUI());
    PlayerState->SpyglassWidget = nullptr;
    
    OwningWorldActor->Destroy();
}

void USpyglassMinigameWidget::SetupInputBindings()
{
	if (!MoveAction || !MinigameMappingContext) return;

    APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(GetOwningPlayerState());
    APlayerController* PC = PlayerState->GetPlayerController();
    if (IsValid(PC))
    {
        UEnhancedInputLocalPlayerSubsystem* Subsystem = 
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
        
        if (Subsystem)
        {
            Subsystem->ClearAllMappings();
            Subsystem->AddMappingContext(MinigameMappingContext, 0); // Priority 0
        }
        
        // Set up the Enhanced Input Component
        if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
        {
            EnhancedInputComponent->ClearActionBindings();
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &USpyglassMinigameWidget::OnMove);
        }
    }
}