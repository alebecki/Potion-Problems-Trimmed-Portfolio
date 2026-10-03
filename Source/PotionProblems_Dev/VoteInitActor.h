// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"
#include "InteractComponent.h"
#include "VoteInitActor.generated.h"


UCLASS()
class POTIONPROBLEMS_DEV_API AVoteInitActor : public AActor
{
	GENERATED_BODY()
	
public:
	AVoteInitActor();
	

protected:
	// Sets default values for this actor's properties
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* VoteInitSprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UInteractComponent* InteractComponent;
public:	
	UFUNCTION(BlueprintCallable)
	void InitiateVotingPhase(APawn* InstigatingPawn);

};
