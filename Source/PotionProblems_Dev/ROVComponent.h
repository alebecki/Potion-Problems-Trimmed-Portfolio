// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ROVComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POTIONPROBLEMS_DEV_API UROVComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UROVComponent();

	bool GetContainsLocalPlayer() const {return bContainsLocalPlayer;}
	
	TArray<TObjectPtr<class APotProbZDCharacter>> PlayersWithin;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool bContainsLocalPlayer = false;
	UPROPERTY()
	TObjectPtr<class APlayerController> PlayerController;
	UPROPERTY()
	TObjectPtr<class UStaticMeshComponent> ROVMesh;

	UPROPERTY(EditInstanceOnly)
	FName RoomName;
};
