// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldSpaceWidgetActor.h"
#include "ObservatoryWorldActor.generated.h"

class UObservatoryScreenWidget;
class UCanvasPanelSlot;


/**
 * FOR WORLD SPACE MINIGAMES
 *
 * Actor that anchors the observatory sky widget in world
 */

UCLASS()
class POTIONPROBLEMS_DEV_API AObservatoryWorldActor : public AWorldSpaceWidgetActor
{
	GENERATED_BODY()

public:

	AObservatoryWorldActor();

	/** STAR */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SkyWidth = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SkyHeight = 500.0f;
	// distance traveled ast sky edge before resetting star
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ResetBuffer = 50.0f;
	UFUNCTION(BlueprintCallable)
	FVector2D GetStarLocation() { return StarLocation;}
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ResetStar();
	UFUNCTION()
	void OnRep_StarLocation();
	
protected:
	
	TObjectPtr<UObservatoryScreenWidget> ObservatoryWidget;

	UPROPERTY(ReplicatedUsing = OnRep_StarLocation)
	FVector2D StarLocation;
	
	FVector2D TargetLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Star")
	float ResetDistance = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Star")
	float StarMoveSpeed = 50.0f;

	virtual void Tick(float DeltaTime) override;

	
	
};
