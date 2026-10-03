// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PotionStatusWidget.generated.h"

/**
 * 
 */

class UImage;
class UProgressBar;

UCLASS()
class POTIONPROBLEMS_DEV_API UPotionStatusWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Meta = (BindWidget))
	UImage* CurrentPotionEffectImage;
	UPROPERTY(Meta = (BindWidget))
	UProgressBar* PotionEffectTimeRemainingProgressBar;

	void SetInitialStatusTime(float InitTime)
	{
		InitialStatusTime = InitTime;
		TimeRemaining = InitTime;
	}
	float GetInitialStatusTime() { return InitialStatusTime; }
	void SetTimRemaining(float Time) { TimeRemaining = Time; }
	float GetTimeRemaining() { return TimeRemaining; }

	void ResetTimer() { TimeRemaining = InitialStatusTime; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	float InitialStatusTime = 3.0f;
	float TimeRemaining;

};
