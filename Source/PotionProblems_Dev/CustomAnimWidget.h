// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "CustomAnimWidget.generated.h"

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UCustomAnimWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CustomButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonAnim;
	
	void SynchronizeProperties() override;
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ButtonText = "Lin";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ButtonTextAfterClick = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FontSize = 32.0f;
	bool bToggled = false;

protected:
	UFUNCTION()
	void HandleButtonClick();
};
