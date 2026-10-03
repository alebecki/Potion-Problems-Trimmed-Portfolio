// Fill out your copyright notice in the Description page of Project Settings.

#include "MinigameInitUIActor.h"
#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"

// Sets default values
AMinigameInitUIActor::AMinigameInitUIActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);

	MinigameSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MinigameSprite"));
	MinigameSprite->SetupAttachment(RootComponent);
}
