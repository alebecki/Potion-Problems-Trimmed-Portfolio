#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PaperSprite.h"
#include "IngredientData.generated.h"

USTRUCT(BlueprintType)
struct FIngredients : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingredient")
    FName Ingredient;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingredient")
    UPaperSprite* IngredientSprite;
};