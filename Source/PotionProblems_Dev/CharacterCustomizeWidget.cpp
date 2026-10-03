// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCustomizeWidget.h"
#include "CharacterData.h"
#include "CharacterInfo.h"
#include "HUDWidget.h"
#include "PotProbGameState.h"
#include "Components/Button.h"
#include "Components/ScaleBox.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "PotProbPlayerController.h"
#include "PotProbPlayerState.h"
#include "PotProbZDCharacter.h"
#include "ToggleButton.h"
#include "Kismet/GameplayStatics.h"

struct FCharacterData;

void UCharacterCustomizeWidget::PopulateCharacterInfo()
{
	if (!CharacterDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterDataTable is not set"));
		return;
	}

	if (!CharacterInfoClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterInfoClass is not set"));
		return;
	}

	// Clear the grid before proceeding
	OptionsPanel->ClearChildren();
	CharacterLore->SetText(FText::FromString(TEXT("")));
	CharacterName->SetText(FText::FromString(TEXT("")));
	
	TArray<FCharacterData*> Rows;
	static const FString ContextString(TEXT("Character Data Context"));
	CharacterDataTable->GetAllRows<FCharacterData>(ContextString, Rows);
	
	for (const FCharacterData* Row : Rows)
	{
		if (Row)
		{
			if (OptionsPanel->GetChildrenCount() >= MaxOptions)
			{
				break;
			}
			
			TObjectPtr<UCharacterInfo> NewButton = NewObject<UCharacterInfo>(this, CharacterInfoClass);
			int CurrOptions = OptionsPanel->GetChildrenCount();
			UUniformGridSlot* NewButtonSlot = OptionsPanel->AddChildToUniformGrid(NewButton,
				CurrOptions / OptionPanelPerRow, CurrOptions % OptionPanelPerRow);
			NewButton->InitializeTextAndIcons(*Row);
			NewButton->SetOwningPlayer(GetOwningPlayer());
			NewButton->SetWidgetInstigator(this);
		}
	}
	
}

void UCharacterCustomizeWidget::SelectCharacter(FCharacterData CharacterData)
{
	SelectedPanel->ClearChildren();
	TObjectPtr<UCharacterInfo> NewButton = NewObject<UCharacterInfo>(this, CharacterInfoClass);
	SelectedPanel->AddChild(NewButton);
	// UUniformGridSlot* NewButtonSlot = SelectedPanel->AddChildToUniformGrid(NewButton);
	NewButton->InitializeTextAndIcons(CharacterData);

	CharacterName->SetText(FText::FromString(CharacterData.CharacterName.ToString()));
	CharacterLore->SetText(CharacterData.CharacterLore);
}

void UCharacterCustomizeWidget::SetSelectedCharacter(FCharacterData NewCharacter)
{
	SelectedCharacterData = NewCharacter;
	SelectCharacter(SelectedCharacterData);
}

void UCharacterCustomizeWidget::UpdateCharCustomStatus()
{
	if (APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
	{
		const auto& ModelOwnership = GameState->GetModelOwnership();
		for (int i = 0; i < OptionsPanel->GetChildrenCount(); ++i)
		{
			if (UCharacterInfo* CharInfo = Cast<UCharacterInfo>(OptionsPanel->GetChildAt(i)))
			{
				if (ModelOwnership[static_cast<int32>(CharInfo->CharacterModel)] != -1 && SelectedCharacterData.CharacterModel != CharInfo->CharacterModel)
				{
					CharInfo->CharacterButton->SetIsEnabled(false);
				} else
				{
					CharInfo->CharacterButton->SetIsEnabled(true);
				}
			}
		}
	}
}

FCharacterData UCharacterCustomizeWidget::GetCharacterDataFromModel(EAnimModels Model)
{
	TArray<FCharacterData*> Rows;
	static const FString ContextString(TEXT("Character Data Context"));
	CharacterDataTable->GetAllRows<FCharacterData>(ContextString, Rows);

	for (const FCharacterData* Row : Rows)
	{
		if (Row && Row->CharacterModel == Model)
		{
			return *Row;
		}
	}

	// Unable to match model index to character data.
	return FCharacterData();
}

void UCharacterCustomizeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PopulateCharacterInfo();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UCharacterCustomizeWidget::UpdateSelectedCharacter, 1.0f, false);
	
	
	CloseCharacterCustomizeButton->OnClicked.AddDynamic(this, &UCharacterCustomizeWidget::SetCollapsed);
	
}

void UCharacterCustomizeWidget::NativeDestruct()
{
	Super::NativeDestruct();

	CloseCharacterCustomizeButton->OnClicked.RemoveDynamic(this, &UCharacterCustomizeWidget::SetCollapsed);
}

void UCharacterCustomizeWidget::HandleSelection()
{
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		// Null Check
		if (!SelectedCharacterData.CharacterName.IsNone())
		{
			PC->SelectCharacter(SelectedCharacterData);
			// Collapse after selecting a character
			SetCollapsed();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Character name is null!\n"));
		}
	}
}

void UCharacterCustomizeWidget::UpdateSelectedCharacter()
{
	if (APotProbZDCharacter* ZDCharacter = Cast<APotProbZDCharacter>(GetOwningPlayer()->GetCharacter()) )
	{
		TArray<FCharacterData*> Rows;
		static const FString ContextString(TEXT("Character Data Context"));
		CharacterDataTable->GetAllRows<FCharacterData>(ContextString, Rows);

		for (const FCharacterData* Row : Rows)
		{
			if (Row && Row->CharacterModel == ZDCharacter->GetPlayerSelectedModel())
			{
				SetSelectedCharacter(*Row);
				if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetOwningPlayer()))
				{
					PC->GetHUDWidgetInstance()->UpdateCharacterSprite(*Row);
				}
				break;
			}
		}
	}

}

