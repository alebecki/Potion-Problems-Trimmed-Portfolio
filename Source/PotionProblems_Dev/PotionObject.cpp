// Fill out your copyright notice in the Description page of Project Settings.


#include "PotionObject.h"
#include "Net/UnrealNetwork.h"
#include "PotProbPlayerState.h"

UPotionObject::UPotionObject()
{
}

void UPotionObject::ActivatePotionForCaller_Implementation(APlayerState* CallerPlayerState)
{
}

void UPotionObject::ActivatePotionForTarget_Implementation(APlayerState* CallerPlayerState, APlayerState* TargetPlayerState)
{
}

void UPotionObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPotionObject, PotionIconSprite);
	DOREPLIFETIME(UPotionObject, PotionName);
	DOREPLIFETIME(UPotionObject, PotionDescription);
	DOREPLIFETIME(UPotionObject, bCanSplash);
	DOREPLIFETIME(UPotionObject, PotionChargeType);
	DOREPLIFETIME(UPotionObject, NumChargesToGive);
}
