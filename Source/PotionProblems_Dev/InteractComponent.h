// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractWidget.h"
#include "Components/ActorComponent.h"
#include "InteractComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractDelegate, APawn*, InstigatorPawn);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POTIONPROBLEMS_DEV_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractComponent();
	UPROPERTY(BlueprintAssignable)
	FOnInteractDelegate OnInteract;
	UPROPERTY(BlueprintAssignable)
	FOnInteractDelegate OnInteractHeld;
	UPROPERTY(BlueprintAssignable)
	FOnInteractDelegate OnInteractSuperHeld;
	void DisplayInteractText(bool bShouldDisplay);
	bool GetIsDisplayingInteractText() const { return bIsDisplayingInteractText; }
protected:
	FString DetermineTextToDisplay();
	bool bIsDisplayingInteractText = false;
	bool bInitBottlePotionAlert = false;
	bool bFrogDoorAlert = false;
	TObjectPtr<UInteractWidget> InteractableWidget;
	AActor* HighlightedActor = nullptr;
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void NativeInteract(APawn* InstigatorPawn);
	virtual void NativeInteractHeld(APawn* InstigatorPawn);
	virtual void NativeInteractSuperHeld(APawn* InstigatorPawn);
	// virtual void NativeInteractSuperHeld(APawn* InstigatorPawn);
	
};
