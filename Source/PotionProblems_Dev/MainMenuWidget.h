// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CustomAnimWidget.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Online/CoreOnline.h"
#include "AkGameplayStatics.h"
#include "Components/CanvasPanel.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UImage;
class UVerticalBox;
class UWidgetSwitcher;
class USlider;
class UTextBlock;
class UEditableTextBox;
class UCircularThrobber;
/**
 *
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	//Animated Buttons - Help//
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* CreditsButtonAnim;

	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* QuitButtonAnim;

	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* JoinPrivateButtonAnim;

	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* CreatePrivateButtonAnim;

	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* JoinPublicButtonAnim;

	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* CreatePublicButtonAnim;

	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* LobbySelectToMainButtonAnim;

	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* ConfirmSetCodeButtonAnim;

	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* SetCodeToLobbySelectButtonAnim;

	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* EnterCodeToLobbySelectButtonAnim;

	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* ConfirmEnterCodeButtonAnim;

	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* CreateLobbyButtonAnim;

	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* LevelSelectToLobbySelectButtonAnim;

	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* FindPublicSessionsButtonAnim;

	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* TutorialToggleAnim;
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* SettingsToMainMenuButtonAnim;
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* ControlsButtonAnim;
	
	//Main Menu UI
	UPROPERTY(Meta = (BindWidget))
	UButton* PlayButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* TutorialButton;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> TutorialClassM;
	UPROPERTY(Meta = (BindWidget))
	UButton* CreditsButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* QuitButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* SettingsButton;
	
	//Lobby Select Menu UI
	UPROPERTY(Meta = (BindWidget))
	UButton* JoinPublicButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* CreatePublicButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* JoinPrivateButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* CreatePrivateButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* LobbySelectToMainButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* LobbySelectSettingsButton;

	//Join Public UI
	UPROPERTY(Meta = (BindWidget))
	UButton* FindPublicSessionsButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* JoinPublicToLobbySelectButton;
	UPROPERTY(Meta = (BindWidget))
	UVerticalBox* FoundSessionsBox;
	UPROPERTY(Meta = (BindWidget))
	UCircularThrobber* FindPublicThrobber;
	UPROPERTY(Meta = (BindWidget))
	UButton* AllPublicLobbiesToLobbySelectButton;

	//SetCode UI
	UPROPERTY(Meta = (BindWidget))
	UButton* SetCodeToLobbySelectButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* ConfirmSetCodeButton;
	UPROPERTY(Meta = (BindWidget))
	UEditableTextBox* SetCodeTextBox;
	// UPROPERTY(Meta = (BindWidget))
	// UTextBlock* SetCodeErrorText;
	UPROPERTY(Meta = (BindWidget))
	UImage* SetCodeErrorAlert;
	UPROPERTY(Meta = (BindWidget))
	UButton* SettingsButton_3;
	UPROPERTY(Meta = (BindWidget))
	UCircularThrobber* SetCodeThrobber;
	UPROPERTY(Meta = (BindWidget))
	UButton* SetCodeToMainButton;


	//EnterCode UI
	UPROPERTY(Meta = (BindWidget))
	UButton* EnterCodeToLobbySelectButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* ConfirmEnterCodeButton;
	UPROPERTY(Meta = (BindWidget))
	UEditableTextBox* EnterCodeTextBox;
	// UPROPERTY(Meta = (BindWidget))
	// UTextBlock* EnterCodeErrorText;
	UPROPERTY(Meta = (BindWidget))
	UImage* EnterCodeErrorAlert;
	UPROPERTY(Meta = (BindWidget))
	UButton* SettingsButton_2;
	UPROPERTY(Meta = (BindWidget))
	UButton* SettingsButton_1;
	UPROPERTY(Meta = (BindWidget))
	UCircularThrobber* EnterCodeThrobber;
	UPROPERTY(Meta = (BindWidget))
	UButton* EnterCodeToMainButton;
	
	//Level Select UI
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PlayersLobbyText;
	UPROPERTY(Meta = (BindWidget))
	UButton* BigMapButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* SmallMapButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* CreateLobbyButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* LevelSelectToLobbySelectButton;
	UPROPERTY(Meta = (BindWidget))
	USlider* MaxPlayersSlider;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* CurrentMaxPlayersText;
	UPROPERTY(Meta = (BindWidget))
	UCircularThrobber* CreateLobbyThrobber;
	UPROPERTY(Meta = (BindWidget))
	UButton* CreateLobbyToMainMenuBubtton;

	// Credits
	UPROPERTY(Meta = (BindWidget))
	UButton* CreditsToMainMenuButton;

	//Settings
	UPROPERTY(Meta = (BindWidget))
	UButton* SettingsToMainMenuXButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* TutorialToggle;
	UPROPERTY(Meta = (BindWidget))
	UButton* SettingsToMainMenuButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* ControlsButton;
	UPROPERTY(Meta = (BindWidget))
	USlider* MasterVolumeSlider;
	UPROPERTY(Meta = (BindWidget))
	USlider* MusicSlider;
	UPROPERTY(Meta = (BindWidget))
	USlider* SFXSlider;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* VolumeText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* MusicText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* SFXText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ShowTutorialText;

	//Settings Values
	UPROPERTY(EditAnywhere)
	UAkRtpc* MasterVolumeRTPC;
	UPROPERTY(EditAnywhere)
	UAkRtpc* MusicVolumeRTPC;
	UPROPERTY(EditAnywhere)
	UAkRtpc* SFXVolumeRTPC;
	
	//Controls
	UPROPERTY(Meta = (BindWidget))
	UButton* ControlsToSettingsButton;
	
	// Loading Screens
	UPROPERTY(Meta = (BindWidget))
	UCanvasPanel* LoadingPanelJoinPublic;
	UPROPERTY(Meta = (BindWidget))
	UCanvasPanel* LoadingPanelJoinPrivate;
	UPROPERTY(Meta = (BindWidget))
	UCanvasPanel* LoadingPanelCreateLobby;
	
	//Legacy UI
	UPROPERTY(Meta = (BindWidget))
	UButton* JoinFriendsButton;
	UPROPERTY(Meta = (BindWidget))
	UVerticalBox* FriendBox;

	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UWidgetSwitcher* WS_MainMenu;

	bool bIsTutorialOn = true;
	
	// Debug
#ifdef UE_EDITOR
	bool bDoAutoFindSessions = true;
#else
	bool bDoAutoFindSessions = false;
#endif
	

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variable")
	TSubclassOf<class UFoundSessionWidget> FoundSessionButtonClass;

	bool bIsBigMapButtonSelected = false;

	FString PrivatePasscode = "";
	bool bIsLobbyPublic = true;

	bool bPlayerCompleteTutorial = false;

	//Main Menu
	UFUNCTION(BlueprintCallable)
	void OnClickPlayButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickTutorialButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickCreditsButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickQuitButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickSettingsButtonAction();
	//Lobby Select
	UFUNCTION(BlueprintCallable)
	void OnClickJoinPublicButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickCreatePublicButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickJoinPrivateButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickCreatePrivateButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickLobbySelectToMainButtonAction();
	//Join Public
	UFUNCTION(BlueprintCallable)
	void OnClickFindAllPublicButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickJoinPublicToLobbySelectButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickAllPublicLobbiesToLobbySelectButtonAction();
	//Enter Code
	UFUNCTION(BlueprintCallable)
	void OnClickEnterCodeToLobbySelectButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickConfirmEnterCodeButtonAction();
	//Set Code
	UFUNCTION(BlueprintCallable)
	void OnClickSetCodeToLobbySelectButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickConfirmSetCodeButtonAction();
	// Handler for EnterCodeTextBox changes
	UFUNCTION()
	void OnEnterCodeTextChanged(const FText& Text);
	// Handler for SetCodeTextBox changes
	UFUNCTION()
	void OnSetCodeTextChanged(const FText& Text);
    // Stores the last valid numeric input for each text box
    FText LastValidEnterCode;
    FText LastValidSetCode;
    
    // Helper function to validate and format numeric input
    FText ValidateNumericInput(const FText& Input, FText& LastValid, UEditableTextBox* TextBox);

	//Level Select
	UFUNCTION(BlueprintCallable)
	void OnClickBigMapButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickSmallMapButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickCreateLobbyButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickLevelSelectToLobbySelectButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnValueChangedMaxPlayersSliderAction(float Value);
	//Settings
	UFUNCTION(BlueprintCallable)
	void OnClickSettingsToMainMenuXButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickTutorialToggleAction();
	UFUNCTION(BlueprintCallable)
	void OnClickSettingsToMainMenuButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnClickControlsButtonAction();
	UFUNCTION(BlueprintCallable)
	void OnValueChangedMasterVolumeSliderAction(float NewValue);
	UFUNCTION(BlueprintCallable)
	void OnValueChangedMusicSliderAction(float NewValue);
	UFUNCTION(BlueprintCallable)
	void OnValueChangedSFXSliderAction(float NewValue);
	//Controls
	UFUNCTION(BlueprintCallable)
	void OnClickControlsToSettingsButtonAction();
	//Legacy
	UFUNCTION(BlueprintCallable)
	void OnClickedFindFriendsButtonAction();

	void OnReadFriendsComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);
	void OnFindSessionsComplete(bool bWasSuccessful);

	void SetActiveWidgetByName(UWidgetSwitcher* WidgetSwitcher, const FString& WidgetName);

	void HandleSetPasscodeValidation(bool bWasSuccessful);
	void HandleJoinPasscodeValidation(bool bWasSuccessful);
};
