// Fill out your copyright notice in the Description page of Project Settings.


#include "ObservatoryWorldActor.h"

#include "Components/SizeBox.h"
#include "ObservatoryScreenWidget.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"

AObservatoryWorldActor::AObservatoryWorldActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// set boundaries of UI widget in world
	SetDrawSize(SkyWidth, SkyHeight);

	ObservatoryWidget = Cast<UObservatoryScreenWidget>(WidgetComponent->GetUserWidgetObject());
	
	ResetStar();
}

void AObservatoryWorldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// move star position on server
	if (FVector2D::Distance(StarLocation, TargetLocation) > ResetDistance)
	{
		FVector2D MoveDirection = TargetLocation - StarLocation;
		MoveDirection.Normalize();
		StarLocation = StarLocation + MoveDirection * StarMoveSpeed * DeltaTime;
		
		if (HasAuthority())
		{
			OnRep_StarLocation();
		}
	}
	else
	{
		ResetStar();
	}
}

void AObservatoryWorldActor::ResetStar_Implementation()
{
	// generate random reset location along screen edge
	FVector2D Size(SkyWidth, SkyHeight);
	
	TargetLocation.X = Size.X / 2.0f + ResetBuffer;
	TargetLocation.Y = FMath::FRandRange(0, Size.Y) - Size.Y / 2.0f;

	StarLocation = FVector2D::ZeroVector;
	StarLocation.X = - (Size.X / 2.0f) - ResetBuffer;
	StarLocation.Y = FMath::FRandRange(0, Size.Y)  - Size.Y / 2.0f;
	
	if (HasAuthority())
	{
		OnRep_StarLocation();
	}
}

void AObservatoryWorldActor::OnRep_StarLocation()
{
	if (!ObservatoryWidget)
	{
		ObservatoryWidget = Cast<UObservatoryScreenWidget>(WidgetComponent->GetUserWidgetObject());
		return;
	}

	ObservatoryWidget->UpdateStarLocation(StarLocation);
}

void AObservatoryWorldActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AObservatoryWorldActor, StarLocation);
}