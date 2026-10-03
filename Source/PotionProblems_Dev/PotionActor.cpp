// Fill out your copyright notice in the Description page of Project Settings.


#include "PotionActor.h"

#include "InteractComponent.h"
#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"
#include "PotProbPlayerState.h"
#include "PotProbZDCharacter.h"
#include "PotionObject.h"
#include "Net/UnrealNetwork.h"
// Sets default values
APotionActor::APotionActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	BoxComp->bHiddenInGame = false;
	BoxComp->SetIsReplicated(true);
	
	PotionIconSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("PotionIconSprite"));
	PotionIconSprite->SetupAttachment(RootComponent);
	PotionIconSprite->SetIsReplicated(true);
	
	InteractComponent = CreateDefaultSubobject<UInteractComponent>(TEXT("InteractComponent"));
}

void APotionActor::SetPotionClass(TSubclassOf<UPotionObject> InputPotionClass)
{
	PotionClass = InputPotionClass;
	if(PotionClass != nullptr && PotionClass.GetDefaultObject()->GetPotionSprite())
	{
		PotionIconSprite->SetSprite(PotionClass.GetDefaultObject()->GetPotionSprite());
	}
}

void APotionActor::TryGivePlayerPotion(APawn* InstigatorPawn)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Error, TEXT("This is not being called on the server. This will fail"));
		return;
	}
	APotProbZDCharacter* PotionPlayerCharacter = Cast<APotProbZDCharacter>(InstigatorPawn);

	if (!PotionPlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Instigator Pawn is not of type PotionPlayerCharacter"));
		return;
	}

	APotProbPlayerState* PotionPlayerState = PotionPlayerCharacter->GetPlayerState<APotProbPlayerState>();

	if (!PotionPlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("Instigator Pawn does not have correct Player State. Either it is not possessed or it is using wrong player state."));
		return;
	}

	if (IsValid(PotionPlayerState->GetPlayerPotion()))
	{
		UE_LOG(LogTemp, Log, TEXT("Tried to give player potion when they already have one. This should fail"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Player does not have a potion yet. Sending Request to Server to recieve potion."));
		PotionPlayerState->ServerReceivePotion(PotionClass);
		Destroy();
	}
}

void APotionActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PotionClass);
}

void APotionActor::BeginPlay()
{
	Super::BeginPlay();
}

void APotionActor::OnRep_PotionClass()
{
	if(PotionClass != nullptr && PotionClass.GetDefaultObject()->GetPotionSprite())
	{
		PotionIconSprite->SetSprite(PotionClass.GetDefaultObject()->GetPotionSprite());
	}
}
