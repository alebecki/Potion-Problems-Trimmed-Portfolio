// Fill out your copyright notice in the Description page of Project Settings.


#include "CauldronIngWidget.h"

#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "IngredientUIWidget.h"
#include "IngredientData.h"
#include "Engine/DataTable.h"
#include "CauldronActor.h"


void UCauldronIngWidget::NativeConstruct()
{
    Super::NativeConstruct();

    IngredientMarkerWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UCauldronIngWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    if (IsValid(CauldronActor))
    {

            IngredientMarkerWidget->SetVisibility(ESlateVisibility::Visible);
            return;
    }
    IngredientMarkerWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UCauldronIngWidget::SetIngredientCountText(FString IngredientCountTextInput)
{
	IngredientCountText->SetText(FText::FromString(IngredientCountTextInput));
}

void UCauldronIngWidget::SetOwningPlayerController(class APlayerController* InPlayerController)
{
    OwningController = InPlayerController;
}

void UCauldronIngWidget::SetCauldronActor(ACauldronActor* InCauldronActor)
{
    CauldronActor = InCauldronActor;
}

void UCauldronIngWidget::UpdateIngredientMarkers(TArray<FName> NewNameArray)
{
    NameArray = NewNameArray;
    if (NameArray.Num() == 0)
    {
        IngredientMarkerWidget->ClearChildren();
    }
    else 
    {

        // draw new ingredient
        FName IngredientName = NameArray.Last();
        UIngredientUIWidget* NewIngredImg = CreateWidget<UIngredientUIWidget>(GetWorld()->GetGameInstance(), IngredientImgClass);
            
        if (NewIngredImg)
        {
            IngredientMarkerWidget->AddChild(NewIngredImg);
                
            for (const auto& Row : IngredientDataTable->GetRowMap())
            {
                FIngredients* CurrIngredient = (FIngredients*)Row.Value;
        
                // Check if the current row's Ingredients field matches the ingredient name
                if (CurrIngredient && CurrIngredient->Ingredient == IngredientName)
                {
                    NewIngredImg->SetIngredientInfo(CurrIngredient->IngredientSprite->GetBakedTexture(), IngredientName.ToString());
                }
            }
        }
    }
}
