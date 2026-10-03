// Fill out your copyright notice in the Description page of Project Settings.


#include "FrogDoorActor.h"

#include "PotProbPlayerState.h"
#include "AkGameplayStatics.h"
#include "PotProbZDCharacter.h"


AFrogDoorActor::AFrogDoorActor()
{
	EntranceNode = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EntranceNode"));
	EntranceNode->SetupAttachment(RootComponent);

	ExitNode = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExitNode"));
	ExitNode->SetupAttachment(RootComponent);
}

void AFrogDoorActor::MovePlayerToExit(APotProbZDCharacter* PlayerCharacter)
{
	if(!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("We are trying to use the frog door when we are not on the server"));
		return;
	}

	APotProbPlayerState* PlayerState = PlayerCharacter->GetPlayerState<APotProbPlayerState>();

	if(!PlayerState)
	{
		return;
	}

	if(!((PlayerState->bIsFrogged) || (PlayerCharacter->GetIsShrinking())))
	{
		// alert player they can't use frog door
		PlayerState->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "You are too big \nto fit through here!");
		
		if (ErrorSound) {
			FOnAkPostEventCallback nullCallback;
			UAkGameplayStatics::PostEvent(ErrorSound, PlayerState->GetPlayerController(), int32(0), nullCallback);
		}
		return;
	}
	
	float PlayerToEntrance = FVector::Dist2D(PlayerCharacter->GetActorLocation(), EntranceNode->GetComponentLocation());
	if(!IsValid(DoorToExitFrom))
	{
		float PlayerToExit = FVector::Dist2D(PlayerCharacter->GetActorLocation(), ExitNode->GetComponentLocation());
		FVector LeavingFrom = PlayerToEntrance < PlayerToExit ? ExitNode->GetComponentLocation() : EntranceNode->GetComponentLocation();
		PlayerCharacter->SetActorLocation(LeavingFrom, false, nullptr, ETeleportType::TeleportPhysics);
	}
	else
	{
		float PlayerToExit = FVector::Dist2D(PlayerCharacter->GetActorLocation(), DoorToExitFrom->EntranceNode->GetComponentLocation());
		FVector LeavingFrom = PlayerToEntrance < PlayerToExit ? DoorToExitFrom->EntranceNode->GetComponentLocation() : EntranceNode->GetComponentLocation();
		PlayerCharacter->SetActorLocation(LeavingFrom, false, nullptr, ETeleportType::TeleportPhysics);
	}
}
