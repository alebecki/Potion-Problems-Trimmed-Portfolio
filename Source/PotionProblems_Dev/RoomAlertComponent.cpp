// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomAlertComponent.h"
#include "PotProbPlayerState.h"
#include "PotProbZDCharacter.h"


URoomAlertComponent::URoomAlertComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URoomAlertComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetIsReplicated(false);

	ROVMesh = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
	ROVMesh->OnComponentBeginOverlap.AddDynamic(this, &URoomAlertComponent::HandleBeginOverlap);

	GetWorld()->GetTimerManager().SetTimer(PonderOrbTimerHandle, this, &URoomAlertComponent::SetOrbTutorialReady, OrbTutorialDelay, false);
}

void URoomAlertComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ROVMesh->OnComponentBeginOverlap.RemoveDynamic(this, &URoomAlertComponent::HandleBeginOverlap);
}

void URoomAlertComponent::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(OtherActor))
	{
		if (Character->IsLocallyControlled())
		{
			if (APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(Character->GetPlayerState()))
			{
				// Play for troublemakers first time entering observatory
				if (RoomType == EPotProbRoomTypes::OBSERVATORY && PlayerState->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER && NumPlayerOverlaps < 1)
				{
					PlayerState->ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Bring an ingredient to a\ntelescope to cause mayhem!");
				}
				// Play when re-entering the potions classroom for the first time
				if (RoomType == EPotProbRoomTypes::CLASSROOM && NumPlayerOverlaps == 1)
				{
					if (bOrbTutorialReady)
					{
						PlayerState->ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Ponder the Orb if you wish\nto call a vote!");
					}
					else
					{
						NumPlayerOverlaps--;
					}
				}
				NumPlayerOverlaps ++;
			}
		}
	}
}