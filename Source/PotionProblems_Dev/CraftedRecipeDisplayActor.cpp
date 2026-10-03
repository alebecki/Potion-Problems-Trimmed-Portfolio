// Fill out your copyright notice in the Description page of Project Settings.

#include "CraftedRecipeDisplayActor.h"
#include "RecipeWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACraftedRecipeDisplayActor::ACraftedRecipeDisplayActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	BoxComp->bHiddenInGame = true;
	BoxComp->SetIsReplicated(true);

	RecipeWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("RecipeWidgetComponent"));
	RecipeWidgetComponent->SetupAttachment(RootComponent);
	RecipeWidgetComponent->SetIsReplicated(true);
}

void ACraftedRecipeDisplayActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, RecipeWidget);
}

void ACraftedRecipeDisplayActor::DisplayRecipe(FRecipeStruct& MousedRecipeStruct)
{
	if (!RecipeWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("RecipeWidget is null. Ensure the widget class is assigned in the WidgetComponent."));
		return;
	}
	RecipeWidget->InitializeTextAndIcons(MousedRecipeStruct);
}

void ACraftedRecipeDisplayActor::ClearRecipe()
{
	if (!RecipeWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("RecipeWidget is null. Ensure the widget class is assigned in the WidgetComponent."));
		return;
	}
	RecipeWidget->RecipeImage->SetBrushFromTexture(nullptr);

	RecipeWidget->RecipeName->SetText(FText::FromString(""));
	RecipeWidget->PotionDescription->SetText(FText::FromString(""));

	RecipeWidget->InformationBox->ClearChildren();
	
}

// Called when the game starts or when spawned
void ACraftedRecipeDisplayActor::BeginPlay()
{
	Super::BeginPlay();
	
	RecipeWidget = Cast<URecipeWidget>(RecipeWidgetComponent->GetUserWidgetObject());

	if (!RecipeWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("RecipeWidget is null. Ensure the widget class is assigned in the WidgetComponent."));
		return;
	}

	RecipeWidget->RecipeButton->OnClicked.RemoveAll(RecipeWidget);
	RecipeWidget->RecipeName->SetText(FText::FromString(""));
	RecipeWidget->PotionDescription->SetText(FText::FromString(""));
}

