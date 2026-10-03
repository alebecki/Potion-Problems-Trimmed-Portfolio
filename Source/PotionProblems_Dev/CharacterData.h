#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PaperSprite.h"
#include "CharacterData.generated.h"

enum class EAnimModels : uint8;

USTRUCT(BlueprintType)
struct FCharacterData : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FName CharacterName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    UTexture* CharacterTexture;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    UTexture* HeadOnlyTexture;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    EAnimModels CharacterModel;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FText CharacterLore;
};