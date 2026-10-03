// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "InteractSubsystem.generated.h"

class UInteractComponent;
class APotProbPlayerController;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UInteractSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual TStatId GetStatId() const override;
	void AddComponent(UInteractComponent* NewComponent);
	void RemoveComponent(UInteractComponent* TargetComponent);
	void PerformInteract(APawn* InstigatorPawn, UInteractComponent* InteractTarget);
	void PerformInteractHeld(APawn* InstigatorPawn, UInteractComponent* InteractTarget);
	void PerformInteractSuperHeld(APawn* InstigatorPawn, UInteractComponent* InteractTarget);
	TWeakObjectPtr<UInteractComponent> GetBestCandidate() { return BestCandidate; }
protected:
	virtual void Tick(float DeltaTime) override;
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	TArray<TWeakObjectPtr<UInteractComponent>> InteractComponents;
	TWeakObjectPtr<UInteractComponent> BestCandidate;
};
