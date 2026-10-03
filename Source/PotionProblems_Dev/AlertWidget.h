// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PotProbGameMode.h"
#include "PotProbEnums.h"
#include "GameFramework/PlayerState.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.h"
#include "AlertWidget.generated.h"


class UProgressBar;
class UScrollBox;
class UVerticalBox;
class UHorizontalBox;
class UTextBlock;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UAlertWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // Initialize the alert with a message and duration
    UFUNCTION(BlueprintCallable, Category = "Alert")
    void InitializeAlert(const FString& Message, float Duration);

    // Destroy and clean up
    void ClearAlertWidget();

protected:
    virtual void NativeConstruct() override;

    // Bindings for UI components
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* AlertMessage;

private:
    // Timer handle for countdown
    FTimerHandle TimerHandle;
    FTimerHandle AlertTimerHandle;

    // Total duration for the alert
    float TotalDuration;

    // Remaining time for the alert
    float RemainingTime;

    // Updates the progress bar and timer
    void UpdateTimer();

    // Clean up the widget when the timer ends
    void OnTimerEnd();
};