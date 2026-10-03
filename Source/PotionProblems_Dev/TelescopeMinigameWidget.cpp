// Fill out your copyright notice in the Description page of Project Settings.

#include "TelescopeMinigameWidget.h"

#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "Components/CanvasPanelSlot.h"
#include "Logging/LogMacros.h"
#include "Components/CanvasPanel.h"
#include "PotProbPlayerController.h"


void UTelescopeMinigameWidget::NativeConstruct()
{
    Super::NativeConstruct();
    SetupInputBindings();

    SpyGlassSlot = Cast<UCanvasPanelSlot>(SpyGlassWidget->Slot);
    StarSlot = Cast<UCanvasPanelSlot>(StarWidget->Slot);

    InputMoveSpeed = 10.0f;
    StarFindDistance = 50.0f;
}


void UTelescopeMinigameWidget::NativeDestruct()
{
    Super::NativeDestruct();

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningPlayer()->GetLocalPlayer());

    if (Subsystem)
    {
        Subsystem->RemoveMappingContext(MinigameMappingContext);
    }
}

void UTelescopeMinigameWidget::CheckStarFound()
{
    if (!IsValid(SpyGlassSlot) || !IsValid(StarWidget))
    {
        return;
    }

    if ((SpyGlassSlot->GetPosition() - StarSlot->GetPosition()).Size() < StarFindDistance)
    {
        StarFound();
    }
}

void UTelescopeMinigameWidget::StarFound()
{
    APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetOwningPlayer());
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

    RemoveFromParent();
    
    PC->SetInputMode(FInputModeGameAndUI());
}

void UTelescopeMinigameWidget::OnMove(const FInputActionInstance& ActionInstance)
{
    if (!IsValid(SpyGlassSlot))
    {
        return;
    }
    
    FVector2D MovementVector = ActionInstance.GetValue().Get<FVector2D>();
    MovementVector.X *= -1.0f;
    FVector2D OffsetVector = SpyGlassSlot->GetPosition() + MovementVector * InputMoveSpeed;
    if(OffsetVector.X > 770.0f)
    {
        OffsetVector.X = 770.0f;
    }
    else if (OffsetVector.X < -770.0f)
    {
        OffsetVector.X = -770.0f;
    }

    if(OffsetVector.Y > 390.0f)
    {
        OffsetVector.Y = 390.0f;
    }
    else if (OffsetVector.Y < -410.0f)
    {
        OffsetVector.Y = -410.0f;
    }
    UE_LOG(LogTemp, Warning, TEXT("( %f , %f )"), OffsetVector.X, OffsetVector.Y);
    SpyGlassSlot->SetPosition(OffsetVector);
    CheckStarFound();
}

void UTelescopeMinigameWidget::SetupInputBindings()
{
	if (!MoveAction || !MinigameMappingContext) return;
    
    APlayerController* PlayerController = GetOwningPlayer();
    if (PlayerController)
    {
        UEnhancedInputLocalPlayerSubsystem* Subsystem = 
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
        
        if (Subsystem)
        {
            Subsystem->ClearAllMappings();
            Subsystem->AddMappingContext(MinigameMappingContext, 0); // Priority 0
        }

        // Set up the Enhanced Input Component
        if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
        {
            EnhancedInputComponent->ClearActionBindings();
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UTelescopeMinigameWidget::OnMove);
        }
    }
}
