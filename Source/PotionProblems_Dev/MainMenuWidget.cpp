// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

#include <string>

#include "CharacterCustomizeWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/CircularThrobber.h"
#include "PotProbOnlineSubsystem.h"
#include "FriendButtonWidget.h"
#include "FoundSessionWidget.h"
#include "OnlineSessionSettings.h"
#include "HUDWidget.h"
#include "PotProbSettingsSaveGame.h"
#include "PotProbSettingsSubsystem.h"
#include "TutorialWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AkGameplayStatics.h"
#include "Components/Image.h"


void UMainMenuWidget::NativeConstruct()
{

	//ryan made me do this - binging buttonanim
	CreditsButton = CreditsButtonAnim -> ButtonAnim;
	QuitButton = QuitButtonAnim -> ButtonAnim;

	JoinPrivateButton = JoinPrivateButtonAnim -> ButtonAnim;
	CreatePrivateButton = CreatePrivateButtonAnim -> ButtonAnim;

	JoinPublicButton = JoinPublicButtonAnim -> ButtonAnim;
	CreatePublicButton = CreatePublicButtonAnim -> ButtonAnim;

	LobbySelectToMainButton = LobbySelectToMainButtonAnim -> ButtonAnim;

	ConfirmSetCodeButton = ConfirmSetCodeButtonAnim -> ButtonAnim;
	SetCodeToLobbySelectButton = SetCodeToLobbySelectButtonAnim -> ButtonAnim;

	EnterCodeToLobbySelectButton = EnterCodeToLobbySelectButtonAnim -> ButtonAnim;
	ConfirmEnterCodeButton = ConfirmEnterCodeButtonAnim -> ButtonAnim;

	CreateLobbyButton = CreateLobbyButtonAnim -> ButtonAnim;
	LevelSelectToLobbySelectButton = LevelSelectToLobbySelectButtonAnim -> ButtonAnim;

	FindPublicSessionsButton = FindPublicSessionsButtonAnim -> ButtonAnim;
	TutorialToggle = TutorialToggleAnim -> ButtonAnim;
	SettingsToMainMenuButton = SettingsToMainMenuButtonAnim -> ButtonAnim;
	ControlsButton = ControlsButtonAnim -> ButtonAnim;
	
	//EnterCodeToLobbySelectButtonAnim
	//ConfirmEnterCodeButtonAnim
	//CreateLobbyButton
	//LevelSelectToLobbySelectButtonAnim
	//and then the other one
	
	Super::NativeConstruct();
	//Main Menu
	PlayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickPlayButtonAction);
	TutorialButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickTutorialButtonAction);
	CreditsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickCreditsButtonAction);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickQuitButtonAction);
	SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickSettingsButtonAction);
	//Lobby Select
	JoinPublicButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickJoinPublicButtonAction);
	CreatePublicButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickCreatePublicButtonAction);
	JoinPrivateButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickJoinPrivateButtonAction);
	CreatePrivateButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickCreatePrivateButtonAction);
	LobbySelectToMainButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickLobbySelectToMainButtonAction);
	LobbySelectSettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickSettingsButtonAction);
	//Join Public
	FindPublicSessionsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickFindAllPublicButtonAction);
	JoinPublicToLobbySelectButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickJoinPublicToLobbySelectButtonAction);
	AllPublicLobbiesToLobbySelectButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickAllPublicLobbiesToLobbySelectButtonAction);
	//Enter Code
	EnterCodeToLobbySelectButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickEnterCodeToLobbySelectButtonAction);
	ConfirmEnterCodeButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickConfirmEnterCodeButtonAction);
	SettingsButton_2->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickPlayButtonAction);
	SettingsButton_1->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickPlayButtonAction);
	EnterCodeToMainButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickLobbySelectToMainButtonAction);
	//Set Code
	SetCodeToLobbySelectButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickSetCodeToLobbySelectButtonAction);
	ConfirmSetCodeButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickConfirmSetCodeButtonAction);
	SettingsButton_3->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickPlayButtonAction);
	SetCodeToMainButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickLobbySelectToMainButtonAction);
	// Code validation
	if (EnterCodeTextBox)
	{
		EnterCodeTextBox->OnTextChanged.AddDynamic(this, &UMainMenuWidget::OnEnterCodeTextChanged);
		LastValidEnterCode = FText::GetEmpty();
	}
    
	if (SetCodeTextBox)
	{
		SetCodeTextBox->OnTextChanged.AddDynamic(this, &UMainMenuWidget::OnSetCodeTextChanged);
		LastValidSetCode = FText::GetEmpty();
	}
	//Level Select
	BigMapButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickBigMapButtonAction);
	SmallMapButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickSmallMapButtonAction);
	CreateLobbyButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickCreateLobbyButtonAction);
	LevelSelectToLobbySelectButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickLevelSelectToLobbySelectButtonAction);
	MaxPlayersSlider->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnValueChangedMaxPlayersSliderAction);
	CreateLobbyToMainMenuBubtton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickLobbySelectToMainButtonAction);
	//Legacy
	JoinFriendsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickedFindFriendsButtonAction);
	//Credits
	CreditsToMainMenuButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickLobbySelectToMainButtonAction);
	//Settings
	SettingsToMainMenuXButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickSettingsToMainMenuXButtonAction);
	TutorialToggle->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickTutorialToggleAction);
	SettingsToMainMenuButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickSettingsToMainMenuButtonAction);
	ControlsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickControlsButtonAction);
	MasterVolumeSlider->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnValueChangedMasterVolumeSliderAction);
	MusicSlider->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnValueChangedMusicSliderAction);
	SFXSlider->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnValueChangedSFXSliderAction);
	//Controls
	ControlsToSettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickControlsToSettingsButtonAction);

	UPotProbOnlineSubsystem* PotionsOnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
	if (!PotionsOnlineSubsystem)
	{
		return;
	}
	PotionsOnlineSubsystem->SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &ThisClass::OnFindSessionsComplete);
	OnClickedFindFriendsButtonAction();
	OnClickFindAllPublicButtonAction();

	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		if (SettingsSubsystem->GetCurrentSettings()->DisplayTutorial)
		{
			bIsTutorialOn=true;
			ShowTutorialText->SetText(FText::FromString("On"));
		}
		else
		{
			bIsTutorialOn=false;
			ShowTutorialText->SetText(FText::FromString("Off"));
		}

		MasterVolumeSlider->SetValue(SettingsSubsystem->GetCurrentSettings()->MasterVolume);
		VolumeText->SetText(FText::FromString(FString::FromInt(SettingsSubsystem->GetCurrentSettings()->MasterVolume)));
		MusicSlider->SetValue(SettingsSubsystem->GetCurrentSettings()->MusicVolume);
		MusicText->SetText(FText::FromString(FString::FromInt(SettingsSubsystem->GetCurrentSettings()->MusicVolume)));
		SFXSlider->SetValue(SettingsSubsystem->GetCurrentSettings()->SFXVolume);
		SFXText->SetText(FText::FromString(FString::FromInt(SettingsSubsystem->GetCurrentSettings()->SFXVolume)));
	}
	EnterCodeThrobber->SetVisibility(ESlateVisibility::Hidden);
	SetCodeThrobber->SetVisibility(ESlateVisibility::Hidden);
	FindPublicThrobber->SetVisibility(ESlateVisibility::Hidden);
	CreateLobbyThrobber->SetVisibility(ESlateVisibility::Hidden);
	
}

void UMainMenuWidget::NativeDestruct()
{
	
	//Main Menu
	PlayButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickPlayButtonAction);
	TutorialButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickTutorialButtonAction);
	CreditsButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickCreditsButtonAction);
	QuitButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickQuitButtonAction);
	SettingsButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickSettingsButtonAction);
	//Lobby Select
	JoinPublicButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickJoinPublicButtonAction);
	CreatePublicButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickCreatePublicButtonAction);
	JoinPrivateButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickJoinPrivateButtonAction);
	CreatePrivateButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickCreatePrivateButtonAction);
	LobbySelectToMainButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickLobbySelectToMainButtonAction);
	LobbySelectSettingsButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickSettingsButtonAction);
	//Join Public
	FindPublicSessionsButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickFindAllPublicButtonAction);
	JoinPublicToLobbySelectButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickJoinPublicToLobbySelectButtonAction);
	//Enter Code
	EnterCodeToLobbySelectButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickEnterCodeToLobbySelectButtonAction);
	ConfirmEnterCodeButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickConfirmEnterCodeButtonAction);
	SettingsButton_2->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickPlayButtonAction);
	SettingsButton_1->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickPlayButtonAction);
	EnterCodeToMainButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickLobbySelectToMainButtonAction);
	//Set Code
	SetCodeToLobbySelectButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickSetCodeToLobbySelectButtonAction);
	ConfirmSetCodeButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickConfirmSetCodeButtonAction);
	SettingsButton_3->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickPlayButtonAction);
	SetCodeToMainButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickLobbySelectToMainButtonAction);
	//Level Select
	BigMapButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickBigMapButtonAction);
	SmallMapButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickSmallMapButtonAction);
	CreateLobbyButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickCreateLobbyButtonAction);
	LevelSelectToLobbySelectButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickLevelSelectToLobbySelectButtonAction);
	MaxPlayersSlider->OnValueChanged.RemoveDynamic(this, &UMainMenuWidget::OnValueChangedMaxPlayersSliderAction);
	CreateLobbyToMainMenuBubtton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickLobbySelectToMainButtonAction);
	//Credits
	CreditsToMainMenuButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickLobbySelectToMainButtonAction);
	//Settings
	SettingsToMainMenuXButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickSettingsToMainMenuXButtonAction);
	TutorialToggle->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickTutorialToggleAction);
	SettingsToMainMenuButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickSettingsToMainMenuButtonAction);
	ControlsButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickControlsButtonAction);
	MasterVolumeSlider->OnValueChanged.RemoveDynamic(this, &UMainMenuWidget::OnValueChangedMasterVolumeSliderAction);
	MusicSlider->OnValueChanged.RemoveDynamic(this, &UMainMenuWidget::OnValueChangedMusicSliderAction);
	SFXSlider->OnValueChanged.RemoveDynamic(this, &UMainMenuWidget::OnValueChangedSFXSliderAction);
	//Controls
	ControlsToSettingsButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickControlsToSettingsButtonAction);
	//Legacy
	JoinFriendsButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnClickedFindFriendsButtonAction);
	
	UPotProbOnlineSubsystem* PotionsOnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
	if (!PotionsOnlineSubsystem)
	{
		return;
	}
	PotionsOnlineSubsystem->SessionInterface->OnFindSessionsCompleteDelegates.RemoveAll(this);
	Super::NativeDestruct();
}

void UMainMenuWidget::OnClickPlayButtonAction()
{
	// if (!bPlayerCompleteTutorial && bIsTutorialOn)
	// {
	// 	if (TutorialClassM)
	// 	{
	// 		UUserWidget* Tutorial = CreateWidget<UUserWidget>(GetWorld(), TutorialClassM);
	// 		if (Tutorial) 
	// 		{
	// 			Tutorial->AddToViewport();
	// 		}        
	// 	}
	// 	bPlayerCompleteTutorial = true;
	// }
	
	SetActiveWidgetByName(WS_MainMenu, "LobbySelect");
}

 

void UMainMenuWidget::OnClickTutorialButtonAction()
{
	if (TutorialClassM)
	{
		UTutorialWidget* Tutorial = CreateWidget<UTutorialWidget>(GetWorld(), TutorialClassM);
		if (Tutorial) 
		{
			Tutorial->SetWidgetInstigator(this);
			Tutorial->AddToViewport();
		}        
	}
	bPlayerCompleteTutorial = true;
}

void UMainMenuWidget::OnClickCreditsButtonAction()
{
	SetActiveWidgetByName(WS_MainMenu, "Credits");
}

void UMainMenuWidget::OnClickQuitButtonAction()
{
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->SaveSettings();
	}
	UKismetSystemLibrary::QuitGame(this, UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, true);
}

void UMainMenuWidget::OnClickSettingsButtonAction()
{
	SetActiveWidgetByName(WS_MainMenu, "Settings");
}

void UMainMenuWidget::OnClickJoinPublicButtonAction()
{
	OnClickFindAllPublicButtonAction();
	SetActiveWidgetByName(WS_MainMenu, "AllPublicLobbies");
}

void UMainMenuWidget::OnClickCreatePublicButtonAction()
{
	SetActiveWidgetByName(WS_MainMenu, "LevelSelect");
	UPotProbOnlineSubsystem* PotionsOnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
	if (!PotionsOnlineSubsystem)
	{
		return;
	}
	PlayersLobbyText->SetText(FText::FromString(PotionsOnlineSubsystem->GetCurrentPlayerName() + "'s Lobby"));
}

void UMainMenuWidget::OnClickJoinPrivateButtonAction()
{
	SetActiveWidgetByName(WS_MainMenu, "EnterCode");
}

void UMainMenuWidget::OnClickCreatePrivateButtonAction()
{
	SetActiveWidgetByName(WS_MainMenu, "SetCode");
}

void UMainMenuWidget::OnClickLobbySelectToMainButtonAction()
{
	SetActiveWidgetByName(WS_MainMenu, "MainMenu");
}

void UMainMenuWidget::OnClickFindAllPublicButtonAction()
{
	FindPublicThrobber->SetVisibility(ESlateVisibility::Visible);
	FoundSessionsBox->ClearChildren();
	UPotProbOnlineSubsystem* PotionsOnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
	if (!PotionsOnlineSubsystem)
	{
		return;
	}
	//if (!bDoAutoFindSessions)
	PotionsOnlineSubsystem->FindAllAvailableSessions();
}

void UMainMenuWidget::OnClickJoinPublicToLobbySelectButtonAction()
{
	SetActiveWidgetByName(WS_MainMenu, "LobbySelect");
}

void UMainMenuWidget::OnClickAllPublicLobbiesToLobbySelectButtonAction()
{
	SetActiveWidgetByName(WS_MainMenu, "LobbySelect");
}


void UMainMenuWidget::OnClickEnterCodeToLobbySelectButtonAction()
{
	EnterCodeTextBox->SetText(FText::FromString(""));
	// EnterCodeErrorText->SetVisibility(ESlateVisibility::Hidden);
	EnterCodeErrorAlert->SetVisibility(ESlateVisibility::Hidden);
	SetActiveWidgetByName(WS_MainMenu, "LobbySelect");
}

void UMainMenuWidget::OnClickConfirmEnterCodeButtonAction()
{
	EnterCodeThrobber->SetVisibility(ESlateVisibility::Visible);
	FString Passcode = EnterCodeTextBox->GetText().ToString();
	if (Passcode.IsEmpty()) 
	{
		// EnterCodeErrorText->SetVisibility(ESlateVisibility::Visible);
		EnterCodeErrorAlert->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		//LoadingPanelJoinPrivate->SetVisibility(ESlateVisibility::Visible);
		UPotProbOnlineSubsystem* OnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
		if (OnlineSubsystem)
		{
			OnlineSubsystem->FindSessionsByPasscode(
				Passcode,
				FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMainMenuWidget::HandleJoinPasscodeValidation)
			);
		}
	}
}

void UMainMenuWidget::OnClickSetCodeToLobbySelectButtonAction()
{
	SetCodeTextBox->SetText(FText::FromString(""));
	bIsLobbyPublic = true;
	// SetCodeErrorText->SetVisibility(ESlateVisibility::Hidden);
	SetCodeErrorAlert->SetVisibility(ESlateVisibility::Hidden);
	SetActiveWidgetByName(WS_MainMenu, "LobbySelect");
}

void UMainMenuWidget::OnClickConfirmSetCodeButtonAction()
{
	FString Passcode = SetCodeTextBox->GetText().ToString();
	if (Passcode.IsEmpty())
	{
		// SetCodeErrorText->SetVisibility(ESlateVisibility::Visible);
		SetCodeErrorAlert->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UPotProbOnlineSubsystem* OnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
		if (OnlineSubsystem)
		{
			SetCodeThrobber->SetVisibility(ESlateVisibility::Visible);
			OnlineSubsystem->FindSessionsByPasscode(
				Passcode,
				FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMainMenuWidget::HandleSetPasscodeValidation)
			);
		}
	}
}

void UMainMenuWidget::OnClickCreateLobbyButtonAction()
{
	if (UPotProbOnlineSubsystem* OnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>())
	{
		CreateLobbyThrobber->SetVisibility(ESlateVisibility::Visible);
		//LoadingPanelCreateLobby->SetVisibility(ESlateVisibility::Visible);
		OnlineSubsystem->SetMaxPlayers(MaxPlayersSlider->GetValue());
		OnlineSubsystem->SetLobbyPublic(bIsLobbyPublic);
		OnlineSubsystem->SetLevelType(bIsBigMapButtonSelected);
		if (!bIsLobbyPublic)
		{
			OnlineSubsystem->SetPrivatePasscode(PrivatePasscode);
		}
		OnlineSubsystem->HostSession();
	}
}

void UMainMenuWidget::OnClickLevelSelectToLobbySelectButtonAction()
{
	SetCodeTextBox->SetText(FText::FromString(""));
	MaxPlayersSlider->SetValue(10);
	SetActiveWidgetByName(WS_MainMenu, "LobbySelect");
}

void UMainMenuWidget::OnValueChangedMaxPlayersSliderAction(float Value)
{
	//Round the value to the nearest integer
	int32 RoundedValue = FMath::RoundToInt(Value);

	//Ensure the value stays within range
	RoundedValue = FMath::Clamp(RoundedValue, 4, 10);

	//Update the Slider's value to the rounded integer
	MaxPlayersSlider->SetValue(RoundedValue);

	//Update the displayed value
	if (CurrentMaxPlayersText)
	{
		FString ValueString = FString::FromInt(RoundedValue);
		CurrentMaxPlayersText->SetText(FText::FromString(ValueString));
	}
}

void UMainMenuWidget::OnClickSettingsToMainMenuXButtonAction()
{
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->SaveSettings();
	}
	SetActiveWidgetByName(WS_MainMenu, "MainMenu");
}

void UMainMenuWidget::OnClickTutorialToggleAction()
{
	if (bIsTutorialOn)
	{
		bIsTutorialOn = false;
		ShowTutorialText->SetText(FText::FromString("Off"));
	}
	else
	{
		bIsTutorialOn = true;
		ShowTutorialText->SetText(FText::FromString("On"));
	}
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->GetCurrentSettings()->DisplayTutorial = bIsTutorialOn;
	}
}

void UMainMenuWidget::OnClickSettingsToMainMenuButtonAction()
{
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->SaveSettings();
	}
	SetActiveWidgetByName(WS_MainMenu, "MainMenu");
}

void UMainMenuWidget::OnClickControlsButtonAction()
{
	SetActiveWidgetByName(WS_MainMenu, "Controls");
}

void UMainMenuWidget::OnValueChangedMasterVolumeSliderAction(float NewValue)
{
	//TODO: If we get sounds in main menu hook up RTPC here
	UAkGameplayStatics::SetRTPCValue(MasterVolumeRTPC, NewValue, 0, nullptr);
	VolumeText->SetText(FText::FromString(FString::FromInt(NewValue)));
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->GetCurrentSettings()->MasterVolume = NewValue;
	}
}

void UMainMenuWidget::OnValueChangedMusicSliderAction(float NewValue)
{
	//TODO: If we get sounds in main menu hook up RTPC here
	UAkGameplayStatics::SetRTPCValue(MusicVolumeRTPC, NewValue, 0, nullptr);
	MusicText->SetText(FText::FromString(FString::FromInt(NewValue)));
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->GetCurrentSettings()->MusicVolume = NewValue;
	}
}

void UMainMenuWidget::OnValueChangedSFXSliderAction(float NewValue)
{
	//TODO: If we get sounds in main menu hook up RTPC here
	UAkGameplayStatics::SetRTPCValue(SFXVolumeRTPC, NewValue, 0, nullptr);
	SFXText->SetText(FText::FromString(FString::FromInt(NewValue)));
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->GetCurrentSettings()->SFXVolume = NewValue;
	}
}

void UMainMenuWidget::OnClickControlsToSettingsButtonAction()
{
	SetActiveWidgetByName(WS_MainMenu, "Settings");
}

void UMainMenuWidget::OnClickBigMapButtonAction()
{
	BigMapButton->SetBackgroundColor(FLinearColor(0.25f, 0.25f, 0.25f));
	SmallMapButton->SetBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f));
	bIsBigMapButtonSelected = false;
}

void UMainMenuWidget::OnClickSmallMapButtonAction()
{
	SmallMapButton->SetBackgroundColor(FLinearColor(0.25f, 0.25f, 0.25f));
	BigMapButton->SetBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f));
	bIsBigMapButtonSelected = false;
}

void UMainMenuWidget::OnClickedFindFriendsButtonAction()
{
	FriendBox->ClearChildren();
	UPotProbOnlineSubsystem* PotionsOnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
	if (!PotionsOnlineSubsystem)
	{
		return;
	}
	FOnReadFriendsListComplete MyDelegate = FOnReadFriendsListComplete::CreateUObject(this,
		&UMainMenuWidget::OnReadFriendsComplete);
	PotionsOnlineSubsystem->ReadFriendsList(MyDelegate);
}

void UMainMenuWidget::OnReadFriendsComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Main Menu Widget could not successfully read friends on delegate"));
		return;
	}

	UPotProbOnlineSubsystem* PotionsOnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
	if (!PotionsOnlineSubsystem)
	{
		return;
	}
	PotionsOnlineSubsystem->SessionInterface->DumpSessionState();
	TArray<FString> Names = PotionsOnlineSubsystem->GetFriendsListNames();
	for (int i = 0; i < Names.Num(); ++i)
	{
		TObjectPtr<UFoundSessionWidget> NewButton = NewObject<UFoundSessionWidget>(this, FoundSessionButtonClass);
		FriendBox->AddChildToVerticalBox(NewButton);
		NewButton->SessionName->SetText(FText::FromString(Names[i]));
		NewButton->SetSessionIndex(i);
		NewButton->SetIsFriendSession(true);
	}
}

void UMainMenuWidget::OnFindSessionsComplete(bool bWasSuccessful)
{
	FindPublicThrobber->SetVisibility(ESlateVisibility::Hidden);
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Main Menu Widget could not successfully find sessions on delegate"));
		return;
	}

	UPotProbOnlineSubsystem* PotionsOnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
	if (!PotionsOnlineSubsystem)
	{
		return;
	}
	PotionsOnlineSubsystem->SessionInterface->DumpSessionState();

	const TArray<FOnlineSessionSearchResult>& SearchResults = PotionsOnlineSubsystem->GetSessionSearch();


	for (int i = 0; i < SearchResults.Num(); ++i)
	{
		FString Passcode;
		if (SearchResults[i].Session.SessionSettings.Get(FName("PrivatePasscode"), Passcode) && !Passcode.IsEmpty())
		{
			continue;
		}
		int32 MaxPlayers = SearchResults[i].Session.SessionSettings.NumPublicConnections;
		int32 CurrentPlayers = MaxPlayers - SearchResults[i].Session.NumOpenPublicConnections;
		TObjectPtr<UFoundSessionWidget> FoundSessionWidget = NewObject<UFoundSessionWidget>(this, FoundSessionButtonClass);
		FoundSessionsBox->AddChildToVerticalBox(FoundSessionWidget);
		FoundSessionWidget->SessionName->SetText(FText::FromString(SearchResults[i].Session.OwningUserName + "'s Lobby"));
		//FoundSessionWidget->PingAmount->SetText(FText::FromString(FString::FromInt(SearchResults[i].PingInMs)));
		//FoundSessionWidget->MapName->SetText(FText::FromString(SearchResults[i].Session.SessionSettings.Settings.FindRef(FName("MAPNAME")).Data.ToString()));
		FoundSessionWidget->PlayerCount->SetText(FText::FromString(FString::FromInt(CurrentPlayers) + "/" + FString::FromInt(MaxPlayers)));
		FoundSessionWidget->SetSessionIndex(i);
	}
}

void UMainMenuWidget::SetActiveWidgetByName(UWidgetSwitcher* WidgetSwitcher, const FString& WidgetName)
{
	if (!WidgetSwitcher) return;

	for (int32 Index = 0; Index < WidgetSwitcher->GetNumWidgets(); ++Index) //Loops through widgets in the widget switcher to get the one you named
	{
		UWidget* ChildWidget = WidgetSwitcher->GetWidgetAtIndex(Index);
		if (ChildWidget && ChildWidget->GetName() == WidgetName)
		{
			WidgetSwitcher->SetActiveWidgetIndex(Index);
			break;
		}
	}
}

void UMainMenuWidget::HandleSetPasscodeValidation(bool bWasSuccessful)
{
	SetCodeThrobber->SetVisibility(ESlateVisibility::Hidden);
	UPotProbOnlineSubsystem* OnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
	if (!OnlineSubsystem || !bWasSuccessful || OnlineSubsystem->GetPasscodeSearch().Num() > 0)
	{
		// SetCodeErrorText->SetVisibility(ESlateVisibility::Visible);
		SetCodeErrorAlert->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	
	PrivatePasscode = SetCodeTextBox->GetText().ToString();
	bIsLobbyPublic = false;
	// SetCodeErrorText->SetVisibility(ESlateVisibility::Hidden);
	SetCodeErrorAlert->SetVisibility(ESlateVisibility::Hidden);
	SetActiveWidgetByName(WS_MainMenu, "LevelSelect");
}

void UMainMenuWidget::HandleJoinPasscodeValidation(bool bWasSuccessful)
{
	EnterCodeThrobber->SetVisibility(ESlateVisibility::Hidden);
	UPotProbOnlineSubsystem* OnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
	if (!OnlineSubsystem || !bWasSuccessful || OnlineSubsystem->GetPasscodeSearch().Num() == 0)
	{
		// EnterCodeErrorText->SetVisibility(ESlateVisibility::Visible);
		EnterCodeErrorAlert->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	// Join the first found session with matching passcode
	// EnterCodeErrorText->SetVisibility(ESlateVisibility::Hidden);
	EnterCodeErrorAlert->SetVisibility(ESlateVisibility::Hidden);
	OnlineSubsystem->JoinSearchedSession(0);
}


void UMainMenuWidget::OnEnterCodeTextChanged(const FText& Text)
{
    ValidateNumericInput(Text, LastValidEnterCode, EnterCodeTextBox);
}

void UMainMenuWidget::OnSetCodeTextChanged(const FText& Text)
{
    ValidateNumericInput(Text, LastValidSetCode, SetCodeTextBox);
}

FText UMainMenuWidget::ValidateNumericInput(const FText& Input, FText& LastValid, UEditableTextBox* TextBox)
{
    // Get the current text as a string
    FString CurrentTextString = Input.ToString();
    bool bIsValid = true;
    
    // Check if the string is longer than 4 characters
    if (CurrentTextString.Len() > 4)
    {
        // Truncate to 4 characters
        CurrentTextString = CurrentTextString.Left(4);
        bIsValid = false;
    }
    
    // Check if all characters are numeric
    for (TCHAR Character : CurrentTextString)
    {
        if (!FChar::IsDigit(Character))
        {
            bIsValid = false;
            break;
        }
    }
    
    if (bIsValid)
    {
        // Save the current valid input
        LastValid = FText::FromString(CurrentTextString);
        return LastValid;
    }
    else
    {
        // Determine if the truncated string is valid
        bool bTruncatedValid = true;
        for (TCHAR Character : CurrentTextString)
        {
            if (!FChar::IsDigit(Character))
            {
                bTruncatedValid = false;
                break;
            }
        }
        
        if (bTruncatedValid && CurrentTextString.Len() > 0)
        {
            LastValid = FText::FromString(CurrentTextString);
        }
        
        // Block execution of the handler to prevent an infinite loop
        TextBox->OnTextChanged.RemoveDynamic(
            this, 
            TextBox == EnterCodeTextBox ? 
                &UMainMenuWidget::OnEnterCodeTextChanged : 
                &UMainMenuWidget::OnSetCodeTextChanged
        );
        
        // Update the text box with the last valid input
        TextBox->SetText(LastValid);
        
        // Re-bind the event handler
        TextBox->OnTextChanged.AddDynamic(
            this, 
            TextBox == EnterCodeTextBox ? 
                &UMainMenuWidget::OnEnterCodeTextChanged : 
                &UMainMenuWidget::OnSetCodeTextChanged
        );
        
        return LastValid;
    }
}