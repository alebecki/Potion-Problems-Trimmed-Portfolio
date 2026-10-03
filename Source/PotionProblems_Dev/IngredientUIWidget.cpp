// Fill out your copyright notice in the Description page of Project Settings.


#include "IngredientUIWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UIngredientUIWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UIngredientUIWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UIngredientUIWidget::MarkAsObtained()
{
	IngredientName->SetColorAndOpacity(IngredientHighlightColor);
	bisObtained = true;
}

void UIngredientUIWidget::MarkAsNotObtained()
{
	IngredientName->SetColorAndOpacity(IngredientEmptyColor);
	bisObtained = false;
}

void UIngredientUIWidget::SetIngredientInfo(UTexture2D* IngredientTexture, FString IngredientText)
{
	IngredientIcon->SetBrushFromTexture(IngredientTexture);
	IngredientName->SetText(FText::FromString(IngredientText));
}
