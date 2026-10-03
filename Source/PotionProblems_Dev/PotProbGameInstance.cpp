// Fill out your copyright notice in the Description page of Project Settings.


#include "PotProbGameInstance.h"
#include "PotProbOnlineSubsystem.h"
#include "PotProbPlayerState.h"

void UPotProbGameInstance::Init()
{
	Super::Init();

	// Bind to OnNetwordFailure
	GetEngine()->OnNetworkFailure().AddUObject(this, &UPotProbGameInstance::HandleNetworkFailure);
}

void UPotProbGameInstance::NotifyClientsOfNewHost_Implementation(const FString& NewHostIP)
{
	if (LocalWorld != nullptr)
	{
		for (APlayerState* PlayerState : LocalWorld->GetGameState()->PlayerArray)
		{
			if (APotProbPlayerState* PotProbPlayerState = Cast<APotProbPlayerState>(PlayerState))
			{
				if (APlayerController* PC = Cast<APlayerController>(PlayerState->GetOwner()))
				{
					if (PotProbPlayerState->GetIsPlayAgain())
					{
						PC->ClientTravel(NewHostIP, TRAVEL_Absolute);
					}
				}
			}
		}
	}
}

void UPotProbGameInstance::StartHost_Implementation()
{
	if (UPotProbOnlineSubsystem* OnlineSubsystem = GetSubsystem<UPotProbOnlineSubsystem>())
	{
		OnlineSubsystem->HostSession();
	}
}

void UPotProbGameInstance::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
                                                const FString& ErrorString)
{
	
}