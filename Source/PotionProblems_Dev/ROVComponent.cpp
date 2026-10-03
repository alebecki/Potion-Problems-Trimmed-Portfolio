// Fill out your copyright notice in the Description page of Project Settings.


#include "ROVComponent.h"

#include "FOVComponent.h"
#include "PotProbPlayerController.h"
#include "PotProbZDCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UROVComponent::UROVComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UROVComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(false);

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ROVMesh = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
	ROVMesh->OnComponentBeginOverlap.AddDynamic(this, &UROVComponent::HandleBeginOverlap);
	ROVMesh->OnComponentEndOverlap.AddDynamic(this, &UROVComponent::HandleEndOverlap);
}

void UROVComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	ROVMesh->OnComponentBeginOverlap.RemoveDynamic(this, &UROVComponent::HandleBeginOverlap);
	ROVMesh->OnComponentEndOverlap.RemoveDynamic(this, &UROVComponent::HandleEndOverlap);
}


void UROVComponent::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(OtherActor))
	{
		if (Character->IsLocallyControlled())
		{
			bContainsLocalPlayer = true;
			GetOwner()->SetActorHiddenInGame(false);
			Cast<APotProbPlayerController>(PlayerController)->ChangeRoomNoise(RoomName);

			// TODO: Replace auto?
			for (auto It = PlayersWithin.CreateIterator(); It; ++It)
			{
				PlayerController->HiddenActors.Remove(It->Get());
				It->Get()->RevealPlayerComponents();
			}
		}
		else
		{
			if (bContainsLocalPlayer)
			{
				PlayerController->HiddenActors.Remove(Character);
				Character->RevealPlayerComponents();
			}
			PlayersWithin.Add(Character);
		}
	}
}

void UROVComponent::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(OtherActor))
	{
		if (Character->IsLocallyControlled())
		{
			bContainsLocalPlayer = false;
			GetOwner()->SetActorHiddenInGame(true);

			if (Character->GetIsClarivoyant())
			{
				return;
			}
			
			// TODO: Replace auto?
			for (auto It = PlayersWithin.CreateIterator(); It; ++It)
			{
				PlayerController->HiddenActors.Add(It->Get());
				It->Get()->HidePlayerComponents();
			}
		}
		else
		{
			if (bContainsLocalPlayer)
			{
				PlayerController->HiddenActors.Add(Character);
				Character->HidePlayerComponents();
			}
			PlayersWithin.Remove(Character);
		}
	}
}
