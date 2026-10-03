// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInfo.h"

#include "CharacterCustomizeWidget.h"
#include "CharacterData.h"
#include "PotProbPlayerController.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ToggleButton.h"
#include "Components/UniformGridPanel.h"
#include "Kismet/GameplayStatics.h"

void UCharacterInfo::NativeConstruct()
{
	Super::NativeConstruct();
	CharacterButton->OnClicked.AddDynamic(this, &UCharacterInfo::OnCharacterWidgetClicked);
}

void UCharacterInfo::NativeDestruct()
{
	Super::NativeDestruct();
	CharacterButton->OnClicked.RemoveAll(this);
}

void UCharacterInfo::InitializeTextAndIcons(const FCharacterData& CharacterInfo)
{
	CharacterImage->SetBrushFromTexture(Cast<UTexture2D>(CharacterInfo.CharacterTexture), true);
	CharacterName->SetText(FText::FromString(CharacterInfo.CharacterName.ToString()));
	CharacterModel = CharacterInfo.CharacterModel;
	CharacterLore = CharacterInfo.CharacterLore;
}

void UCharacterInfo::OnCharacterWidgetClicked()
{
	FCharacterData CharacterData;
	CharacterData.CharacterName = FName(*CharacterName->GetText().ToString());
	CharacterData.CharacterTexture = Cast<UTexture>(CharacterImage->GetBrush().GetResourceObject());
	CharacterData.CharacterModel = CharacterModel;
	CharacterData.CharacterLore = CharacterLore;
	
	if (UCharacterCustomizeWidget* CharacterCustomizeWidget = Cast<UCharacterCustomizeWidget>(WidgetInstigator))
	{
		CharacterCustomizeWidget->SetSelectedCharacter(CharacterData);

		for(UWidget* U : CharacterCustomizeWidget->OptionsPanel->GetAllChildren())
		{
			if(UCharacterInfo* PlayerWidget = Cast<UCharacterInfo>(U))
			{
				if (PlayerWidget != this && PlayerWidget->CharacterButton->GetIsToggled())
				{
					PlayerWidget->CharacterButton->Toggle();
				}
			}
		}
	}
	
	/*if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		FCharacterData CharacterData;
		CharacterData.CharacterName = FName(*CharacterName->GetText().ToString());
		CharacterData.CharacterTexture = Cast<UTexture>(CharacterImage->GetBrush().GetResourceObject());
		CharacterData.CharacterModel = CharacterModel;
		CharacterData.CharacterLore = CharacterLore;
		PC->SelectCharacter(CharacterData);
	}*/
}
