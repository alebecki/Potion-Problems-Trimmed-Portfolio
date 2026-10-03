// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DisguiseUIButton.generated.h"

enum class EAnimModels : uint8;
class UTextBlock;
class UButton;
class UImage;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UDisguiseUIButton : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void SetPlayerName(const FString& PlayerText) const;
	void SetAnimModel(EAnimModels AnimModelIndex);
	void SetParentHUDWidget(UUserWidget* ParentWidget) { ParentHUDWidget = ParentWidget; };
	UPROPERTY(meta = (BindWidget))
	UButton* SelectButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerName;
	UPROPERTY(meta = (BindWidget))
	UImage* CharacterImage;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* CharacterDataTable;
	UPROPERTY(Transient)
	TMap<EAnimModels, UTexture*> CharacterTextures;

private:
	UPROPERTY()
	UUserWidget* ParentHUDWidget;
	UFUNCTION()
	void OnButtonClicked();
	EAnimModels AnimModel;
};
