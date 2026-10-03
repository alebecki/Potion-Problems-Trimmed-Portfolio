// Fill out your copyright notice in the Description page of Project Settings.


#include "FriendButtonWidget.h"
#include "Components/Button.h"
#include "PotProbOnlineSubsystem.h"

void UFriendButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	JoinButton->OnClicked.AddDynamic(this, &UFriendButtonWidget::OnClickedAction);
}

void UFriendButtonWidget::NativeDestruct()
{
	JoinButton->OnClicked.RemoveDynamic(this, &UFriendButtonWidget::OnClickedAction);
}

void UFriendButtonWidget::OnClickedAction()
{
	UPotProbOnlineSubsystem* PotionsOnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
	if (!PotionsOnlineSubsystem)
	{
		return;
	}

	PotionsOnlineSubsystem->JoinFriendSession(FriendIndex);
}
