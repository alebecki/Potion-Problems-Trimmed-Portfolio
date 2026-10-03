// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PotProbEnums.h"
#include "PlayerNameTagWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UPlayerNameTagWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* NameText;
	
	void SetNameText(FString NewNameText);
	void SetColor(FLinearColor NewColor);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
};
