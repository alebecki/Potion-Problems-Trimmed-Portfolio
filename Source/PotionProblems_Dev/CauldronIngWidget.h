// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngredientData.h"
#include "CauldronIngWidget.generated.h"

class UTextBlock;
class UIngredientUIWidget;
class UHorizontalBox;
class ACauldronActor;

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UCauldronIngWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* IngredientCountText;
	   
	// make sure to set not replicated
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* IngredientMarkerWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UIngredientUIWidget> IngredientImgClass;

	UPROPERTY(BlueprintReadOnly)
	ACauldronActor* CauldronActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ShowDistance = 150.0f;
	
	void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void SetIngredientCountText(FString IngredientCountTextInput);
	
	void SetOwningPlayerController(class APlayerController* InPlayerController);

	void SetCauldronActor(ACauldronActor* InCauldronActor);
	
	void UpdateIngredientMarkers(TArray<FName> NewNameArray);

protected:

    UPROPERTY()
	TObjectPtr<APlayerController> OwningController;

    UPROPERTY(BlueprintReadWrite, Category = "Names")
    TArray<FName> NameArray;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    UDataTable* IngredientDataTable;
};
