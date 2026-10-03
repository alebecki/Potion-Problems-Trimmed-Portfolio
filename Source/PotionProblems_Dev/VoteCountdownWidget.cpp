// Fill out your copyright notice in the Description page of Project Settings.


#include "VoteCountdownWidget.h"
#include "PotProbPlayerController.h"
#include "PotProbGameState.h"
#include "Components/TextBlock.h"


void UVoteCountdownWidget::NativeConstruct()
{
	Super::NativeConstruct();
	bHasScriptImplementedTick = true;
	Timer = CountdownDuration;
	// Stop all potion timers
	if (APotProbGameState* PotGameState =  GetWorld()->GetGameState<APotProbGameState>())
	{
		PotGameState->PauseTimers();
	}
}

void UVoteCountdownWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Timer -= InDeltaTime;
	int TimerInt = Timer;
	TimerText->SetText(FText::FromString(FString::Printf(TEXT("%i"), TimerInt)));
	if (Timer <= 0.0f)
	{
		if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			PC->Server_StartVotingPhase();
		}
		RemoveFromParent();
		Timer = FLT_MAX;
	}
	if (APotProbGameState* PotGameState = GetWorld()->GetGameState<APotProbGameState>())
	{
		if (PotGameState->CurrentPhase == EPotProbPhases::PHASE_END)
		{
			PotGameState->UnPauseTimers();
			RemoveFromParent();
		}
	}
}
