// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "ToggleButton.generated.h"

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UToggleButton : public UButton
{
	GENERATED_BODY()

public:
	void PostLoad() override;
	void BeginDestroy() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style")
	FSlateBrush ToggledBrush;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style")
	FSlateBrush ToggledHoverBrush;

	const bool GetIsToggled() { return bIsToggled; }
	UFUNCTION(BlueprintCallable)
	void Toggle();

	virtual void SynchronizeProperties() override;

protected:
	UPROPERTY()
	FSlateBrush OriginalNormalBrush;
	UPROPERTY()
	FSlateBrush OriginalHoveredBrush;
	
	void UpdateButtonStyle();

	bool bIsToggled = false;
};
