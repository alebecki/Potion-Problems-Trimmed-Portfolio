// Fill out your copyright notice in the Description page of Project Settings.


#include "MinigameInitActor.h"

#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "MinigameIngWidget.h"

// Sets default values
AMinigameInitActor::AMinigameInitActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	CanInteract = true;
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);

	MinigameSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MinigameSprite"));
	MinigameSprite->SetupAttachment(RootComponent);
	MinigameSprite->SetIsReplicated(true);
}

void AMinigameInitActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMinigameInitActor::DeactivateTelescope_Implementation()
{
    if (MinigameSprite)
    {
        MinigameSprite->SetSpriteColor(DeactivatedColor);
    }

	// deactivate telescope interact widget
	CanInteract = false;
	TArray<UActorComponent*> Components = GetComponentsByTag(UWidgetComponent::StaticClass(), "Interact");

	// There should only be one interact WidgetComponent
	if(Components.Num() != 1)
	{
		return;
	}
	UWidgetComponent* WidgetComp = Cast<UWidgetComponent>(Components[0]);
	if(WidgetComp)
	{
		WidgetComp->SetVisibility(false);
	}
}

void AMinigameInitActor::SetRandomIngredient()
{
	TArray<FName> NameOptions = GetNameOptions();
	
	if (NameOptions.IsEmpty())
	{
		UE_LOG(LogTemp, Display, TEXT("Ingredient Name Options is Empty."));
		return;
	}
	
	int RandIngIdx = rand() % NameOptions.Num();
	IngredientName = NameOptions[RandIngIdx];
	
	if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer)
	{
		OnRep_IngredientName();
	}
}

void AMinigameInitActor::OnRep_IngredientName()
{
    // update ingredient widget component
    TArray<UActorComponent*> Components = GetComponentsByTag(UWidgetComponent::StaticClass(), "Ingredient");
    
    // There should only be one ingredient WidgetComponent
    if(Components.Num() != 1)
    {
    	return;
    }
    
    UWidgetComponent* WidgetComp = Cast<UWidgetComponent>(Components[0]);
    if(!WidgetComp)
    {
    	return;
    }
    
    UMinigameIngWidget* IngWidget = Cast<UMinigameIngWidget>(WidgetComp->GetWidget());
    
    if (!IngWidget)
    {
    	return;
    }
    
    IngWidget->SetInitActor(this);
    IngWidget->UpdateIngredientMarker(IngredientName);
}

void AMinigameInitActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMinigameInitActor, CanInteract);
	DOREPLIFETIME(AMinigameInitActor, MinigameSprite);
	DOREPLIFETIME(AMinigameInitActor, IngredientName);
}