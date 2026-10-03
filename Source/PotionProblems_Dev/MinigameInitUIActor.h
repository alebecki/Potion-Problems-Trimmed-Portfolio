// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IngredientData.h"
#include "GameFramework/Actor.h"
#include "MinigameInitUIActor.generated.h"

class UBoxComponent;
class UPaperSpriteComponent;

/** DEPRECATED: ONLY FOR HUD BASED MINIGAMES */ 

UCLASS()
class POTIONPROBLEMS_DEV_API AMinigameInitUIActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMinigameInitUIActor();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> BoxComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperSpriteComponent> MinigameSprite;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Widget")
	TSubclassOf<UUserWidget> MinigameWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingredient", meta = (GetOptions = "GetNameOptions"))
	FName IngredientName;

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
};
