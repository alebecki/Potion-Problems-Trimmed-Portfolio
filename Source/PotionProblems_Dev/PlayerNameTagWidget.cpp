// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerNameTagWidget.h"

#include "PotProbPlayerController.h"
#include "Components/TextBlock.h"

void UPlayerNameTagWidget::SetNameText(FString NewNameText)
{
	NameText->SetText(FText::FromString(NewNameText));
}

void UPlayerNameTagWidget::SetColor(FLinearColor NewColor)
{
	NameText->SetColorAndOpacity(NewColor);
}

void UPlayerNameTagWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
}
