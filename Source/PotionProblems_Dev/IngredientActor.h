// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"
#include "InteractComponent.h"
#include "IngredientData.h"
#include "IngredientActor.generated.h"


UCLASS()
class POTIONPROBLEMS_DEV_API AIngredientActor : public AActor
{
	GENERATED_BODY()
	
public:
	AIngredientActor();
	
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Sets default values for this actor's properties
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* IngredientSprite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingredient", meta = (GetOptions = "GetNameOptions"))
	FName IngredientName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* IngredientDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UInteractComponent* InteractComponent;
	
public:	
	UFUNCTION(BlueprintCallable)
	void GiveIngredientToInstigator(APawn* InstigatingPawn);
};
