// Fill out your copyright notice in the Description page of Project Settings.


#include "PotionCraftedActor.h"
#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"
#include "PotionObject.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// Sets default values
APotionCraftedActor::APotionCraftedActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	BoxComp->bHiddenInGame = true;
	BoxComp->SetIsReplicated(true);

	PotionIconSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("PotionIconSprite"));
	PotionIconSprite->SetupAttachment(RootComponent);
	PotionIconSprite->SetIsReplicated(true);

	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->OnBeginCursorOver.AddDynamic(this, &APotionCraftedActor::OnMouseOverBegin);
	BoxComp->OnEndCursorOver.AddDynamic(this, &APotionCraftedActor::OnMouseOverEnd);

}

void APotionCraftedActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PotionClass);
	DOREPLIFETIME(ThisClass, b_IsCrafted);
	DOREPLIFETIME(ThisClass, BaseRecipe);
}

void APotionCraftedActor::Server_SetPotionCrafted_Implementation()
{
	b_IsCrafted = true;

	if (GetLocalRole() == ROLE_Authority)
	{
		OnRep_IsCrafted();
	}
}

// Called when the game starts or when spawned
void APotionCraftedActor::BeginPlay()
{
	Super::BeginPlay();
	
	APotProbGameMode* PotionGameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode());
	if (!PotionGameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("We are not on the server. Caution"));
		return;
	}

	if (!PotionClass) 
	{
		return;
	}

	TArray<FRecipeStruct> Recipes = PotionGameMode->GetPotionRecipes();
	for (const FRecipeStruct& Recipe : Recipes)
	{
		if (Recipe.Potion == PotionClass) {
			BaseRecipe = Recipe;
		}
	}

	TArray<AActor*> CraftedRecipeDisplayActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACraftedRecipeDisplayActor::StaticClass(), CraftedRecipeDisplayActors);

	for (AActor* CraftedDisplay : CraftedRecipeDisplayActors) {
		ACraftedRecipeDisplayActor* CraftedRecipeDisplay = Cast<ACraftedRecipeDisplayActor>(CraftedDisplay);
		if (CraftedRecipeDisplay) {
			RecipeDisplay = CraftedRecipeDisplay;
		}

	}
	//Server_SetPotionCrafted();
}

void APotionCraftedActor::OnRep_IsCrafted()
{
	PotionIconSprite->SetSprite(PotionClass.GetDefaultObject()->GetPotionSprite());
}

void APotionCraftedActor::OnMouseOverBegin(UPrimitiveComponent* TouchedComponent)
{
	if (!RecipeDisplay) {
		TArray<AActor*> CraftedRecipeDisplayActors;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACraftedRecipeDisplayActor::StaticClass(), CraftedRecipeDisplayActors);

		for (AActor* CraftedDisplay : CraftedRecipeDisplayActors) {
			ACraftedRecipeDisplayActor* CraftedRecipeDisplay = Cast<ACraftedRecipeDisplayActor>(CraftedDisplay);
			if (CraftedRecipeDisplay) {
				RecipeDisplay = CraftedRecipeDisplay;
			}

		}
	}

	if (b_IsCrafted) {
		RecipeDisplay->DisplayRecipe(BaseRecipe);
	}
}

void APotionCraftedActor::OnMouseOverEnd(UPrimitiveComponent* TouchedComponent)
{
	if (b_IsCrafted) {
		RecipeDisplay->ClearRecipe();
	}
}

