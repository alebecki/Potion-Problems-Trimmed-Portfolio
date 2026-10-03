// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "ChillyBoostTrigger.generated.h"

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API AChillyBoostTrigger : public ATriggerBox
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void HandleAutoDelete();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoDeleteTime = 3.0f;
};
