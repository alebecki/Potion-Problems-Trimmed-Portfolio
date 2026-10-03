// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PotProbGameMode.h"
#include "Blueprint/UserWidget.h"
#include "RecipeWidget.generated.h"

class UVerticalBox;
class UIngredientUIWidget;
class UImage;
struct FRecipeStruct;
class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API URecipeWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	UPROPERTY(meta = (BindWidget))
	UImage* RecipeImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RecipeName;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PotionDescription;
	UPROPERTY(meta = (BindWidget))
	UButton* RecipeButton;
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* InformationBox;
	
	void InitializeTextAndIcons(const FRecipeStruct& RecipeStruct);
	void MarkIngredientAsSelected(int32 IngredientIndex);
	void ClearIngredientSelections();

	bool GetIsObtained(int32 I);
private:
	UFUNCTION()
	void SetTargetRecipe();
	FRecipeStruct RecipeStruct;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UIngredientUIWidget> IngredientUIWidget;

	//bool isObtained = false;

};
