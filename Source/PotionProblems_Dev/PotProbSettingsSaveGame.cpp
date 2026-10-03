#include "PotProbSettingsSaveGame.h"

UPotProbSettingsSaveGame::UPotProbSettingsSaveGame()
{
	SaveSlotName = TEXT("SettingsSaveSlot");
	UserIndex = 0;
	
	DisplayTutorial = true; 
	MasterVolume = 100.0f; 
	MusicVolume = 100.0f;
	SFXVolume = 100.0f;
}
