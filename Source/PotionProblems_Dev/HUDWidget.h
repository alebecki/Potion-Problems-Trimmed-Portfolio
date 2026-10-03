// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterData.h"
#include "CustomAnimWidget.h"
#include "Blueprint/UserWidget.h"
#include "PotProbGameMode.h"
#include "Components/CanvasPanel.h"
#include "HUDWidget.generated.h"

class UBackgroundBlur;
class URecipeWidget;
class UScrollBox;
class UVerticalBox;
class UHorizontalBox;
class USizeBox;
class UTextBlock;
class UEditableTextBox;
class UProgressBar;
class UImage;
class UButton;
class UBorder;
class UAlertWidget;
class URetainerBox;
class USlider;
class UWidgetSwitcher;
class UAkAudioEvent;
class UAkRtpc;
class UPotionWidget;
class ULobbyStatusWidget;
enum class EAnimModels : uint8;

/**
 *
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/* HUD Tutorial */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> TutorialClass;
	/* End HUD Tutorial Assets */
	
	/* Player Role Assets */
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PlayerRole;
	/* End Player Role Assets */

	void CauldronMessage();
	void EndCauldronMessage();
	void UpdateCharacterSprite(const FCharacterData& CharacterData);

	/* Debug Relevant Assets. TODO: To Be Removed for final product */
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PlayerFrogged;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* LobbyStatus; // Still in use?
	/* End Debug Relevant Assets. TODO: To Be Removed for final product */
	
	/* Lobby Status Assets */
	UPROPERTY(Meta = (BindWidget))
	UVerticalBox* LobbyStatusNamesBox;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* CurrentNumPlayers;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TotalNumPlayers;
	/* End Lobby Status Assets */
	
	
	/* Discover Frogged HUD Assets */
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* InteractHint;
	/* End Discover Frogged HUD Assets */

	/* Anim Buttons */
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* ReadyButtonAnim;
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* CloseButtonAnim;
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* LobbyButtonAnim;
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* CharacterCustomizeButtonAnim;
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* ExitGameButtonAnim;
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* QuitGameButtonAnim;
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* PlayAgainButtonAnim;
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* EndGameContinueButtonAnim;
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* ResumeButtonAnim;
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* ControlsButtonAnim;
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* ExitButtonAnim;
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* ControlsToSettingsButtonAnim;
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* TutorialToggleAnim;
	
	/* Game Information HUD Assets */ 
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PingMS;
	UPROPERTY(Meta = (BindWidget))
	UProgressBar* PotionCompletionProgress;
	UPROPERTY(Meta = (BindWidget))
	UButton* MapButton;
	UPROPERTY(Meta = (BindWidget))
	UBorder* Map;
	UPROPERTY(Meta = (BindWidget))
	UButton* CloseMapButton;
	UPROPERTY(Meta = (BindWidget))
	UImage* CharacterSprite;
	UPROPERTY(Meta = (BindWidget))
	UImage* MapImage;
	UPROPERTY(Meta = (BindWidget))
	UImage* SmallMapImage;
	UPROPERTY(Meta = (BindWidget))
	UVerticalBox* StatusEffectVerticalBox;
	UPROPERTY(Meta = (BindWidget))
	UImage* PotionSlotImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* DefaultPotionSlotImage;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* SplashText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* DrinkText;
	UPROPERTY(Meta = (BindWidget))
	UImage* DrinkPotionBanner;
	UPROPERTY(Meta = (BindWidget))
	UImage* DrinkPotionCircle;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* DrinkPotionBind;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PotionNameText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PotionHeldNameText;
	UPROPERTY(Meta = (BindWidget))
	UButton* ReadyButton;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ReadyButtonText;
	UPROPERTY(Meta = (BindWidget))
	UButton* CharacterCustomizeButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* OpenTutorialButton;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* CharacterCustomizeButtonText;
	UPROPERTY(Meta = (BindWidget))
	URecipeWidget* FrogRecipeInfo;
	UPROPERTY(Meta = (BindWidget))
	UButton* CloseButton;
	UPROPERTY(Meta = (BindWidget))
	UImage* PigeonPoint;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ScrollCloseText;
	UPROPERTY(Meta = (BindWidget))
	UBackgroundBlur* Blur;

	//UPROPERTY(Meta = (BindWidget))
	//URecipeWidget* PotionRecipeInfo;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* OpenMapText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ChatText;
	UPROPERTY(Meta = (BindWidget))
	UImage* ChatSprite;

	UPROPERTY(Meta = (BindWidget))
	UImage* PlayerLocationImage;
	UPROPERTY(Meta = (BindWidget))
	UImage* CauldronIcon1;
	UPROPERTY(Meta = (BindWidget))
	UImage* CauldronIcon2;
	UPROPERTY(Meta = (BindWidget))
	UImage* CauldronIcon3;
	UPROPERTY(Meta = (BindWidget))
	UImage* CauldronIcon4;
	UPROPERTY(Meta = (BindWidget))
	UImage* CauldronIcon5;
	UPROPERTY(Meta = (BindWidget))
	UImage* CauldronIcon6;
	UPROPERTY(Meta = (BindWidget))
	UImage* CauldronIcon7;
	UPROPERTY(Meta = (BindWidget))
	UImage* CauldronIcon8;
	UPROPERTY(Meta = (BindWidget))
	UImage* CauldronIcon9;
	UPROPERTY(Meta = (BindWidget))
	UImage* CauldronIcon10;
	UPROPERTY(Meta = (BindWidget))
	UCanvasPanel* MapCanvasPanel;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* CauldronSwitchMessage;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* CootiesText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* DropIngredientText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* DropIngredientBind;
	UPROPERTY(Meta = (BindWidget))
	UImage* DropIngredientImage;
	UPROPERTY(Meta = (BindWidget))
	UImage* DropIngredientBanner;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* CompletionProgress;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* FrogTunnelText;
	UPROPERTY(Meta = (BindWidget))
	UButton* SettingsButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* WASDButton;
	UPROPERTY(Meta = (BindWidget))
	UBorder* ControlTips;
	UPROPERTY(Meta = (BindWidget))
	UButton* LobbyButton;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* MapName;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* MapDetails;
	/* End Game Information HUD Assets */

	/* End Game Information HUD Assets */
	UPROPERTY(Meta = (BindWidget))
	UButton* EndGameContinueButton;
	/* End End Game Information HUD Assets */
	
	/* Start Loop Game Information HUD Assets */
	UPROPERTY(Meta = (BindWidget))
	UButton* ExitGameButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* QuitGameButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* PlayAgainButton;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PlayAgainButtonText;
	/*UPROPERTY(Meta = (BindWidget))
	UTextBlock* TimerText;*/
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TimerReasonText;
	UPROPERTY(Meta = (BindWidget))
	UHorizontalBox* PlayAgainHorizontal;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PlayersReadiedAgainText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PlayersInGameText;
	/* End Loop Game Information HUD Assets */

	/*Start End Game Screen HUD Assets*/
	//Apprentices Win Screen (EndGameApprenticeWin)
	UPROPERTY(Meta = (BindWidget))
	UImage* ApprenticeWinReasonText1;
	UPROPERTY(Meta = (BindWidget))
	UImage* ApprenticeWinReasonText2;
	UPROPERTY(Meta = (BindWidget))
	UImage* ApprenticePlayerSprite;
	UPROPERTY(Meta = (BindWidget))
	UHorizontalBox* ApprenticePlayersRightBox;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* ApprenticeSizebox1;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* ApprenticeSizebox2;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* ApprenticeSizebox3;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* ApprenticeSizebox4;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* ApprenticeSizebox5;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* ApprenticeSizebox6;
	UPROPERTY(Meta = (BindWidget))
	UHorizontalBox* ApprenticePlayersLeftBox;
	//Troublemaker Win Screen (EndGameTroublemakerWin)
	UPROPERTY(Meta = (BindWidget))
	UHorizontalBox* TroublemakerPlayersBox;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* TroublemakerSizebox1;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* TroublemakerSizebox2;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* TroublemakerSizebox3;
	//Apprentice Win Screen for Troublemakers (EndGameApprenticeWinAsTroublemaker)
	UPROPERTY(Meta = (BindWidget))
	UImage* ApprenticeWinReasonTextTroublemaker1;
	UPROPERTY(Meta = (BindWidget))
	UImage* ApprenticeWinReasonTextTroublemaker2;
	UPROPERTY(Meta = (BindWidget))
	UHorizontalBox* ApprenticePlayersRightBoxTroublemaker;
	UPROPERTY(Meta = (BindWidget))
	UHorizontalBox* ApprenticePlayersLeftBoxTroublemaker;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* AllApprenticeSizebox1;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* AllApprenticeSizebox2;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* AllApprenticeSizebox3;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* AllApprenticeSizebox4;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* AllApprenticeSizebox5;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* AllApprenticeSizebox6;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* AllApprenticeSizebox7;
	/*End End Game Screen HUD Assets*/

	/*Start End Game Stats HUD Assets*/
	//Apprentice Stats (EndGameApprenticeStats)
	UPROPERTY(Meta = (BindWidget))
	USlider* PotionsCreatedSlider;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* NumOfPotionsCreatedText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TotalNumOfPotionsCreatedText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ApprenticeStatsPlayerNameText;
	UPROPERTY(Meta = (BindWidget))
	UImage* ApprenticeStatsPlayerSprite;
	UPROPERTY(Meta = (BindWidget))
	UHorizontalBox* TroublemakersVotedCorrectlyBox;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* VotedTroublemakerSizebox1;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* VotedTroublemakerSizebox2;
	UPROPERTY(Meta = (BindWidget))
	USizeBox* VotedTroublemakerSizebox3;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> TroublemakersVotedClass;
	//Troublemaker Stats (EndGameTroublemakerStats)
	UPROPERTY(Meta = (BindWidget))
	USlider* PlayersFroggedSlider;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* NumPlayersFroggedText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TotalNumOfPlayersFroggedText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TroublemakerStatsPlayerNameText;
	UPROPERTY(Meta = (BindWidget))
	UImage* TroublemakerStatsPlayerSprite;
	UPROPERTY(Meta = (BindWidget))
	USlider* VotingRoundsSurvivedSlider;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* NumVotingRoundsSurvivedText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TotalNumOfRoundsSurvivedText;
	/*End End Game Stats HUD Assets*/
	
	/* Start Settings HUD Assets */
	UPROPERTY(Meta = (BindWidget))
	UButton* SettingsToMainMenuXButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* TutorialToggle;
	UPROPERTY(Meta = (BindWidget))
	UButton* ResumeButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* ControlsButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* ExitButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* ResumeFromControls;
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

	UPROPERTY(Meta = (BindWidget))
	UButton* ControlsToSettingsButton;

	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UWidgetSwitcher* WSHUDWidget;
	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UWidgetSwitcher* WSGameStates;
	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UWidgetSwitcher* WSPlayerType;
	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UWidgetSwitcher* WSEndGame;
	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UWidgetSwitcher* WSEndGameLoopStats;
	/* End Settings HUD Assets */

	/* Start Settings Values */
	bool bIsTutorialOn = true;
	UPROPERTY(EditAnywhere)
	UAkRtpc* MasterVolumeRTPC;
	UPROPERTY(EditAnywhere)
	UAkRtpc* MusicVolumeRTPC;
	UPROPERTY(EditAnywhere)
	UAkRtpc* SFXVolumeRTPC;
	/* End Settings Values */
	
	/* Progress Bar Data */
	// Lerp Alpha Value for Progress Bar
	UPROPERTY(EditDefaultsOnly, Category="Designer Variables")
	float AlphaValue;
	// Determines how fast the progress bar will ease into the target value
	UPROPERTY(EditDefaultsOnly, Category="Designer Variables")
	float DegreeOfLerp;
	/* End Progress Bar Data */

	/*Drop Ingredient Display*/
	void DisplayDropIngredientText(bool bVal);

	/* Potion Popup */
	// bool to return success of failure (failure if no potion widget exists)
	bool RemovePotionWidget();
	/* End Potion Popup */

	/* Potion Drink Text */
	void ToggleDrinkText(bool bEffectActive) const;
	/* End Potion Drink Text */

	UFUNCTION(BlueprintCallable)
	void DEBUG_AddApprenticeCraftedPotionByOne();

	// Lobby Status Display
	UPROPERTY()
	TMap<FString, ULobbyStatusWidget*> LobbyStatusWidgets;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULobbyStatusWidget> LobbyStatusWidgetClass;
	
	// End Game Display Function
	void UpdateEndGameScreen(bool bApprenticesWin, bool bWonByVoting);
	
	/* Level Switcher Functions */
	void IncrementHUDGameState();
	void DisplayEndScreen();
	void IncrementEndGameScreen();
	void DecrementEndGameScreen();
	/* End Level Switcher Functions */

	/* Player Switcher Functions */
	void SetHUDPlayerType(bool bIsTroublemaker);
	/* End Player Switcher Functions */
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/* Tutorial */
	UFUNCTION()
	void OnOpenTutorialButtonClicked();
	/* End Tutorial */

	/* HUD Switcher */
	void SetActiveWidgetByName(UWidgetSwitcher* WidgetSwitcher, const FString& WidgetName);
	/* End HUD Switchers */

	/* Settings Buttons & Sliders */
	UFUNCTION(BlueprintCallable)
	void OnSettingsButtonClicked();
	UFUNCTION(BlueprintCallable)
	void OnSettingsToMainMenuXButtonClicked();
	UFUNCTION(BlueprintCallable)
	void OnTutorialToggleClicked();
	UFUNCTION(BlueprintCallable)
	void OnResumeButtonClicked();
	UFUNCTION(BlueprintCallable)
	void OnControlsButtonClicked();
	UFUNCTION(BlueprintCallable)
	void OnExitButtonClicked();
	UFUNCTION(BlueprintCallable)
	void OnMasterVolumeSliderValueChanged(float NewValue);
	UFUNCTION(BlueprintCallable)
	void OnMusicSliderValueChanged(float NewValue);
	UFUNCTION(BlueprintCallable)
	void OnSFXSliderValueChanged(float NewValue);
	UFUNCTION(BlueprintCallable)
	void OnControlsToSettingsButtonClicked();
	UFUNCTION(BlueprintCallable)
	void OnControlTipButtonClicked();
	/* End Settings Buttons & Sliders */
	
	/* Progress Bar */
	float PreviousPercentage = 0.0f;
	float TargetPercentage = 0.0f;
	/* End Progress Bar */
	
	/* Alert Messages Start */
	UPROPERTY(BlueprintReadWrite, Category = "Alert")
	float AlertLifetimeDuration = 4.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Alert")
	float TutorialAlertLifetimeDuration = 7.0f;

	// Timer for processing Alert Messages
	FTimerHandle LowPriorityAlertMessageTimerHandle;
	FTimerHandle HighPriorityAlertMessageTimerHandle;
	FTimerHandle TutorialAlertMessageTimerHandle;
	FTimerHandle DelayTimerHandle;

	// Queue of alert messages for High or Low alerts
	TQueue<FString> LowPriorityAlertMessageQueue;
	TQueue<FString> HighPriorityAlertMessageQueue;
	TQueue<FString> TutorialAlertMessageQueue;

	// Whether the High or Low Priority message queue is currently processing
	bool bIsProcessing_LowPriorityQueue = false;
	bool bIsProcessing_HighPriorityQueue = false;
	bool bIsProcessing_TutorialPriorityQueue = false;

	// The Blueprint widget class for AlertWidget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> AlertWidgetClass;

	// The Blueprint widget class for Tutorial AlertWidget that shows up at the bottom of the screen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> TutorialAlertWidgetClass;
	
	// Canvas for Alerts
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* LowPriorityAlertCanvas;
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* HighPriorityAlertCanvas;
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* TutorialAlertCanvas;

	// Widgets for Alerts
	UAlertWidget* LowAlertWidgetInstance = nullptr;
	UAlertWidget* HighAlertWidgetInstance = nullptr;
	UAlertWidget* TutorialAlertWidgetInstance = nullptr;

	/* Alert Messages End */

	/* Ingredient And Potion Image */
	UPROPERTY(EditDefaultsOnly, Category="Designer Variables")
	UTexture2D* NoIngredientTexture;
	UPROPERTY(EditDefaultsOnly, Category="Designer Variables")
	UTexture2D* NoPotionTexture;
	/* End Ingredient Image */
	
	/* Potion Popup */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Designer Variables | Potion Popup")
	TSubclassOf<UPotionWidget> PotionWidgetClass;
	UPROPERTY()
	UPotionWidget* PotionWidgetInstance = nullptr;
	/* End Potion Popup */

	/* Map Size Data */
	UPROPERTY(EditDefaultsOnly, Category = "Map Settings")
	FVector MapPosition;
	UPROPERTY(EditDefaultsOnly, Category = "Map Settings")
	FVector MapSize;
	/* End of Map Size Data */

	/* End Game Data */
	UPROPERTY(EditDefaultsOnly, Category="Designer Variables")
	FString ApprenticeWinString = "APPRENTICES WIN!";
	UPROPERTY(EditDefaultsOnly, Category="Designer Variables")
	FString TroubleMakerWinString = "TROUBLEMAKERS WIN!";
	UPROPERTY(EditDefaultsOnly, Category="Designer Variables")
	FString InsufficientPlayersForLoop = "Insufficient Number of Players for Another Game. Returning to Main Menu";
	UPROPERTY(EditDefaultsOnly, Category="Designer Variables")
	FString NotPlayingAgainKick = "Not Playing Again. Returning to Main Menu";
	UPROPERTY(EditDefaultsOnly, Category="Designer Variables")
	FString LoadingNewLevelText = "Loading New Level";
	/* End of End Game Data */

	void CheapShotForIngredientDisplay();

	FTimerHandle TimerSwitcharooMessage;

	// Audio Alert
	// void RemoveAlert();

	bool bHasOpenedMap = false;
	TArray<UImage*> CauldronIcons;

	UPROPERTY(Transient)
	TMap<EAnimModels, UTexture*> CharacterTextures;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* CharacterDataTable;
	
public:
	TMap<EAnimModels, UTexture*>& GetCharacterTextures() { return CharacterTextures; }
	
	/* Alert Signals */
	void AddAlertSignalMessage(const EPotProbAlertTypes& AlertType, const FString& MessageContent);
	// Two queues, a low and high priority one for each Alert section in the HUD
	void ProcessLowPriorityAlertMessageQueue();
	void ProcessHighPriorityAlertMessageQueue();
	void ProcessTutorialAlertMessageQueue();
	/* End Alert Signals */

	/* Debug Functions. TODO: To be removed for final product */ 
	UFUNCTION()
	void UpdateLobbyStatus();
	/* End Debug Functions. TODO: To be removed for final product */
	
	/* Progress Bar Functions */
	void UpdateProgressBar();
	/* End Progress Bar Functions */
	/* Map Button Functions */
	UFUNCTION()
	void OnMapButtonClicked();
	UFUNCTION()
	void OnCloseButtonClicked();
	/* End Map Button Functions */

	/* Ready Button Function */
	UFUNCTION()
	void OnReadyButtonClicked();
	/* End Ready Button Function*/

	/* Map Icon Functions */
	void UpdatePlayerPositionIcon(FVector PlayerPos);
	FVector ConvertFromWorldspaceToUISpace(FVector WorldspacePos);
	/* End of Map Icon Functions */

	
	/* Character Customize Button Function */
	UFUNCTION()
	void OnChracterCustomizeButtonClicked();
	/* End Character Customize Button Function */

	/* Potion And Ingredient Functions */
	void CreatePotionWidgetPopup(UPotionObject* Potion);
	void AddRecipeInformationToHUD(const FRecipeStruct& Recipe);
	void AddFrogRecipeInformationToHUD(const FRecipeStruct& FrogRecipe);
	void DisplayCootiesData(bool bShouldDisplay);

	/* Trigger Box Debug Message*/
	UFUNCTION(BlueprintCallable)
	void DisplayFrogTunnelMessage();

	UFUNCTION(BlueprintCallable)
	void HideFrogTunnelMessage();

	/* Tutorial */
	void AddTutorialFlipBookToViewport();
	/* End Tutorial */

	/* Switching from lobby HUD	to game HUD */
	void UpdateMapText(FString MapText);
	/* End Switching from lobby HUD	to game HUD */

	/* Game Loop */
	UFUNCTION(BlueprintCallable)
	void OnEndGameContinueButtonClicked();
	UFUNCTION(BlueprintCallable)
	void OnLobbyButtonClicked();
	UFUNCTION(BlueprintCallable)
	void OnQuitGameButtonClicked();
	UFUNCTION(BlueprintCallable)
	void OnPlayAgainButtonClicked();
	
	void UpdateNumInGamePlayers(int NewNumPlayers);
	void UpdateNumLoopingPlayers();
	void UpdateLoopReason(bool bKickAll);
	
	bool bPlayAgainButtonActive = false;
	/* End Game Loop*/

	/* Debug Console Commands */
	void DebugChangeHUD(FName NewScreen);
	/* Debug Console Commands */

	UFUNCTION()
	void OnCloseScroll();
	UFUNCTION()
	void OnOpenScroll();
};
