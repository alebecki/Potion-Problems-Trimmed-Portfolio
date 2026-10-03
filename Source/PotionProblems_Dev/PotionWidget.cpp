// Fill out your copyright notice in the Description page of Project Settings.


#include "PotionWidget.h"

#include "PaperSprite.h"
#include "PotionObject.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UPotionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//GetWorld()->GetTimerManager().SetTimer(AutoDestroyTimerHandle, this, &UPotionWidget::DestroyWidget, AutoDestroyTime, false);
}

void UPotionWidget::NativeDestruct()
{
	Super::NativeDestruct();

	//GetWorld()->GetTimerManager().ClearTimer(AutoDestroyTimerHandle);
}

void UPotionWidget::SetPotionText(UPotionObject* Potion)
{
	if (Potion)
	{
		Name->SetText(FText::FromString(*Potion->GetPotionName()));
		PotionImage->SetBrushFromTexture(Potion->GetPotionSprite()->GetBakedTexture());
		Description->SetText(FText::FromString(*Potion->GetPotionDescription()));
	}
}

void UPotionWidget::DestroyWidget()
{
	RemoveFromParent();
}
