// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ObservatoryScreenWidget.generated.h"

class UCanvasPanelSlot;


/**
 * FOR WORLD SPACE MINIGAMES
 *
 * Observatory sky widget that moves the star
 */

UCLASS()
class POTIONPROBLEMS_DEV_API UObservatoryScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUserWidget> StarWidget;

	TObjectPtr<UCanvasPanelSlot> StarSlot;

	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
	void UpdateStarLocation(FVector2D NewLocation) const;
};
