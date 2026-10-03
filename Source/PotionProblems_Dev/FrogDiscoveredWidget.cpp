// Fill out your copyright notice in the Description page of Project Settings.


#include "FrogDiscoveredWidget.h"

#include "PotProbPlayerController.h"
#include "Components/TextBlock.h"

class APotProbPlayerController;

void UFrogDiscoveredWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// set timer to destroy self
	GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &UFrogDiscoveredWidget::DestroySelf, FrogDiscoveredDuration, false);
}

void UFrogDiscoveredWidget::UpdateDiscoveredText(FString playerName)
{
	FrogDiscoveredText->SetText(FText::FromString(TEXT("A frogged ") + playerName + TEXT(" has been")));
}

void UFrogDiscoveredWidget::DestroySelf()
{
	RemoveFromParent();
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->Server_StartVoteCountdown();
	}
}
