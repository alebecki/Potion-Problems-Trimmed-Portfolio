// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputAction.h"
#include "SpyglassMinigameWidget.generated.h"

class UCanvasPanelSlot;
class AWorldSpaceWidgetActor;
class UInputMappingContext;
class AObservatoryWorldActor;
class AMinigameInitActor;

/**
 * FOR WORLD SPACE MINIGAMES
 *
 * Telescope reticle that players move to catch the star
 */

UCLASS()
class POTIONPROBLEMS_DEV_API USpyglassMinigameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	/** ACTORS AND WIDGETS */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUserWidget> SpyGlassWidget;
	TObjectPtr<UCanvasPanelSlot> SpyGlassSlot;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AObservatoryWorldActor> ObservatoryWorldActor;
	FVector2D CenterLocation = FVector2D::ZeroVector;
	float ObservatoryWidth = 0.0f;
	float ObservatoryHeight = 0.0f;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> OwningWorldActor;
	UPROPERTY(BlueprintReadWrite)
	AMinigameInitActor* ParentInitActor;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** INPUT BINDINGS */
	// Spyglass mapping context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> MinigameMappingContext;
	// Player movement mapping context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> PlayerGameMappingContext;
	UFUNCTION()
	void RemoveInputBindings();
	UFUNCTION()
	void SetupInputBindings();
    
protected:
	
	/** SPYGLASS MOVEMENT */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InputMoveSpeed = 30.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StarFindDistance = 40.0f;
	// WASD movement of spy glass
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	void CheckStarFound();
	void OnMove(const FInputActionInstance& ActionInstance);
	
};
