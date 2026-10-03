#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RarityData.generated.h"

USTRUCT(BlueprintType)
struct FRarities : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity")
    FName Rarity;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingredient")
    float Percentage;
};