// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UInteractWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void UpdateText(FString& InputText);
	UPROPERTY(meta = (BindWidget))
	UTextBlock* InteractText;
	
};
