// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PotionActor.generated.h"

class UInteractComponent;
class UBoxComponent;
class UPaperSpriteComponent;
class UPotionObject;
UCLASS()
class POTIONPROBLEMS_DEV_API APotionActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APotionActor();
	void SetPotionClass(TSubclassOf<UPotionObject> InputPotionClass);
	
	UFUNCTION(BlueprintCallable)
	void TryGivePlayerPotion(APawn* InstigatorPawn);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	void BeginPlay() override;
    UFUNCTION()
    void OnRep_PotionClass();
	// The Type of Potion that this Actor will give to the character that interacts with it.
	UPROPERTY(EditAnywhere, Category = "Designer Variables", ReplicatedUsing=OnRep_PotionClass)
	TSubclassOf<UPotionObject> PotionClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* PotionIconSprite;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UInteractComponent* InteractComponent;
};
