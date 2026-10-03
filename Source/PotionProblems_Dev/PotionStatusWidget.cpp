// Fill out your copyright notice in the Description page of Project Settings.


#include "PotionStatusWidget.h"

#include "PotProbGameState.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UPotionStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();
	TimeRemaining = InitialStatusTime;// lowkey just a placeholder, I still need to set it elsewhere because this is too early
}

void UPotionStatusWidget::NativeDestruct()
{
	
	Super::NativeDestruct();
}

void UPotionStatusWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	PotionEffectTimeRemainingProgressBar->SetPercent(TimeRemaining / InitialStatusTime);

	/*if (APotProbGameState* PotGameState = GetWorld()->GetGameState<APotProbGameState>())
	{
		if (PotGameState->CurrentPhase != EPotProbPhases::PHASE_VOTE)
		{
			TimeRemaining -= InDeltaTime;
		}
	}*/
	TimeRemaining -= InDeltaTime;

	if (TimeRemaining <= 0)
	{
		this->RemoveFromParent();
		this->Destruct();
	}
}
