// Fill out your copyright notice in the Description page of Project Settings.


#include "VotePlayerWidget.h"

#include "PotProbGameState.h"
#include "PotProbPlayerController.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "VoteWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "ToggleButton.h"
#include "Components/WrapBox.h"

void UVotePlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	VoteButton->OnClicked.AddDynamic(this, &UVotePlayerWidget::OnClickedAction);
}

void UVotePlayerWidget::NativeDestruct()
{
	Super::NativeDestruct();
	VoteButton->OnClicked.RemoveDynamic(this, &UVotePlayerWidget::OnClickedAction);
}

void UVotePlayerWidget::SetVoteCounts(int32 Votes)
{
	auto dots = VoteDots->GetAllChildren();
	if (Votes > dots.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Votes are more than dots"));
		return;
	}

	for (int i = 0; i < dots.Num(); i++)
	{
		auto dot = Cast<UImage>(dots[i]);
		if (dot)
		{
			if (i < Votes)
			{
				dot->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
			else
			{
				dot->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void UVotePlayerWidget::OnClickedAction()
{
	if (GetOwningPlayer()->WasInputKeyJustReleased(EKeys::Enter))
	{
		return;
	}
	VoteWidget->SetPlayerSelected(PlayerName->GetText().ToString());

	for(UWidget* U : VoteWidget->VotePlayers->GetAllChildren())
	{
		if(UVotePlayerWidget* PlayerWidget = Cast<UVotePlayerWidget>(U))
		{
			if (PlayerWidget != this && PlayerWidget->VoteButton->GetIsToggled())
			{
				PlayerWidget->VoteButton->Toggle();
			}
		}
	}
}
