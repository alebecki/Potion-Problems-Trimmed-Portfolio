// Fill out your copyright notice in the Description page of Project Settings.


#include "CauldronActor.h"

#include "CauldronIngWidget.h"
#include "HUDWidget.h"
#include "InteractComponent.h"
#include "InteractSubsystem.h"
#include "PotProbZDCharacter.h"
#include "PotProbGameState.h"
#include "PotionActor.h"
#include "PotionCraftedActor.h"
#include "PotionObject.h"
#include "Kismet/GameplayStatics.h"
#include "PotProbPlayerController.h"
#include "PotProbPlayerState.h"
#include "AI/NavigationSystemBase.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
class APotProbPlayerController;

// Sets default values
ACauldronActor::ACauldronActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	
	bReplicates = true;
}

void ACauldronActor::AddItemToCauldron(APawn* InstigatorPawn)
{
	//Check if the Player Character, Player State, and the Game Mode are valid
	const TObjectPtr<APotProbZDCharacter> PlayerCharacter = Cast<APotProbZDCharacter>(InstigatorPawn);
	const TObjectPtr<APotProbPlayerState> PlayerState = Cast<APotProbPlayerState>(PlayerCharacter->GetPlayerState());
	APotProbGameMode* PotionGameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode());
	if (!IsValid(PlayerCharacter))
	{
		return;
	}
	if (!IsValid(PlayerState))
	{
		return;
	}
	if (!PotionGameMode)
	{
		return;
	}

	//If you are not holding an ingredient then return
	if (PlayerState->GetIngredientName() == NAME_None || !PlayerState->GetIsHoldingIngredient())
	{
		return;
	}

	// If you are an apprentice and the cauldron already has two ingredients, return
	if (PlayerState->PotProbRole == EPotProbRoles::ROLE_APPRENTICE && CauldronIngredients.Num() >= 2)
	{
		PlayerState->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "This cauldron is already full!");
		return;
	}

	// If you are an troublemaker and the cauldron already has three ingredients, return
	if (PlayerState->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER && CauldronIngredients.Num() >= 3)
	{
		PlayerState->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "This cauldron is already full!");
		return;
	}

	// If you're trying to add a duplicate ingredient
	int FoundIndex = CauldronIngredients.Find(PlayerState->GetIngredientName());
	if (CauldronIngredients.Num() > 0 && FoundIndex != -1)
	{
		PlayerState->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "You can't brew a potion \nwith 2 of the same ingredient!");
		return;
	}
	
	//If the server is not the one adding the ingredient
	if (GetLocalRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to add item to cauldron when not on server. Caution"));
		return;
	}
	// Add ingredient to cauldron
	CauldronIngredients.Add(PlayerState->GetIngredientName());
	OnRep_CauldronIngredients();
	//update the hud and remove the ingredient from the instigator
	if (APotProbPlayerController* PotionController = Cast<APotProbPlayerController>(PlayerCharacter->GetController()))
	{
		PotionController->Server_ChangeIngredientStatus(NAME_None, false);
		if (PotionController->HUDWidgetInstance)
		{
			PotionController->HUDWidgetInstance->DisplayDropIngredientText(false);
		}
	}
	
	// tutorial alert
	if (!PlayerState->bFirstIngredientAdded)
	{
	    PlayerState->ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Good job!\nFind more ingredients!");
	    PlayerState->bFirstIngredientAdded = true;
	}
}

void ACauldronActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACauldronActor, CauldronIngredients);
	DOREPLIFETIME(ACauldronActor, FrogRecipeIngredients);
}

// Called when the game starts or when spawned
void ACauldronActor::BeginPlay()
{
	Super::BeginPlay();

	APotProbGameMode* GM = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->AddCauldrons(this);
	}
}

int ACauldronActor::FindNumOccurancesOfIngredients(FName IngredientName, TArray<FName>& Ingredients)
{
	//returns the number of times an ingredients name appears in an array of ingredients
	int counter = 0;
	for (FName EnteredIngredientName : Ingredients)
	{
		if (EnteredIngredientName == IngredientName)
		{
			counter++;
		}
	}
	return counter;
}

void ACauldronActor::TryGeneratePotion(APawn* InstigatorPawn)
{
	//if we are not on the server, return and do not add ingredient to cauldron
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to create a potion when not on server. Caution"));
		return;
	}
	
	//if the server game mode is not valid, return and do not add ingredient to cauldron
	APotProbGameMode* PotionGameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode());
	if (!PotionGameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("We are not on the server. Caution"));
		return;
	}

	// get if the player is a troublemaker
	APotProbPlayerState* PlayerState = InstigatorPawn->GetPlayerState<APotProbPlayerState>();
	bool bIsPlayerTroublemaker = PlayerState->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER;
	
	// Check if player is already holding a Potion
    if (PlayerState->GetPlayerPotion() != nullptr)
    {
        PlayerState->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "You can't hold any more potions!");
        
        UE_LOG(LogTemp, Warning,
        TEXT("We tried to craft a potion, but are already holding a potion"));
        return;
    }
    
    // Check if too few ingredients
    if (CauldronIngredients.Num() <= 1)
    {
        UE_LOG(LogTemp, Warning,
        TEXT("We tried to craft a potion, but there were to few ingredients to craft one"));
        return;
    }

    // Check if the player is an apprentice and trying to generate a potion with 3 or more ingredients
    if (!bIsPlayerTroublemaker && CauldronIngredients.Num() >= 3)
    {
        PlayerState->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "This potion looks suspicious. \nYou probably shouldn't bottle it...");

        UE_LOG(LogTemp, Warning,
        TEXT("We tried to craft a potion, but are an apprentice trying to generate a frog potion"));
        return;
    }
    
    // get the recipes
	FRecipeStruct FoundPotionStruct;
    TSubclassOf<UPotionObject> PotionType;

    TArray<FRecipeStruct> Recipes = PotionGameMode->GetPotionRecipes();
    FRecipeStruct FrogRecipe = Recipes[0];
    Recipes.RemoveAt(0);

    // Sort the IngredientNames
    CauldronIngredients.Sort(APotProbGameMode::CustomRecipeStructSort);
	
	bool bHasFoundRightRecipe = false;
	
	// Check for frog potion
	// Needs to be UNFROGGED TROUBLEMAKER
	if (bIsPlayerTroublemaker && !PlayerState->bIsFrogged)
	{
		// Number matches
		if (FrogRecipe.Ingredients.Num() == CauldronIngredients.Num())
		{
			bool bMatch = true;
			for (int i = 0; i < FrogRecipe.Ingredients.Num(); ++i)
			{
				if (CauldronIngredients[i] != FrogRecipe.Ingredients[i])
				{
					bMatch = false;
					break;
				}
			}

			if (bMatch)
			{
				// Found a Recipe
				bHasFoundRightRecipe = true;
				PotionType = FrogRecipe.Potion;
			}
		}
	}
	 // Check all other potions
	if (!bHasFoundRightRecipe)
	{
		for (FRecipeStruct Recipe : Recipes)
		{
			// Number matches
			if (Recipe.Ingredients.Num() == CauldronIngredients.Num())
			{
				bool bMatch = true;
				for (int i = 0; i < Recipe.Ingredients.Num(); ++i)
				{
					if (CauldronIngredients[i] != Recipe.Ingredients[i])
					{
						bMatch = false;
						break;
					}
				}

				if (bMatch)
				{
					// Found a Recipe
					bHasFoundRightRecipe = true;
					PotionType = Recipe.Potion;
					break;
				}
			}
		}
	}
	
	// Check if they crafted an Unstable Potion
	if (!bHasFoundRightRecipe && CauldronIngredients.Num() > 1)
	{
		UE_LOG(LogTemp, Warning,
			   TEXT("Could not find any correct recipe for this sequence of ingredients. Generating Random Potion"));
		PotionType = UnstablePotionClass;
	}
	
	//if an apprentice crafts a non-unstable potion, update the win con
	if(!bIsPlayerTroublemaker && PotionType != UnstablePotionClass)
	{
		// Just call the server version so that it always runs on the server.
		PotionGameMode->AddRecipeToCurrentlyCraftedApprentice(FoundPotionStruct);
		PlayerState->NumPotionsCrafted++;
		Server_CheckWin();
	}
		
	//give the player the crafted potion
	PlayerState->ServerReceivePotion(PotionType);
		
	//empty the cauldron
	CauldronIngredients.Empty();
	OnRep_CauldronIngredients();
}

bool ACauldronActor::CanMakeFrogPotion(APotProbPlayerState* PlayerState)
{
    bool bIsPlayerTroublemaker = PlayerState->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER;
	
    if (bIsPlayerTroublemaker && !PlayerState->bIsFrogged)
	{
		if (FrogRecipeIngredients.Num() > 0 && FrogRecipeIngredients.Num() == CauldronIngredients.Num())
		{
		    CauldronIngredients.Sort(APotProbGameMode::CustomRecipeStructSort);
			for (int i = 0; i < FrogRecipeIngredients.Num(); ++i)
			{
				if (CauldronIngredients[i] != FrogRecipeIngredients[i])
				{
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

void ACauldronActor::OnRep_CauldronIngredients()
{
	UWidgetComponent* RenderedUI = GetComponentByClass<UWidgetComponent>();
	if (!RenderedUI)
	{
		return;
	}

	// Set the widget to be visible to all players
	//RenderedUI->SetWidgetSpace(EWidgetSpace::World);
	//RenderedUI->SetDrawAtDesiredSize(true);
	//RenderedUI->bOnlyOwnerSee = false;

	UCauldronIngWidget* IngWidget = Cast<UCauldronIngWidget>(RenderedUI->GetWidget());
	if (!IngWidget)
	{
		return;
	}
	FString DisplayText = TEXT("Ingredients: ") + FString::FromInt(CauldronIngredients.Num());
	if (CauldronIngredients.Num() == 0)
	{
		DisplayText = TEXT("Empty Cauldron");
	}
	
	IngWidget->SetIngredientCountText(DisplayText);

	IngWidget->UpdateIngredientMarkers(CauldronIngredients);
	IngWidget->SetCauldronActor(this);
}


void ACauldronActor::Server_CheckWin_Implementation()
{
	APotProbGameMode* GameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->CheckWin();
	}
}