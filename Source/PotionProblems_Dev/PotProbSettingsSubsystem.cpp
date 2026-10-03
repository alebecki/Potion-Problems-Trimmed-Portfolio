// Fill out your copyright notice in the Description page of Project Settings.


#include "PotProbSettingsSubsystem.h"
#include "PotProbSettingsSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "AkAudioDevice.h"
#include "GameFramework/GameUserSettings.h"

void UPotProbSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadSettings();
}

void UPotProbSettingsSubsystem::Deinitialize()
{
	SaveSettings();
	Super::Deinitialize();
}

void UPotProbSettingsSubsystem::LoadSettings()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("SettingsSaveSlot"), 0))
	{
		CurrentSettings = Cast<UPotProbSettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSaveSlot"), 0));
	}
	else
	{
		CurrentSettings = Cast<UPotProbSettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(UPotProbSettingsSaveGame::StaticClass()));
		SaveSettings();
	}
}

void UPotProbSettingsSubsystem::SaveSettings()
{
	if (CurrentSettings)
	{
		UGameplayStatics::SaveGameToSlot(CurrentSettings, CurrentSettings->SaveSlotName, CurrentSettings->UserIndex);
	}
}




