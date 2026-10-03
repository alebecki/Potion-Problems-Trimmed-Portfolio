// Fill out your copyright notice in the Description page of Project Settings.


#include "PotionRecipeSelectionWidget.h"

#include "PotProbPlayerState.h"
#include "RecipeWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UPotionRecipeSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (WarningText)
	{
		WarningText->SetVisibility(ESlateVisibility::Collapsed);
	}
	//if (WarningArrow)
	//{
	//	WarningArrow->SetVisibility(ESlateVisibility::Collapsed);
	//}

	PickAgain->OnClicked.AddDynamic(this, &UPotionRecipeSelectionWidget::RerollRecipies);
}

void UPotionRecipeSelectionWidget::NativeDestruct()
{
	PickAgain->OnClicked.RemoveAll(this);
	Super::NativeDestruct();
}

void UPotionRecipeSelectionWidget::InitializeRecipes(const TArray<FRecipeStruct>& SelectedRecipes)
{
	APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(GetOwningPlayerState());

	if (!PlayerState)
	{
		return;
	}
	
	if (!RecipeWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("RecipeWidget class for PotionRecipeSelectionWidget is null. Please set them in the blueprint"));
		return;
	}
	
	for (int32 i = 0; i < SelectedRecipes.Num(); i++)
	{
		TObjectPtr<URecipeWidget> NewButton = NewObject<URecipeWidget>(this, RecipeWidgetClass);
		RecipiesBox->AddChildToHorizontalBox(NewButton);
		// Lowkey unsafe b/c we're assuming the that the texture.size == recipies.size but we'd catch that right? :D
		NewButton->InitializeTextAndIcons(SelectedRecipes[i]);
		// Bind to delegate to make sure that when a potion is selected, we can destroy this UI 
		NewButton->RecipeButton->OnClicked.AddDynamic(this, &UPotionRecipeSelectionWidget::CleanUpUI);
	}
	SetVisibility(ESlateVisibility::Visible);
}

void UPotionRecipeSelectionWidget::RerollRecipies()
{
		APotProbPlayerState* PlayerState = UGameplayStatics::GetPlayerController(this, 0)->GetPlayerState<APotProbPlayerState>();
			if (!PlayerState)
			{
				return;
			}
			//allows 4 times of reroll EXCEPT from the initialization
			if (PlayerState->GetNumRerolls() < 5) {
				RecipiesBox->ClearChildren();
				PlayerState->ServerRerollPotions();
			}

	if (WarningText)
	{
		WarningText->SetVisibility(ESlateVisibility::Visible);
	}
	//if (WarningArrow)
	//{
	//	WarningArrow->SetVisibility(ESlateVisibility::Visible);
	//}
}

void UPotionRecipeSelectionWidget::CleanUpUI()
{
	RecipiesBox->ClearChildren();
	SetVisibility(ESlateVisibility::Collapsed);
}
