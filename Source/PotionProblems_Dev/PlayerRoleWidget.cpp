// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerRoleWidget.h"

#include "HUDWidget.h"
#include "PotProbPlayerState.h"
#include "FellowTBMKRWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "PotProbPlayerController.h"
#include "PotProbZDCharacter.h"
#include "Components/ScaleBox.h"
#include "Components/VerticalBox.h"

void UPlayerRoleWidget::SetPlayerInformation(const FString& PlayerRoleText, const FString& ActionOneText, const FString& ActionTwoText,
                                             const FString& ActionThreeText, const FString& TroublemakerCountText)
{
	PlayerRole->SetText(FText::FromString(PlayerRoleText));
	ActionOne->SetText(FText::FromString(ActionOneText));
	ActionTwo->SetText(FText::FromString(ActionTwoText));
	ActionThree->SetText(FText::FromString(ActionThreeText));
	TroublemakerCount->SetText(FText::FromString(TroublemakerCountText));
}

void UPlayerRoleWidget::NativeConstruct()
{
	CloseButton = CloseButtonAnim->ButtonAnim;
	Super::NativeConstruct();

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UPlayerRoleWidget::OnClose);
	}

	APotProbPlayerState* PlayerState = UGameplayStatics::GetPlayerController(this, 0)->GetPlayerState<APotProbPlayerState>();
	if (PlayerState->bIsFrogged)
	{
		Frog->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	if (PlayerState->PotProbRole == EPotProbRoles::ROLE_APPRENTICE)
	{
		Apprentice->SetVisibility(ESlateVisibility::Visible);
	}
	if (PlayerState->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
	{
		Troublemaker->SetVisibility(ESlateVisibility::Visible);
		SBFellowTBMKRList->SetVisibility(ESlateVisibility::Visible);
		SBFellowTBMKRImage->SetVisibility(ESlateVisibility::Visible);
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UPlayerRoleWidget::BuildFellowTBMKRList, 0.25f, false);
	}

	
}

void UPlayerRoleWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UPlayerRoleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UPlayerRoleWidget::OnClose()
{
	SetVisibility(ESlateVisibility::Collapsed);
	
	APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if(!PlayerController)
	{
		return;
	}
	APotProbPlayerState* PlayerState = PlayerController->GetPlayerState<APotProbPlayerState>();
	if(!PlayerState)
	{
		return;
	}
	
	PlayerState->ServerRerollPotions();

	// tutorial alerts based on role
    if (bAlertOnClose)
    {
       PlayerState->ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Open your map to\nfind ingredients!");
       if (PlayerState->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
       {
       	    PlayerState->ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Find the three ingredients indicated on your\nrecipe card to craft The Froggart Curse!");
       	    PlayerState->ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Be sneaky, as apprentices might put ingredients\nin your cauldron to sabotage you!");
       }
    }

	// clean up widget
	PlayerController->PlayerRoleWidgetInstance = nullptr;
	RemoveFromParent();
}

void UPlayerRoleWidget::BuildFellowTBMKRList()
{
	APotProbGameState* GameState = Cast<APotProbGameState>(UGameplayStatics::GetGameState(this));
	for (auto PS : GameState->PlayerArray)
	{
		if (APotProbPlayerState* PotProbPlayerState = Cast<APotProbPlayerState>(PS))
		{
			if (PotProbPlayerState->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER && PotProbPlayerState != GetOwningPlayer()->GetPlayerState<APotProbPlayerState>())
			{
				UFellowTBMKRWidget* NewWidget = CreateWidget<UFellowTBMKRWidget>(this, FellowTBMKRWidgetClass);
				FellowTBMKRList->AddChild(NewWidget);
				
				auto& CharacterTextures = static_cast<APotProbPlayerController*>(GetOwningPlayer())->GetHUDWidgetInstance()->GetCharacterTextures();
				NewWidget->PlayerName->SetText(FText::FromString(PotProbPlayerState->GetPlayerName()));
				
				APotProbZDCharacter* PlayerChar = Cast<APotProbZDCharacter>(PotProbPlayerState->GetPawn());
				EAnimModels ModelSelected = PlayerChar->GetPlayerSelectedModel();
				NewWidget->PlayerImage->SetBrushFromTexture(Cast<UTexture2D>(CharacterTextures[ModelSelected]));
			}
		}
	}
}
