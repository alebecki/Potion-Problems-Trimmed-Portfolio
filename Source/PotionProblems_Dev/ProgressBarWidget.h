// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProgressBarWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPercentChanged, float, NewPercent);

/**
 * 
 */
UCLASS(Blueprintable)
class POTIONPROBLEMS_DEV_API UProgressBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:

    UPROPERTY(BlueprintAssignable, Category = "UI")
    FOnPercentChanged OnPercentChanged;
    
    void SetPercent(float NewPercent);
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar")
    float Percent = 0.0f;
};