// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldSpaceWidgetActor.generated.h"

class UWidgetComponent;

UCLASS()
class POTIONPROBLEMS_DEV_API AWorldSpaceWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:
	AWorldSpaceWidgetActor();
	
	UPROPERTY(Replicated, VisibleAnywhere)
	UWidgetComponent* WidgetComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Widget")
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Widget")
	FVector2D DrawSize = {1000.0f, 500.0f };
	UFUNCTION()
	void SetDrawSize(float Width, float Height);
};
