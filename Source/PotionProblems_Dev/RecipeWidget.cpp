// Fill out your copyright notice in the Description page of Project Settings.


#include "RecipeWidget.h"

#include "IngredientUIWidget.h"
#include "PaperSprite.h"
#include "PotProbGameMode.h"
#include "PotProbPlayerState.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"

void URecipeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RecipeButton->OnClicked.AddDynamic(this, &URecipeWidget::SetTargetRecipe);
}

void URecipeWidget::NativeDestruct()
{
	RecipeButton->OnClicked.RemoveAll(this);
	Super::NativeDestruct();
}

void URecipeWidget::InitializeTextAndIcons(const FRecipeStruct& InputRecipeStruct)
{
	// First Initialize Potion Icon
	RecipeImage->SetBrushFromTexture(InputRecipeStruct.Potion.GetDefaultObject()->GetPotionSprite()->GetBakedTexture());
	// Initialize Potion Name
	RecipeName->SetText(FText::FromString(InputRecipeStruct.Potion.GetDefaultObject()->GetPotionName()));
	// Initialize Potion Description
	PotionDescription->SetText(FText::FromString(InputRecipeStruct.Potion.GetDefaultObject()->GetPotionDescription()));
	// For the Ingredients Initialize Icon --> Text
	if(!IngredientUIWidget || InputRecipeStruct.Ingredients.Num() != InputRecipeStruct.IngredientSprites.Num())
	{
		return;
	}
	InformationBox->ClearChildren();
	for(int32 I = 0; I < InputRecipeStruct.Ingredients.Num(); I++)
	{
		TObjectPtr<UIngredientUIWidget> NewIngredient = NewObject<UIngredientUIWidget>(this, IngredientUIWidget);
		InformationBox->AddChildToVerticalBox(NewIngredient);
		NewIngredient->SetIngredientInfo(InputRecipeStruct.IngredientSprites[I], InputRecipeStruct.Ingredients[I].ToString());
	}
	RecipeStruct = InputRecipeStruct;
}

void URecipeWidget::SetTargetRecipe()
{
	APotProbPlayerState* PotionPlayerState = Cast<APotProbPlayerState>(GetOwningPlayerState());
	if(!PotionPlayerState)
	{
		return;
	}
	//PotionPlayerState->ResetRerolls();
	PotionPlayerState->Server_SetCurrentRecipe(RecipeStruct);
}

void URecipeWidget::MarkIngredientAsSelected(int32 IngredientIndex)
{
	TArray<UWidget*> Widgets = InformationBox->GetAllChildren();
	UIngredientUIWidget* IngredientUIWidgetChild = Cast<UIngredientUIWidget>(Widgets[IngredientIndex]);
	if(IngredientUIWidgetChild)
	{
		IngredientUIWidgetChild->MarkAsObtained();
	}
}

void URecipeWidget::ClearIngredientSelections()
{
	TArray<UWidget*> Widgets = InformationBox->GetAllChildren();
	for (int IngredientIndex = 0;IngredientIndex < Widgets.Num();IngredientIndex ++)
	{
		if(UIngredientUIWidget* IngredientUIWidgetChild = Cast<UIngredientUIWidget>(Widgets[IngredientIndex]))
		{
			IngredientUIWidgetChild->MarkAsNotObtained();
		}
	}
}

bool URecipeWidget::GetIsObtained(int32 I)
{
	TArray<UWidget*> Widgets = InformationBox->GetAllChildren();
	UIngredientUIWidget* IngredientUIWidgetChild = Cast<UIngredientUIWidget>(Widgets[I]);
	if (IngredientUIWidgetChild)
	{
		return IngredientUIWidgetChild->GetIsObtained();
	}
	return false;
}
