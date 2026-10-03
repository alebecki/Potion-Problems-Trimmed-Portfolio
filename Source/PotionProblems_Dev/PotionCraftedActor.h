// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PotProbGameMode.h"
#include "CraftedRecipeDisplayActor.h"

#include "PotionCraftedActor.generated.h"

class UBoxComponent;
class UPaperSpriteComponent;
class UPotionObject;
UCLASS()
class POTIONPROBLEMS_DEV_API APotionCraftedActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APotionCraftedActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_SetPotionCrafted();

	bool GetIsCrafted() { return b_IsCrafted; };

	FRecipeStruct GetRecipe() { return BaseRecipe; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_IsCrafted();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Designer Variables", Replicated)
	TSubclassOf<UPotionObject> PotionClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* PotionIconSprite;

	UPROPERTY(Replicated)
	FRecipeStruct BaseRecipe;

	UPROPERTY(ReplicatedUsing=OnRep_IsCrafted)
	bool b_IsCrafted;

	ACraftedRecipeDisplayActor* RecipeDisplay;

public:

	UFUNCTION(BlueprintCallable)
	void OnMouseOverBegin(UPrimitiveComponent* TouchedComponent);

	UFUNCTION(BlueprintCallable)
	void OnMouseOverEnd(UPrimitiveComponent* TouchedComponent);
};
