// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RarityData.h"
#include "PotionObject.generated.h"

UENUM(BlueprintType)
enum class EPotionChargeType : uint8
{
	CHARGE_TRANSLOCATION,
	CHARGE_BLINK,
	CHARGE_SMEAR,
	CHARGE_NONE
};
class UPaperSprite;
class UAkAudioEvent;
/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class POTIONPROBLEMS_DEV_API UPotionObject : public UObject
{
	GENERATED_BODY()
public:
	UPotionObject();

	UPROPERTY(EditDefaultsOnly)
	UAkAudioEvent* PotionSound;

	UFUNCTION(CallInEditor)
	TArray<FName> GetRarityOptions() const
    {
            TArray<FName> NameOptions;
            static const FString ContextString(TEXT("Rarity Data Context"));
    
            if (RarityDataTable)
            {
            	
                TArray<FRarities*> Rows;
    
                RarityDataTable->GetAllRows(ContextString, Rows);
    
                for (const FRarities* Row : Rows)
                {
                    if (Row)
                    {
                        NameOptions.Add(Row->Rarity);
                    }
                }
            }
        return NameOptions;
    }

	UPaperSprite* GetPotionSprite() const { return PotionIconSprite; }
	const FString& GetPotionName() const { return PotionName; }
	const FString& GetPotionDescription() const { return PotionDescription; }
	const int32 GetPotionQuantity() const { return PotionQuantity; }
	int32 GetPotionCharges() const { return NumChargesToGive; }
	EPotionChargeType GetPotionChargeType() const { return PotionChargeType; }
	bool GetCanSplash() const { return bCanSplash; }
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ActivatePotionForCaller(APlayerState* CallerPlayerState);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ActivatePotionForTarget(APlayerState* CallerPlayerState, APlayerState* TargetPlayerState);

	const FName& GetPotionRarity() const { return PotionRarity; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	UPaperSprite* PotionIconSprite;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString PotionName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString PotionDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	EPotionChargeType PotionChargeType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 NumChargesToGive;
	// Number of recipes that should map to this potion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 PotionQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* RarityDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity", meta = (GetOptions = "GetRarityOptions"))
	FName PotionRarity;
	// Can the player splash this potion on other stuff? (Interact) Or is it only drinkable
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated)
	bool bCanSplash;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }
};
