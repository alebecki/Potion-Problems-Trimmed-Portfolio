// Fill out your copyright notice in the Description page of Project Settings.


#include "ProxyMessageWidget.h"
#include "Components/TextBlock.h"

void UProxyMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	GetWorld()->GetTimerManager().SetTimer(AutoDeleteTimerHandle, this, &UProxyMessageWidget::HandleAutoDelete, AutoDeleteTime, false);
}

void UProxyMessageWidget::NativeDestruct()
{
	Super::NativeDestruct();

	GetWorld()->GetTimerManager().ClearTimer(AutoDeleteTimerHandle);
}

void UProxyMessageWidget::SetMessageText(const FText Text)
{
	ProxyMessageText->SetText(Text);
}

void UProxyMessageWidget::HandleAutoDelete()
{
	if (GetParent())
	{
		RemoveFromParent();
	}
}
