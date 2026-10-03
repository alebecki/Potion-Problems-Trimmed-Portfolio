// Fill out your copyright notice in the Description page of Project Settings.


#include "Mirror.h"

#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"

// Sets default values
AMirror::AMirror()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	UBoxComponent* BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);

	UPaperSpriteComponent* Sprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	Sprite->SetupAttachment(RootComponent);
	bReplicates = true;

}

// Called when the game starts or when spawned
void AMirror::BeginPlay()
{
	Super::BeginPlay();
	
}

