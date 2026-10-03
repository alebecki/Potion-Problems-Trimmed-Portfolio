// Fill out your copyright notice in the Description page of Project Settings.

#include "MinigameIngWidget.h"

#include "Components/TextBlock.h"
#include "IngredientUIWidget.h"
#include "IngredientData.h"
#include "Engine/DataTable.h"
#include "MinigameInitActor.h"


void UMinigameIngWidget::NativeConstruct()
{
    Super::NativeConstruct();

    IngredientImgWidget->SetVisibility(ESlateVisibility::Visible);
}

void UMinigameIngWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    if (IsValid(InitActor) && InitActor->GetCanInteract())
    {
        if (FVector::Distance(InitActor->GetActorLocation(), GetOwningPlayer()->GetPawn()->GetActorLocation()) <= ShowDistance)
        {
            IngredientImgWidget->SetVisibility(ESlateVisibility::Visible);
            return;
        }
    }
    IngredientImgWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UMinigameIngWidget::SetInitActor(AMinigameInitActor* InInitActor)
{
    InitActor = InInitActor;
}

void UMinigameIngWidget::UpdateIngredientMarker (const FName IngredientName) const
{
    for (const auto& Row : IngredientDataTable->GetRowMap())
    {
        FIngredients* CurrIngredient = (FIngredients*)Row.Value;
        
        // Check if the current row's Ingredients field matches the ingredient name
        if (CurrIngredient && CurrIngredient->Ingredient == IngredientName)
        {
            IngredientImgWidget->SetIngredientInfo(CurrIngredient->IngredientSprite->GetBakedTexture(), IngredientName.ToString());
            UE_LOG(LogTemp, Warning, TEXT("Minigame Ingredient Set"));
        }
    }
}
