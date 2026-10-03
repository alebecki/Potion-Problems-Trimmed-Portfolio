// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterData.h"
#include "CharacterCustomizeWidget.generated.h"

class UUniformGridPanel;
class UScaleBox;
class UCharacterInfo;
class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UCharacterCustomizeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (BindWidget))
	UUniformGridPanel* OptionsPanel;
	UPROPERTY(Meta = (BindWidget))
	UScaleBox* SelectedPanel;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* CharacterLore;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* CharacterName;

	UPROPERTY(Meta = (BindWidget))
	UButton* CloseCharacterCustomizeButton;

	UFUNCTION()
	void SetCollapsed() { SetVisibility(ESlateVisibility::Collapsed); }

	void PopulateCharacterInfo();
	void SelectCharacter(FCharacterData CharacterData);

	void SetSelectedCharacter(FCharacterData NewCharacter);

	UFUNCTION()
	void UpdateCharCustomStatus();

	UFUNCTION()
	FCharacterData GetCharacterDataFromModel(EAnimModels Model);

	//protected:
//	const FString characterNames{ name, name, name, name };
//
//	const FString characterDescriptions;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HandleSelection();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* CharacterDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	int OptionPanelPerRow = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	int MaxOptions = 10;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCharacterInfo> CharacterInfoClass;
	

	FCharacterData SelectedCharacterData;
	EAnimModels SelectedCharacterIndex;
	void UpdateSelectedCharacter();
};
