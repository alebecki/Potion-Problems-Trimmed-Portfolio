// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldSpaceWidgetActor.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

AWorldSpaceWidgetActor::AWorldSpaceWidgetActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates =  true;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetWidgetClass(WidgetClass); // Set your widget class
	WidgetComponent->SetDrawSize(DrawSize); // Adjust size as needed
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World); // Ensure it's set to World space
	RootComponent = WidgetComponent; // Set as root
	WidgetComponent->SetIsReplicated(true);
}

void AWorldSpaceWidgetActor::SetDrawSize(float Width, float Height)
{
	DrawSize.X = Width;
	DrawSize.Y = Height;
	WidgetComponent->SetDrawSize(DrawSize);
}

void AWorldSpaceWidgetActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWorldSpaceWidgetActor, WidgetComponent);
}
