// Fill out your copyright notice in the Description page of Project Settings.


#include "ChillyBoostTrigger.h"

#include "PotProbZDCharacter.h"
#include "Components/ShapeComponent.h"

void AChillyBoostTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AChillyBoostTrigger::HandleBeginOverlap);
	//GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &AChillyBoostTrigger::HandleEndOverlap);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AChillyBoostTrigger::HandleAutoDelete, AutoDeleteTime, false);
}

void AChillyBoostTrigger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetCollisionComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &AChillyBoostTrigger::HandleBeginOverlap);
	//GetCollisionComponent()->OnComponentEndOverlap.RemoveDynamic(this, &AChillyBoostTrigger::HandleEndOverlap);
}

void AChillyBoostTrigger::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(OtherActor))
	{
		if (Character->IsLocallyControlled() && Character != GetInstigator())
		{
			Character->ActivateChillyMiniBoost();
		}
	}
}

void AChillyBoostTrigger::HandleAutoDelete()
{
	Destroy();
}

