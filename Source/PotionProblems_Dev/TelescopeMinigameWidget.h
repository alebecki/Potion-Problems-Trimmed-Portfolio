// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputAction.h"
#include "TelescopeMinigameWidget.generated.h"

class UInputMappingContext;
class UCanvasPanelSlot;


/** DEPRECATED: ONLY FOR HUD BASED MINIGAMES */ 

UCLASS()
class POTIONPROBLEMS_DEV_API UTelescopeMinigameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UUserWidget> SpyGlassWidget;
    
    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UUserWidget> StarWidget;

	TObjectPtr<UCanvasPanelSlot> SpyGlassSlot;

	TObjectPtr<UCanvasPanelSlot> StarSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	float InputMoveSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Star")
	float StarFindDistance;
    
    virtual void NativeConstruct() override;
	
    virtual void NativeDestruct() override;
    
    /** MappingContext */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> MinigameMappingContext;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> PlayerGameMappingContext;
    
    // WASD movement of spy glass
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;
    
protected:
    
    void OnMove(const FInputActionInstance& ActionInstance);
    
    void SetupInputBindings();

	void CheckStarFound();

	void StarFound();
};
