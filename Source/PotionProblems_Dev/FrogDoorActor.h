// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "FrogDoorActor.generated.h"

class APotProbZDCharacter;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API AFrogDoorActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AFrogDoorActor();
	// Moves Player who interacts with this door to the exit door (if it exists) otherwise it moves to the exit node
	UFUNCTION(BlueprintCallable)
	void MovePlayerToExit(APotProbZDCharacter* PlayerCharacter);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* EntranceNode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ExitNode;

	UPROPERTY(EditAnywhere)
	class UAkAudioEvent* ErrorSound;
private:
	// The door that we want this door to exit from
	UPROPERTY(EditAnywhere, Category = "Designer Variable")
	TObjectPtr<AFrogDoorActor> DoorToExitFrom;
	
};
