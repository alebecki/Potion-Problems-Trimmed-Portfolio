// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomAnimWidget.h"
#include "Components/TextBlock.h"

void UCustomAnimWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (CustomButton != nullptr)
	{
		FSlateFontInfo info = CustomButton->GetFont();
		
		info.Size = FontSize;
		CustomButton->SetFont(info);
		CustomButton->SetText(FText::FromString(ButtonText));
	}
}

void UCustomAnimWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ButtonAnim)
	{
		ButtonAnim->OnClicked.AddDynamic(this, &UCustomAnimWidget::HandleButtonClick);
	}
}

void UCustomAnimWidget::HandleButtonClick()
{
	if (!CustomButton) return;

	if (!ButtonTextAfterClick.IsEmpty())
	{
		bToggled = !bToggled;
		const FString& NewText = bToggled ? ButtonTextAfterClick : ButtonText;
		CustomButton->SetText(FText::FromString(NewText));
	}
	else
	{
		CustomButton->SetText(FText::FromString(ButtonText));
	}
}

