// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngredientUIWidget.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UIngredientUIWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	UImage* IngredientIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* IngredientName;
	// Color for when you obtain an ingredient
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	FSlateColor IngredientHighlightColor;
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	FSlateColor IngredientEmptyColor;
	void MarkAsObtained();
	void MarkAsNotObtained();
	void SetIngredientInfo(UTexture2D* IngredientTexture, FString IngredientText);
	bool GetIsObtained() { return bisObtained; }
private:
	bool bisObtained = false;
};
