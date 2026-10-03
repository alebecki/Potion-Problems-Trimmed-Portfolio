// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CountdownWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UCountdownWidget : public UUserWidget
{
	GENERATED_BODY()
		
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TimerText;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CountdownDuration = 10.0f;

private:
	float Timer = 0.0f;

};
