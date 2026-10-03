// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyStatusWidget.h"

#include "HUDWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"

void ULobbyStatusWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (UHUDWidget* HUD = Cast<UHUDWidget>(GetOuter()))
	{
		HUD->LobbyStatusWidgets.Remove(Username->GetText().ToString());
	}
}

void ULobbyStatusWidget::SetName(const FString& InName)
{
	Username->SetText(FText::FromString(InName));
}

void ULobbyStatusWidget::SetActive(bool InActive)
{
	if (ToggleSwitcher)
	{
		if (InActive)
		{
			ToggleSwitcher->SetActiveWidgetIndex(0);
			Username->SetColorAndOpacity(FLinearColor::White);
		}
		else
		{
			ToggleSwitcher->SetActiveWidgetIndex(1);
			Username->SetColorAndOpacity(FLinearColor::Red);
		}
	}
}
