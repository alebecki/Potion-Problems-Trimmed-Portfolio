// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FellowTBMKRWidget.generated.h"

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UFellowTBMKRWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UImage> PlayerImage;
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerName;
	
};
