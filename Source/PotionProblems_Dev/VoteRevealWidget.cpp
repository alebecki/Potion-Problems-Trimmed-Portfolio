// Fill out your copyright notice in the Description page of Project Settings.


#include "VoteRevealWidget.h"

#include "PotProbGameMode.h"
#include "PotProbGameState.h"
#include "Components/TextBlock.h"

void UVoteRevealWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // set timer to destroy self
    GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &UVoteRevealWidget::DestroySelf, VoteRevealDuration, false);
}

void UVoteRevealWidget::UpdateRevealText(bool playerFrogged, FString playerName)
{
    if (!playerFrogged)
    {
       VoteRevealText->SetText(FText::FromString(TEXT("No one")));
    }
    else
    {
        VoteRevealText->SetText(FText::FromString(playerName));
    }
}

void UVoteRevealWidget::DestroySelf()
{
    if (APotProbGameMode* GameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode()))
    {
        GameMode->CheckWin();
    }
   if (APotProbGameState* PotGameState =  GetWorld()->GetGameState<APotProbGameState>())
   {
       //PotGameState->UnPauseTimers();
   }
    RemoveFromParent();
}