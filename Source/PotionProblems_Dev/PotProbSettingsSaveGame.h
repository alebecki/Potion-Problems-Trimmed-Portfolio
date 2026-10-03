// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PotProbSettingsSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UPotProbSettingsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	
	UPotProbSettingsSaveGame();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Display")
	bool DisplayTutorial;

	// Audio volume settings (0-1 range)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	float MasterVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	float MusicVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	float SFXVolume;
	
	// Save slot
	UPROPERTY(VisibleAnywhere, Category = "Settings")
	FString SaveSlotName;

	// User index
	UPROPERTY(VisibleAnywhere, Category = "Settings")
	uint32 UserIndex;
};
