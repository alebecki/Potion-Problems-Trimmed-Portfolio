// Fill out your copyright notice in the Description page of Project Settings.


#include "DisguiseUIButton.h"

#include "CharacterData.h"
#include "PotProbPlayerState.h"
#include "PotProbZDCharacter.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UDisguiseUIButton::NativeConstruct()
{
	Super::NativeConstruct();

	TArray<FCharacterData*> Rows;
	static const FString ContextString(TEXT("Character Data Context"));
	CharacterDataTable->GetAllRows<FCharacterData>(ContextString, Rows);
	
	for (const FCharacterData* Row : Rows)
	{
		if (Row)
		{
			CharacterTextures.Add(Row->CharacterModel, Row->CharacterTexture);
		}
	}
	
	SelectButton->OnClicked.AddDynamic(this, &UDisguiseUIButton::OnButtonClicked);
}

void UDisguiseUIButton::NativeDestruct()
{
	SelectButton->OnClicked.RemoveAll(this);
	Super::NativeDestruct();
}

void UDisguiseUIButton::SetPlayerName(const FString& PlayerText) const
{
	PlayerName->SetText(FText::FromString(PlayerText));
}

void UDisguiseUIButton::SetAnimModel(EAnimModels AnimModelIndex)
{
	CharacterImage->SetBrushFromTexture(Cast<UTexture2D>(CharacterTextures[AnimModelIndex]));
	AnimModel = AnimModelIndex;
}

void UDisguiseUIButton::OnButtonClicked()
{
	APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(GetOwningPlayerPawn());
	if(!Character)
	{
		return;
	}

	Character->Server_SetModelOverride(AnimModel);
	Character->SetPlayerText(PlayerName->GetText().ToString());
	
	APotProbPlayerState* PlayerState = Character->GetPlayerState<APotProbPlayerState>();
	if(!PlayerState)
	{
		return;
	}
	PlayerState->ServerStartDisguise(Character->DisguisePotionDuration);

	if (ParentHUDWidget)
	{
		ParentHUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
