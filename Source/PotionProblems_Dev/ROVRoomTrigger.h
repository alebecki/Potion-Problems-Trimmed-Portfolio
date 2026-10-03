// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "ROVRoomTrigger.generated.h"

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API AROVRoomTrigger : public ATriggerBox
{
	GENERATED_BODY()

public:
	bool GetContainsLocalPlayer() const {return bContainsLocalPlayer;}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class AActor> ROVMesh;
	
	TArray<TObjectPtr<class APotProbZDCharacter>> PlayersWithin;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool bContainsLocalPlayer = false;
	
	UPROPERTY()
	TObjectPtr<class APlayerController> PlayerController; 
};
