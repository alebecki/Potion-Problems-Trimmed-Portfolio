// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProxyMessageWidget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class POTIONPROBLEMS_DEV_API UProxyMessageWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	FTimerHandle AutoDeleteTimerHandle;
	
	UFUNCTION()
	void HandleAutoDelete();
	
public:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UTextBlock> ProxyMessageText;

	UPROPERTY(EditAnywhere)
	float AutoDeleteTime = 3.0f;

	void SetMessageText(const FText Text);
};
