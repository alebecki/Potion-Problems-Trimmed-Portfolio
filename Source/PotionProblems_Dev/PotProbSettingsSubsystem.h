// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PotProbSettingsSubsystem.generated.h"

class UPotProbSettingsSaveGame;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UPotProbSettingsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	void LoadSettings();
	void SaveSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	UPotProbSettingsSaveGame* GetCurrentSettings() const { return CurrentSettings; }

private:
	UPROPERTY()
	UPotProbSettingsSaveGame* CurrentSettings;
};
