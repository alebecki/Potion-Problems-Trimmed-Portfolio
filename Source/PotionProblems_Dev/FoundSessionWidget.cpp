// Fill out your copyright notice in the Description page of Project Settings.


#include "FoundSessionWidget.h"

#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CircularThrobber.h"
#include "PotProbOnlineSubsystem.h"
void UFoundSessionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	JoinButton->OnClicked.AddDynamic(this, &UFoundSessionWidget::OnClickedAction);
}

void UFoundSessionWidget::NativeDestruct()
{
	JoinButton->OnClicked.RemoveDynamic(this, &UFoundSessionWidget::OnClickedAction);
}

void UFoundSessionWidget::OnClickedAction()
{
	UPotProbOnlineSubsystem* PotionsSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
	if (!PotionsSubsystem)
	{
		return;
	}
	SessionLoadingThrobber->SetVisibility(ESlateVisibility::Visible);
	//MainMenu->LoadingPanelJoinPublic->SetVisibility(ESlateVisibility::Visible);
	if (bIsFriendSession)
	{
		PotionsSubsystem->JoinFriendSession(SessionIndex);
	}
	else
	{
		PotionsSubsystem->JoinFoundSession(SessionIndex);
	}
}
