// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PotionWidget.generated.h"

class UPotionObject;
class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UPotionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct();
	void NativeDestruct();
	
	UFUNCTION(BlueprintCallable)
	void SetPotionText(UPotionObject* Potion);

	UFUNCTION()
	void DestroyWidget();

	// Currently Disabled (uncomment timers in native construct and destory)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Designer Variable")
	float AutoDestroyTime = 15.0f;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> Name;
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> PotionImage;
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> Description;

protected:
	FTimerHandle AutoDestroyTimerHandle;
};
