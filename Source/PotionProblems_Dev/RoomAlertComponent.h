// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PotProbEnums.h"
#include "RoomAlertComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POTIONPROBLEMS_DEV_API URoomAlertComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URoomAlertComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	EPotProbRoomTypes RoomType;
	UPROPERTY()
	TObjectPtr<class UStaticMeshComponent> ROVMesh;
	int NumPlayerOverlaps = 0;

	FTimerHandle PonderOrbTimerHandle;
	float OrbTutorialDelay = 45.0f;
	bool bOrbTutorialReady = false;
	void SetOrbTutorialReady() { bOrbTutorialReady = true; }
};
