// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractWidget.h"

#include "Components/TextBlock.h"

void UInteractWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInteractWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UInteractWidget::UpdateText(FString& InputText)
{
	InteractText->SetText(FText::FromString(InputText));
}
