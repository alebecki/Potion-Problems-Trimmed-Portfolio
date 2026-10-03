// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "RecipeWidget.h"
#include "CraftedRecipeDisplayActor.generated.h"

UCLASS()
class POTIONPROBLEMS_DEV_API ACraftedRecipeDisplayActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACraftedRecipeDisplayActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void DisplayRecipe(FRecipeStruct& MousedRecipeStruct);

	void ClearRecipe();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UWidgetComponent* RecipeWidgetComponent;

	UPROPERTY(Replicated)
	URecipeWidget* RecipeWidget;

};
