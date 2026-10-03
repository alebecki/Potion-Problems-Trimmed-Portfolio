// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinigameIngWidget.generated.h"

class UTextBlock;
class UIngredientUIWidget;
class UHorizontalBox;
class AMinigameInitActor;


/**
 * FOR WORLD SPACE MINIGAMES
 *
 * In-world caption that shows what ingredient is needed to play the minigame
 */

UCLASS()
class POTIONPROBLEMS_DEV_API UMinigameIngWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, BlueprintReadWrite)
	UIngredientUIWidget* IngredientImgWidget;
	void UpdateIngredientMarker (const FName IngredientName) const;
	
	UPROPERTY(BlueprintReadWrite)
	AMinigameInitActor* InitActor;
	void SetInitActor(AMinigameInitActor* InInitActor);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ShowDistance = 100.0f;

protected:
	
	UPROPERTY(BlueprintReadWrite, Category = "Names")
	TArray<FName> NameArray;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* IngredientDataTable;
};
