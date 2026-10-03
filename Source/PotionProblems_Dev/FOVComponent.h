// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FOVComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POTIONPROBLEMS_DEV_API UFOVComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFOVComponent();

	void GenerateParams(TArray<TObjectPtr<class AActor>> Players);

	void SetRayLength(float x) {RayLength = x;}

	TArray<TObjectPtr<class AActor>> OtherPlayers;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	FCollisionQueryParams Params;

	//class UDynamicMeshComponent* DynamicMesh = nullptr;
	class UProceduralMeshComponent* ProceduralMesh = nullptr;
	TArray<FVector> Vertices;

	/*
	 * Variables to edit in BP
	 */
	UPROPERTY(EditDefaultsOnly)
	int NumRays = 360;

	UPROPERTY(EditDefaultsOnly)
	float RayLength = 1000;

	UPROPERTY(EditDefaultsOnly)
	float TraceHeight = -35.0; // 0.0f
	
	// Additional distance above TraceHeight
	UPROPERTY(EditDefaultsOnly)
	float MeshHeight = 0;

	void GenerateMesh();
	void PerformLineTraces();
	void PreformLineTracesPlayers();

	void SetPlayersVisible();
};
