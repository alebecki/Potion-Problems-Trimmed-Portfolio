// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PotProbGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UPotProbGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	
	UFUNCTION(NetMulticast, Reliable)
	void NotifyClientsOfNewHost(const FString& NewHostIP);
	UFUNCTION(Client, Reliable)
	void StartHost();


protected:
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

	UPROPERTY()
	TObjectPtr<UWorld> LocalWorld = nullptr;
};
