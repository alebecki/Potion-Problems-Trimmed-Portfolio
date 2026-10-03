// Fill out your copyright notice in the Description page of Project Settings.


#include "IngredientActor.h"

#include "HUDWidget.h"
#include "PotProbPlayerController.h"
#include "PotProbPlayerState.h"
#include "PotProbZDCharacter.h"

// Sets default values
AIngredientActor::AIngredientActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);

	IngredientSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("IngredientSprite"));
	IngredientSprite->SetupAttachment(RootComponent);
	InteractComponent = CreateDefaultSubobject<UInteractComponent>(TEXT("InteractComponent"));
}


void AIngredientActor::BeginPlay()
{
	Super::BeginPlay();
	/*if (IngredientDataTable)
	{

		TArray<FIngredients*> Rows;

		static const FString ContextString(TEXT("Ingredient Data Context"));

		IngredientDataTable->GetAllRows(ContextString, Rows);

		for (const FIngredients* Row : Rows)
		{
			if (Row) {
				if (Row->Ingredient == IngredientName)
				{
					IngredientSprite->SetSprite(Row->IngredientSprite);
				}
			}
		}
	}*/
}

void AIngredientActor::GiveIngredientToInstigator(APawn* InstigatingPawn)
{
	APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(InstigatingPawn);

	if(!PlayerCharacter)
	{
		return;
	}
	APotProbPlayerState* PotionPlayerState = Cast<APotProbPlayerState>(PlayerCharacter->GetPlayerState());
	if(!PotionPlayerState)
	{
		return;
	}
	if(GetLocalRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to give ingredient to Instigator when not on server. Caution."));
		return;
	}
	if(PotionPlayerState->GetIsHoldingIngredient())
	{
		UE_LOG(LogTemp, Log, TEXT("Player is already holding one ingredient. We cannot let them hold another."));
		return;
	}

	// tutorial alert
	if (!PotionPlayerState->HasPickedUpFirstIngredient())
	{
		PotionPlayerState->ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Bring your ingredient to any cauldron\nto begin brewing a potion!");
	}
	
	if (APotProbPlayerController* PotionController = Cast<APotProbPlayerController>(PlayerCharacter->GetController()))
	{
		PotionController->Server_ChangeIngredientStatus(IngredientName, true);
	}
}
