// Fill out your copyright notice in the Description page of Project Settings.


#include "ROVRoomTrigger.h"

#include "PotProbZDCharacter.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/GameSession.h"
#include "Kismet/GameplayStatics.h"

void AROVRoomTrigger::BeginPlay()
{
	Super::BeginPlay();
	bReplicates = false;

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AROVRoomTrigger::HandleBeginOverlap);
	GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &AROVRoomTrigger::HandleEndOverlap);
}

void AROVRoomTrigger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetCollisionComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &AROVRoomTrigger::HandleBeginOverlap);
	GetCollisionComponent()->OnComponentEndOverlap.RemoveDynamic(this, &AROVRoomTrigger::HandleEndOverlap);
}

void AROVRoomTrigger::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(OtherActor))
	{
		if (Character->IsLocallyControlled())
		{
			bContainsLocalPlayer = true;
			ROVMesh->SetActorHiddenInGame(false);

			// TODO: Replace auto?
			for (auto It = PlayersWithin.CreateIterator(); It; ++It)
			{
				PlayerController->HiddenActors.Remove(It->Get());
			}
		}
		else
		{
			if (bContainsLocalPlayer)
			{
				PlayerController->HiddenActors.Remove(Character);
			}
			PlayersWithin.Add(Character);
		}
	}
}

void AROVRoomTrigger::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(OtherActor))
	{
		if (Character->IsLocallyControlled())
		{
			bContainsLocalPlayer = false;
			ROVMesh->SetActorHiddenInGame(true);

			// dont add to hidden if player is currently clarivoyant
			if (Character->GetIsClarivoyant())
			{
				return;
			}
			
			// TODO: Replace auto?
			for (auto It = PlayersWithin.CreateIterator(); It; ++It)
			{
				PlayerController->HiddenActors.Add(It->Get());
			}
		}
		else
		{
			if (bContainsLocalPlayer)
			{
				PlayerController->HiddenActors.Add(Character);
			}
			PlayersWithin.Remove(Character);
		}
	}
}
