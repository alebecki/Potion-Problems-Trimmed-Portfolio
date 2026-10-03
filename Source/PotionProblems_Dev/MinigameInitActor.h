// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IngredientData.h"
#include "GameFramework/Actor.h"
#include "MinigameInitActor.generated.h"

class UBoxComponent;
class UPaperSpriteComponent;
class AWorldSpaceWidgetActor;
class UMinigameIngWidget;
class UWidgetComponent;


/**
 * FOR WORLD SPACE MINIGAMES
 *
 * Telescope actor players interact with to initiate the star minigame
 */

UCLASS()
class POTIONPROBLEMS_DEV_API AMinigameInitActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMinigameInitActor();
	virtual void BeginPlay() override;

	// observatory sky actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AWorldSpaceWidgetActor> ParentWidgetActor;
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void DeactivateTelescope();

	UFUNCTION()
	void SetRandomIngredient();
	FName GetIngredientName() const { return IngredientName; }
	bool GetCanInteract() const { return CanInteract; }

protected:

	/** TELESCOPE */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> BoxComp;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperSpriteComponent> MinigameSprite;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor DeactivatedColor;
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool CanInteract;

	/** INGREDIENT */
	UPROPERTY(ReplicatedUsing=OnRep_IngredientName, BlueprintReadWrite, EditInstanceOnly, Category = "Ingredient", meta = (GetOptions = "GetNameOptions"))
	FName IngredientName;
	UFUNCTION()
	void OnRep_IngredientName();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* IngredientDataTable;

	UFUNCTION(CallInEditor)
	TArray<FName> GetNameOptions() const
	{
		TArray<FName> NameOptions;
		static const FString ContextString(TEXT("Ingredient Data Context"));
    
		if (IngredientDataTable)
		{
			TArray<FIngredients*> Rows;
    
			IngredientDataTable->GetAllRows(ContextString, Rows);
    
			for (const FIngredients* Row : Rows)
			{
				if (Row)
				{
					NameOptions.Add(Row->Ingredient);
				}
			}
		}
		return NameOptions;
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
