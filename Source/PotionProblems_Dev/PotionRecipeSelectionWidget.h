// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PotProbGameMode.h"
#include "Blueprint/UserWidget.h"
#include "PotionRecipeSelectionWidget.generated.h"

class UButton;
class UHorizontalBox;
class URecipeWidget;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UPotionRecipeSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* RecipiesBox;

	UPROPERTY(meta = (BindWidget))
	UButton* PickAgain;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WarningText;

	UPROPERTY(meta = (BindWidget))
	class UImage* WarningArrow;

	void InitializeRecipes(const TArray<FRecipeStruct>& SelectedRecipes);
private:
	// In future will have to add like 4,5 ingredient widgets as well
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	TSubclassOf<URecipeWidget> RecipeWidgetClass;
	UFUNCTION()
	void RerollRecipies();
	UFUNCTION()
	void CleanUpUI();
};
