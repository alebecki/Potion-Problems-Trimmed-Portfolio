// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"
#include "CauldronActor.generated.h"

class UWidgetComponent;
class UTextRenderComponent;
class UPotionObject;
class APotProbPlayerState;

UCLASS()
class POTIONPROBLEMS_DEV_API ACauldronActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACauldronActor();

	UFUNCTION(BlueprintCallable)
	void AddItemToCauldron(APawn* InstigatorPawn);
	UFUNCTION(BlueprintCallable)
	void TryGeneratePotion(APawn* InstigatorPawn);
	int32 GetNumberIngredientsInCauldron() { return CauldronIngredients.Num(); }
    bool CanMakeFrogPotion(APotProbPlayerState* PlayerState);
    UPROPERTY(Replicated, BlueprintReadOnly)
    TArray<FName> FrogRecipeIngredients;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int FindNumOccurancesOfIngredients(FName IngredientName, TArray<FName>& Ingredients);
	
	UFUNCTION(Server, Reliable)
	void Server_CheckWin();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* CauldronSprite;
	
	UFUNCTION()
	void OnRep_CauldronIngredients();
	
	UPROPERTY(ReplicatedUsing=OnRep_CauldronIngredients, BlueprintReadWrite, Category = "Names")
	TArray<FName> CauldronIngredients;
	
	UPROPERTY(EditDefaultsOnly)
	float Radius = 3.0f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPotionObject> UnstablePotionClass = nullptr;
};
