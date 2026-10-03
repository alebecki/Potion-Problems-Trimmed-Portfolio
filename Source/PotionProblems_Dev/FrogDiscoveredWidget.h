// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VoteRevealWidget.h"
#include "FrogDiscoveredWidget.generated.h"

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UFrogDiscoveredWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* FrogDiscoveredText;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FrogDiscoveredDuration = 3.0f;
    
	void UpdateDiscoveredText(FString playerName = FString(TEXT("")));

private:

	FTimerHandle DestructionTimerHandle;

	void DestroySelf();
};
