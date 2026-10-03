// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

#include "CharacterCustomizeWidget.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "PotProbGameState.h"
#include "PotProbPlayerController.h"
#include "PotProbZDCharacter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PotProbPlayerState.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "PotionWidget.h"
#include "RecipeWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "AlertWidget.h"
#include "PotProbSettingsSaveGame.h"
#include "PotProbSettingsSubsystem.h"
#include "TutorialWidget.h"
#include "Components/Slider.h"
#include "Components/WidgetSwitcher.h"
#include "PlayerRoleWidget.h"
#include "LobbyStatusWidget.h"
#include "TroublemakersVotedWidget.h"
#include "AkGameplayStatics.h"
#include "Components/BackgroundBlur.h"


void UHUDWidget::UpdateLobbyStatus()
{
	if (APotProbGameState* ProbGameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
	{
		if (ProbGameState->CurrentPhase != EPotProbPhases::PHASE_LOBBY)
		{
			LobbyStatus->SetText(FText::FromString(TEXT("Lobby: the game is started")));
			ReadyButton->SetVisibility(ESlateVisibility::Collapsed);
			CharacterCustomizeButton->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			if (ProbGameState->bIsCountdownActive)
			{
				LobbyStatus->SetText(FText::FromString(FString::Printf(
					TEXT("Lobby: the game starts in %.0f seconds"), ProbGameState->RemainingCountdownTime)));
			}
			else
			{
				int NumReadied = 0;
				int NumPlayersWaitingFor = ProbGameState->MinPlayers - ProbGameState->NumPlayers;
				FString LobbyStatusString;
				if (NumPlayersWaitingFor > 0)
				{
					LobbyStatusString.Append(FString::Printf(TEXT("Lobby: waiting for %d player(s)\n"), NumPlayersWaitingFor));
				} else
				{
					LobbyStatusString.Append(FString::Printf(TEXT("Lobby: waiting for players to be ready\n")));
				}
				TArray<FString> PlayerNamesInGame;
				for (const auto& PlayerState : ProbGameState->PlayerArray)
				{
					if (APotProbPlayerState* Player = Cast<APotProbPlayerState>(PlayerState))
					{
						PlayerNamesInGame.Add(Player->GetPlayerName());
						if (LobbyStatusWidgets.Contains(*Player->GetPlayerName()))
						{
							LobbyStatusWidgets[*Player->GetPlayerName()]->SetActive(Player->bIsReady);
							if (Player->bIsReady)
							{
								++NumReadied;
							}
						}
						else
						{
							ULobbyStatusWidget* NewLobbyStatusWidget = CreateWidget<ULobbyStatusWidget>(this, LobbyStatusWidgetClass);
							NewLobbyStatusWidget->SetName(Player->GetPlayerName());
							LobbyStatusNamesBox->AddChildToVerticalBox(NewLobbyStatusWidget);
							//NewLobbyStatusWidget->SetActive(Player->bIsReady);
							LobbyStatusWidgets.Add(*Player->GetPlayerName(),NewLobbyStatusWidget);
						}
						//LobbyStatusString.Append(FString::Printf(TEXT("%s: %s\n"), *Player->GetPlayerName(), Player->bIsReady ? TEXT("Ready") : TEXT("Not Ready")));
					}
				}
				for (auto CurrWidget : LobbyStatusWidgets)
				{
					if (!PlayerNamesInGame.Contains(CurrWidget.Key))
					{
						CurrWidget.Value->RemoveFromParent();
						LobbyStatusWidgets.Remove(CurrWidget.Key);
					}
				}
				FText WaitingForPlayerString = FText::FromString(LobbyStatusString);

				// New Lobby Status Numbers
				TotalNumPlayers->SetText(FText::AsNumber( ProbGameState->NumPlayers));
				CurrentNumPlayers->SetText(FText::AsNumber(NumReadied));
				LobbyStatus->SetText(WaitingForPlayerString);
			}
		}
	}
}

void UHUDWidget::CheapShotForIngredientDisplay()
{
	if (APotProbPlayerState* PPS = Cast<APotProbPlayerState>(GetOwningPlayerState()))
	{
		if (PPS->GetIsHoldingIngredient() && DropIngredientText->GetVisibility() == ESlateVisibility::Hidden)
		{
			DropIngredientText->SetVisibility(ESlateVisibility::HitTestInvisible);
			DropIngredientBanner->SetVisibility(ESlateVisibility::HitTestInvisible);
			DropIngredientBind->SetVisibility(ESlateVisibility::HitTestInvisible);
			DropIngredientImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else if (!PPS->GetIsHoldingIngredient() && DropIngredientText->GetVisibility() == ESlateVisibility::HitTestInvisible)
		{
			DropIngredientText->SetVisibility(ESlateVisibility::Hidden);
			DropIngredientBanner->SetVisibility(ESlateVisibility::Hidden);
			DropIngredientBind->SetVisibility(ESlateVisibility::Hidden);
			DropIngredientImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

// Add a new message to the queue
void UHUDWidget::AddAlertSignalMessage(const EPotProbAlertTypes& AlertType, const FString& MessageContent)
{
	if (AlertType == EPotProbAlertTypes::HIGH_PRIORITY)
	{
		// Add important message to the queue
		HighPriorityAlertMessageQueue.Empty();
		HighPriorityAlertMessageQueue.Enqueue(MessageContent);

		// Start processing if not already running
		/* remove queue functionality - new alerts now override old ones
		if (!bIsProcessing_HighPriorityQueue)
		{
			HighAlertWidgetInstance = nullptr;
			bIsProcessing_HighPriorityQueue = true;
			ProcessHighPriorityAlertMessageQueue();
		}
		*/

		HighAlertWidgetInstance = nullptr;
		bIsProcessing_HighPriorityQueue = true;
		ProcessHighPriorityAlertMessageQueue();
	}
	else if (AlertType == EPotProbAlertTypes::LOW_PRIORITY)
	{
		// Add unimportant message to the queue
		LowPriorityAlertMessageQueue.Empty();
		LowPriorityAlertMessageQueue.Enqueue(MessageContent);
		// Start processing if not already running
		/* remove queue functionality - new alerts now override old ones
		if (!bIsProcessing_LowPriorityQueue)
		{
			UAlertWidget* AlertWidgetInstance = nullptr;
			bIsProcessing_LowPriorityQueue = true;
			ProcessLowPriorityAlertMessageQueue();
		}
		*/

		UAlertWidget* AlertWidgetInstance = nullptr;
		bIsProcessing_LowPriorityQueue = true;
		ProcessLowPriorityAlertMessageQueue();
	}
	else if (AlertType == EPotProbAlertTypes::TUTORIAL_ALERT)
	{
		// Add unimportant message to the queue
		TutorialAlertMessageQueue.Enqueue(MessageContent);
		// Start processing if not already running
		if (!bIsProcessing_TutorialPriorityQueue)
		{
			UAlertWidget* AlertWidgetInstance = nullptr;
			bIsProcessing_TutorialPriorityQueue = true;
			ProcessTutorialAlertMessageQueue();
		}
	}

	// Separation of Alerts and HUD
	// HUD:
	// 1. Queues the alerts
	// 2. Sets their lifetimeduration
	// 3. Places spawn location of alert as High or Low priority
	// 
	// Alert:
	// 1. Displays graphic
	// 2. Plays sound

}

void UHUDWidget::ProcessLowPriorityAlertMessageQueue()
{
	APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// if Alert widget does not exist, create one and update its values
	if (!LowAlertWidgetInstance)
	{
		LowAlertWidgetInstance = Cast<UAlertWidget>(CreateWidget<UUserWidget>(PlayerController, AlertWidgetClass));
		UCanvasPanelSlot* CanvasSlot = LowPriorityAlertCanvas->AddChildToCanvas(LowAlertWidgetInstance);
	}

	FString AlertMessage;
	if (LowPriorityAlertMessageQueue.Dequeue(AlertMessage)) // Get the next message in the queue
	{	
		// Set a timer to process the next message after a delay
		GetWorld()->GetTimerManager().SetTimer(
			LowPriorityAlertMessageTimerHandle,
			this,
			&UHUDWidget::ProcessLowPriorityAlertMessageQueue,
			AlertLifetimeDuration,
			false
		);
		// Set the Alert up
		LowAlertWidgetInstance->InitializeAlert(AlertMessage, AlertLifetimeDuration);

	}
	else
	{
		// No more messages in the queue
		bIsProcessing_LowPriorityQueue = false;
		// if the queue is empty, clear the alert widget off the HUDWidget
		LowAlertWidgetInstance->ClearAlertWidget();
		LowAlertWidgetInstance = nullptr;
	}
}

void UHUDWidget::ProcessHighPriorityAlertMessageQueue()
{
	APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// if Alert widget does not exist, create one and update its values
	if (!HighAlertWidgetInstance)
	{
		HighAlertWidgetInstance = Cast<UAlertWidget>(CreateWidget<UUserWidget>(PlayerController, AlertWidgetClass));
		UCanvasPanelSlot* CanvasSlot = HighPriorityAlertCanvas->AddChildToCanvas(HighAlertWidgetInstance);
	}

	FString AlertMessage;
	if (HighPriorityAlertMessageQueue.Dequeue(AlertMessage)) // Get the next message in the queue
	{
		// Set a timer to process the next message after a delay
		GetWorld()->GetTimerManager().SetTimer(
			HighPriorityAlertMessageTimerHandle,
			this,
			&UHUDWidget::ProcessHighPriorityAlertMessageQueue,
			AlertLifetimeDuration,
			false
		);
		// Set the Alert up
		HighAlertWidgetInstance->InitializeAlert(AlertMessage, AlertLifetimeDuration);
	}
	else
	{
		// No more messages in the queue
		bIsProcessing_HighPriorityQueue = false;
		// if the queue is empty, clear the alert widget off the HUDWidget
		HighAlertWidgetInstance->ClearAlertWidget();
		HighAlertWidgetInstance = nullptr;
	}
}

void UHUDWidget::ProcessTutorialAlertMessageQueue()
{
	APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// if Alert widget does not exist, create one and update its values
	if (!TutorialAlertWidgetInstance)
	{
		TutorialAlertWidgetInstance = Cast<UAlertWidget>(CreateWidget<UUserWidget>(PlayerController, TutorialAlertWidgetClass));
		UCanvasPanelSlot* CanvasSlot = TutorialAlertCanvas->AddChildToCanvas(TutorialAlertWidgetInstance);
	}

	FString AlertMessage;
	if (TutorialAlertMessageQueue.Dequeue(AlertMessage)) // Get the next message in the queue
	{	
		// Set a timer to process the next message after a delay
		GetWorld()->GetTimerManager().SetTimer(
			TutorialAlertMessageTimerHandle,
			this,
			&UHUDWidget::ProcessTutorialAlertMessageQueue,
			TutorialAlertLifetimeDuration,
			false
		);
		// Set the Alert up
		TutorialAlertWidgetInstance->InitializeAlert(AlertMessage, TutorialAlertLifetimeDuration);
	}
	else
	{
		// No more messages in the queue
		bIsProcessing_TutorialPriorityQueue = false;
		// if the queue is empty, clear the alert widget off the HUDWidget
		TutorialAlertWidgetInstance->ClearAlertWidget();
		TutorialAlertWidgetInstance = nullptr;
	}
}

/*void UHUDWidget::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	ChatTextBox->SetText(FText::GetEmpty());
	APotProbPlayerController* MultiController = Cast<APotProbPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	APotProbGameState* PotProbGameState = Cast<APotProbGameState>(GetWorld()->GetGameState());
	if (PotProbGameState)
	{
		if (PotProbGameState->CurrentPhase == EPotProbPhases::PHASE_VOTE)
		{
			UE_LOG(LogTemp, Warning, TEXT("Warning: Voting phase is active. Giving focus back to UI."));
			MultiController->Client_ToggleVotingUI(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Warning: Voting phase is not active. Giving focus back to game."));
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(MultiController);
		}
	}

	if (CommitMethod == ETextCommit::OnEnter)
	{
		APotProbPlayerState* PlayerState = MultiController->GetPlayerState<APotProbPlayerState>();

		if (!PlayerState)
		{
			UE_LOG(LogTemp, Error,
			       TEXT("Warning: PlayerState for PotionPlayerController is not APotProbPlayerState. HUDWidget Line 43."
			       ));
			return;
		}

		// Chat-enabled debug commands
		const FString& Command = Text.ToString();
		if (Command.Equals(TEXT("/startvoting"), ESearchCase::IgnoreCase))
		{
			MultiController->Server_StartVotingPhase();
			return;
		}
		if (Command.Equals(TEXT("/togglefrogged"), ESearchCase::IgnoreCase))
		{
			PlayerState->SetIsFrogged(!PlayerState->bIsFrogged, true);
			return;
		}
		if (Command.Equals(TEXT("/toggleready"), ESearchCase::IgnoreCase))
		{
			PlayerState->Server_SetReady(!PlayerState->bIsReady);
			return;
		}
		if (Command.Equals(TEXT("/togglerole"), ESearchCase::IgnoreCase))
		{
			EPotProbRoles NewRole = PlayerState->PotProbRole == EPotProbRoles::ROLE_APPRENTICE
				                        ? EPotProbRoles::ROLE_TROUBLEMAKER
				                        : EPotProbRoles::ROLE_APPRENTICE;
			PlayerState->Server_SetRole(NewRole);
			MultiController->Client_ShowPlayerStartRole(NewRole, PotProbGameState->NumTroublemakers);
			return;
		}
		if (Command.Equals(TEXT("/startgame"), ESearchCase::IgnoreCase))
		{
			MultiController->DEBUG_Server_StartGame();
			return;
		}
		if (Command.Equals(TEXT("/craftedpotion"), ESearchCase::IgnoreCase))
		{
			MultiController->DEBUG_Server_AddApprenticeCraftedPotionByOne();
			return;
		}
		// End chat-enabled debug commands

		//PlayerState->ServerReceiveChatMessage(PlayerState->GetPlayerName(), Text.ToString());
	}
}*/

void UHUDWidget::CauldronMessage()
{
	CauldronSwitchMessage->SetVisibility(ESlateVisibility::HitTestInvisible);
	GetWorld()->GetTimerManager().SetTimer(TimerSwitcharooMessage,this, &UHUDWidget::EndCauldronMessage, 5.0f) ;
}

void UHUDWidget::EndCauldronMessage()
{
	CauldronSwitchMessage->SetVisibility(ESlateVisibility::Hidden);
}

void UHUDWidget::DisplayDropIngredientText(bool bVal)
{
	if (bVal)
	{
		DropIngredientText->SetVisibility(ESlateVisibility::HitTestInvisible);
		DropIngredientImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		DropIngredientBanner->SetVisibility(ESlateVisibility::HitTestInvisible);
		DropIngredientBind->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		DropIngredientText->SetVisibility(ESlateVisibility::Hidden);
		DropIngredientImage->SetVisibility(ESlateVisibility::Hidden);
		DropIngredientBanner->SetVisibility(ESlateVisibility::Hidden);
		DropIngredientBind->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool UHUDWidget::RemovePotionWidget()
{
	if (PotionWidgetInstance)
	{
		PotionWidgetInstance->DestroyWidget();
		PotionWidgetInstance = nullptr;
		return true;
	}
	return false;
}

void UHUDWidget::ToggleDrinkText(bool bEffectActive) const
{
	if (bEffectActive)
	{
		DrinkText->SetText(FText::FromString("Cancel Effect"));
	}
	else
	{
		DrinkText->SetText(FText::FromString("Drink Potion"));
	}
}

void UHUDWidget::DEBUG_AddApprenticeCraftedPotionByOne()
{
	if (APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		PlayerController->DEBUG_Server_AddApprenticeCraftedPotionByOne();
	}
}

void UHUDWidget::UpdateEndGameScreen(bool bApprenticesWin, bool bWonByVoting)
{
	TArray<USizeBox*> ApprenticeSizeboxes = {
		ApprenticeSizebox1, ApprenticeSizebox4, ApprenticeSizebox2,
		ApprenticeSizebox5, ApprenticeSizebox3, ApprenticeSizebox6
	};
	TArray<USizeBox*> TroublemakerSizeboxes = {
		TroublemakerSizebox1, TroublemakerSizebox2, TroublemakerSizebox3
	};
	TArray<USizeBox*> AllApprenticeSizeboxes = { AllApprenticeSizebox1, AllApprenticeSizebox2, 
		AllApprenticeSizebox3, AllApprenticeSizebox4, AllApprenticeSizebox5, AllApprenticeSizebox6, AllApprenticeSizebox7
	};
	
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetOwningPlayer()))
	{
		if (APotProbPlayerState* PS = PC->GetPlayerState<APotProbPlayerState>())
		{
			if (bApprenticesWin)
			{
				if (PS->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
				{
					SetActiveWidgetByName(WSEndGame, "EndGameApprenticeWinAsTroublemaker");
					if (bWonByVoting)
					{
						ApprenticeWinReasonTextTroublemaker2->SetVisibility(ESlateVisibility::Visible);
						ApprenticeWinReasonTextTroublemaker1->SetVisibility(ESlateVisibility::Hidden);
					}
					else
					{
						ApprenticeWinReasonTextTroublemaker1->SetVisibility(ESlateVisibility::Visible);
						ApprenticeWinReasonTextTroublemaker2->SetVisibility(ESlateVisibility::Hidden);
					}
					if (APotProbGameState* ProbGameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
					{
						int i = 0;
						for (const auto& PlayerState : ProbGameState->PlayerArray)
						{
							if (APotProbPlayerState* Player = Cast<APotProbPlayerState>(PlayerState))
							{
									if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(Player->GetPawn()))
									{
										if (Player->PotProbRole == EPotProbRoles::ROLE_APPRENTICE)
										{
											UImage* NewPlayerSprite = NewObject<UImage>(this);
											NewPlayerSprite->SetBrushFromTexture(Cast<UTexture2D>(CharacterTextures[PlayerCharacter->GetPlayerSelectedModel()]));
											AllApprenticeSizeboxes[i]->AddChild(NewPlayerSprite);
											i++;
										}
									}
							}
						}
					}
				}
				else
				{
					SetActiveWidgetByName(WSEndGame, "EndGameApprenticeWin");
					if (bWonByVoting)
					{
						ApprenticeWinReasonText2->SetVisibility(ESlateVisibility::Visible);
						ApprenticeWinReasonText1->SetVisibility(ESlateVisibility::Hidden);
					}
					else
					{
						ApprenticeWinReasonText1->SetVisibility(ESlateVisibility::Visible);
						ApprenticeWinReasonText2->SetVisibility(ESlateVisibility::Hidden);
					}
					if (APotProbGameState* ProbGameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
					{
						int i = 0;
						for (const auto& PlayerState : ProbGameState->PlayerArray)
						{
							if (APotProbPlayerState* Player = Cast<APotProbPlayerState>(PlayerState))
							{
									if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(Player->GetPawn()))
									{
										if (Player == PS)
										{
											ApprenticePlayerSprite->SetBrushFromTexture(Cast<UTexture2D>(CharacterTextures[PlayerCharacter->GetPlayerSelectedModel()]));
										}
										else if (Player->PotProbRole == EPotProbRoles::ROLE_APPRENTICE)
										{
											UImage* NewPlayerSprite = NewObject<UImage>(this);
											NewPlayerSprite->SetBrushFromTexture(Cast<UTexture2D>(CharacterTextures[PlayerCharacter->GetPlayerSelectedModel()]));
											ApprenticeSizeboxes[i]->AddChild(NewPlayerSprite);
											i++;
										}
									}
							}
						}
					}
				}
			}
			else
			{
				SetActiveWidgetByName(WSEndGame, "EndGameTroublemakerWin");
				if (APotProbGameState* ProbGameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
				{
					int i = 0;
					for (const auto& PlayerState : ProbGameState->PlayerArray)
					{
						if (APotProbPlayerState* Player = Cast<APotProbPlayerState>(PlayerState))
						{
								if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(PlayerState->GetPawn()))
								{
									if (Player->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
									{
										UImage* NewPlayerSprite = NewObject<UImage>(this);
										NewPlayerSprite->SetBrushFromTexture(Cast<UTexture2D>(CharacterTextures[PlayerCharacter->GetPlayerSelectedModel()]));
										TroublemakerSizeboxes[i]->AddChild(NewPlayerSprite);
										i++;
									}
								}
						}
					}
				}
			}
		}
	}
}

void UHUDWidget::IncrementHUDGameState()
{
	int index = WSGameStates->GetActiveWidgetIndex();
	if (++index >= WSGameStates->GetNumWidgets())
	{
		index = 0;
	}
	WSGameStates->SetActiveWidgetIndex(index);
}

void UHUDWidget::DisplayEndScreen()
{
	int EndGameIndex = WSGameStates->GetNumWidgets()-1;
	WSGameStates->SetActiveWidgetIndex(EndGameIndex);
}

void UHUDWidget::IncrementEndGameScreen()
{
	int index = WSEndGame->GetActiveWidgetIndex();
	if (++index >= WSEndGame->GetNumWidgets())
	{
		// Make Continue Button Disappear
		EndGameContinueButton->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	WSEndGame->SetActiveWidgetIndex(index);
}

void UHUDWidget::DecrementEndGameScreen()
{
	int index = WSEndGame->GetActiveWidgetIndex();
	if (--index < 0)
	{
		return;
	}
	WSEndGame->SetActiveWidgetIndex(index);
}

void UHUDWidget::SetHUDPlayerType(bool bIsTroublemaker)
{
	if (!bIsTroublemaker)
	{
		SetActiveWidgetByName(WSPlayerType, "HUDApprentice");
	}
	else
	{
		SetActiveWidgetByName(WSPlayerType, "HUDTroublemaker");
	}
}

void UHUDWidget::SetActiveWidgetByName(UWidgetSwitcher* WidgetSwitcher, const FString& WidgetName)
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

void UHUDWidget::NativeConstruct()
{
	ReadyButton = ReadyButtonAnim -> ButtonAnim;
	CloseButton = CloseButtonAnim->ButtonAnim;
	LobbyButton = LobbyButtonAnim->ButtonAnim;
	CharacterCustomizeButton = CharacterCustomizeButtonAnim->ButtonAnim;
	ExitGameButton = ExitGameButtonAnim->ButtonAnim;
	QuitGameButton = QuitGameButtonAnim->ButtonAnim;
	PlayAgainButton = PlayAgainButtonAnim->ButtonAnim;
	EndGameContinueButton = EndGameContinueButtonAnim->ButtonAnim;
	ResumeButton = ResumeButtonAnim->ButtonAnim;
	ControlsButton = ControlsButtonAnim->ButtonAnim;
	ExitButton = ExitButtonAnim->ButtonAnim;
	ControlsToSettingsButton = ControlsToSettingsButtonAnim->ButtonAnim;
	TutorialToggle = TutorialToggleAnim->ButtonAnim;

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UHUDWidget::OnCloseScroll);
	}
	Super::NativeConstruct();
	MapButton->OnClicked.AddDynamic(this, &UHUDWidget::OnMapButtonClicked);
	CloseMapButton->OnClicked.AddDynamic(this, &UHUDWidget::OnCloseButtonClicked);
	ReadyButton->OnClicked.AddDynamic(this, &UHUDWidget::OnReadyButtonClicked);
	CharacterCustomizeButton->OnClicked.AddDynamic(this, &UHUDWidget::OnChracterCustomizeButtonClicked);
	OpenTutorialButton->OnClicked.AddDynamic(this, &UHUDWidget::OnOpenTutorialButtonClicked);
	SettingsToMainMenuXButton->OnClicked.AddDynamic(this, &UHUDWidget::OnSettingsToMainMenuXButtonClicked);
	TutorialToggle->OnClicked.AddDynamic(this, &UHUDWidget::OnTutorialToggleClicked);
	ResumeButton->OnClicked.AddDynamic(this, &UHUDWidget::OnResumeButtonClicked);
	ResumeFromControls->OnClicked.AddDynamic(this, &UHUDWidget::OnResumeButtonClicked);
	ControlsButton->OnClicked.AddDynamic(this, &UHUDWidget::OnControlsButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &UHUDWidget::OnExitButtonClicked);
	MasterVolumeSlider->OnValueChanged.AddDynamic(this, &UHUDWidget::OnMasterVolumeSliderValueChanged);
	MusicSlider->OnValueChanged.AddDynamic(this, &UHUDWidget::OnMusicSliderValueChanged);
	SFXSlider->OnValueChanged.AddDynamic(this, &UHUDWidget::OnSFXSliderValueChanged);
	ControlsToSettingsButton->OnClicked.AddDynamic(this, &UHUDWidget::OnControlsToSettingsButtonClicked);
	SettingsButton->OnClicked.AddDynamic(this, &UHUDWidget::OnSettingsButtonClicked);
	LobbyButton->OnClicked.AddDynamic(this, &UHUDWidget::OnLobbyButtonClicked);
	ExitGameButton->OnClicked.AddDynamic(this, &UHUDWidget::OnLobbyButtonClicked);
	QuitGameButton->OnClicked.AddDynamic(this, &UHUDWidget::OnQuitGameButtonClicked);
	PlayAgainButton->OnClicked.AddDynamic(this, &UHUDWidget::OnPlayAgainButtonClicked);
	WASDButton->OnClicked.AddDynamic(this, &UHUDWidget::OnControlsButtonClicked);
	EndGameContinueButton->OnClicked.AddDynamic(this, &UHUDWidget::OnEndGameContinueButtonClicked);
	CloseButton->OnClicked.AddDynamic(this, &UHUDWidget::OnCloseButtonClicked);
	
	SplashText->SetVisibility(ESlateVisibility::Hidden);
	DrinkText->SetVisibility(ESlateVisibility::Hidden);
	DrinkPotionBanner->SetVisibility(ESlateVisibility::Hidden);
	DrinkPotionCircle->SetVisibility(ESlateVisibility::Hidden);
	DrinkPotionBind->SetVisibility(ESlateVisibility::Hidden);
	//DrinkPotionCircle->SetVisibility(ESlateVisibility::Hidden);
	DrinkPotionBanner->SetVisibility(ESlateVisibility::Hidden);
	DrinkPotionBind->SetVisibility(ESlateVisibility::Hidden);
	PotionNameText->SetVisibility(ESlateVisibility::Hidden);
	//RecipeDetailText->SetVisibility(ESlateVisibility::Hidden);
	//RecipeDetailArrow->SetVisibility(ESlateVisibility::Hidden);
	//MovementText->SetVisibility(ESlateVisibility::Hidden);
	//MovementSprite->SetVisibility(ESlateVisibility::Hidden);
	FrogRecipeInfo->SetVisibility(ESlateVisibility::Collapsed);
	
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
		UAkGameplayStatics::SetRTPCValue(MasterVolumeRTPC, SettingsSubsystem->GetCurrentSettings()->MasterVolume, 0, nullptr);
		MusicSlider->SetValue(SettingsSubsystem->GetCurrentSettings()->MusicVolume);
		MusicText->SetText(FText::FromString(FString::FromInt(SettingsSubsystem->GetCurrentSettings()->MusicVolume)));
		UAkGameplayStatics::SetRTPCValue(MusicVolumeRTPC, SettingsSubsystem->GetCurrentSettings()->MusicVolume, 0, nullptr);
		SFXSlider->SetValue(SettingsSubsystem->GetCurrentSettings()->SFXVolume);
		SFXText->SetText(FText::FromString(FString::FromInt(SettingsSubsystem->GetCurrentSettings()->SFXVolume)));
		UAkGameplayStatics::SetRTPCValue(SFXVolumeRTPC, SettingsSubsystem->GetCurrentSettings()->SFXVolume, 0, nullptr);
	}

	TArray<FCharacterData*> Rows;
	static const FString ContextString(TEXT("Character Data Context"));
	CharacterDataTable->GetAllRows<FCharacterData>(ContextString, Rows);

	for (const FCharacterData* Row : Rows)
	{
		if (Row)
		{
			CharacterTextures.Add(Row->CharacterModel, Row->CharacterTexture);
		}
	}
	
	CauldronIcons.Add(CauldronIcon1);
	CauldronIcons.Add(CauldronIcon2);
	CauldronIcons.Add(CauldronIcon3);
	CauldronIcons.Add(CauldronIcon4);
	CauldronIcons.Add(CauldronIcon5);
	CauldronIcons.Add(CauldronIcon6);
	CauldronIcons.Add(CauldronIcon7);
	CauldronIcons.Add(CauldronIcon8);
	CauldronIcons.Add(CauldronIcon9);
	CauldronIcons.Add(CauldronIcon10);

}

void UHUDWidget::NativeDestruct()
{
	MapButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnMapButtonClicked);
	CloseMapButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnCloseButtonClicked);
	ReadyButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnReadyButtonClicked);
	CharacterCustomizeButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnChracterCustomizeButtonClicked);
	SettingsToMainMenuXButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnSettingsToMainMenuXButtonClicked);
	TutorialToggle->OnClicked.RemoveDynamic(this, &UHUDWidget::OnTutorialToggleClicked);
	ResumeButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnResumeButtonClicked);
	ResumeFromControls->OnClicked.RemoveDynamic(this, &UHUDWidget::OnResumeButtonClicked);
	ControlsButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnControlsButtonClicked);
	ExitButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnExitButtonClicked);
	MasterVolumeSlider->OnValueChanged.RemoveDynamic(this, &UHUDWidget::OnMasterVolumeSliderValueChanged);
	MusicSlider->OnValueChanged.RemoveDynamic(this, &UHUDWidget::OnMusicSliderValueChanged);
	SFXSlider->OnValueChanged.RemoveDynamic(this, &UHUDWidget::OnSFXSliderValueChanged);
	ControlsToSettingsButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnControlsToSettingsButtonClicked);
	SettingsButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnSettingsButtonClicked);
	LobbyButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnLobbyButtonClicked);
	ExitGameButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnLobbyButtonClicked);
	QuitGameButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnQuitGameButtonClicked);
	PlayAgainButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnPlayAgainButtonClicked);
	WASDButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnControlsButtonClicked);
	EndGameContinueButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnEndGameContinueButtonClicked);
	CloseButton->OnClicked.RemoveDynamic(this, &UHUDWidget::OnCloseButtonClicked);

	Super::NativeDestruct();

	LobbyStatusWidgets.Empty();
}
void UHUDWidget::OnCloseScroll()
{
	PigeonPoint->SetVisibility(ESlateVisibility::Collapsed);
	CloseButton->SetVisibility(ESlateVisibility::Collapsed);
	ScrollCloseText->SetVisibility(ESlateVisibility::Collapsed);
	Blur->SetVisibility(ESlateVisibility::Collapsed);
}

void UHUDWidget::OnOpenScroll()
{
	PigeonPoint->SetVisibility(ESlateVisibility::Visible);
	CloseButton->SetVisibility(ESlateVisibility::Visible);
	ScrollCloseText->SetVisibility(ESlateVisibility::Visible);
	Blur->SetVisibility(ESlateVisibility::Visible);
}

void UHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (APotProbGameState* ProbGameState = GetWorld()->GetGameState<APotProbGameState>())
	{
		if (ProbGameState->CurrentPhase == EPotProbPhases::PHASE_INGAME)
		{
			// Update Progress Bar
			UpdateProgressBar();
			CheapShotForIngredientDisplay();
		}
	}
}

void UHUDWidget::OnOpenTutorialButtonClicked()
{
	// show starting player role widget
	if (APotProbGameState* GS = Cast<APotProbGameState>(GetWorld()->GetGameState()))
	{
		// show pigeonpoint scroll if in lobby
		if (GS->CurrentPhase == EPotProbPhases::PHASE_LOBBY)
		{
			OnOpenScroll();
			return;
		}
		// show player role widget if in game
		if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetOwningPlayer()))
		{
			if (APotProbPlayerState* PS = PC->GetPlayerState<APotProbPlayerState>())
			{
				PC->Client_ShowPlayerStartRole(PS->PotProbRole, GS->NumTroublemakers);
			}
		}
	}
}

void UHUDWidget::OnSettingsButtonClicked()
{
	SetActiveWidgetByName(WSHUDWidget, "InGameSettings");
}

void UHUDWidget::OnSettingsToMainMenuXButtonClicked()
{
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->SaveSettings();
	}
	SetActiveWidgetByName(WSHUDWidget, "GameOverlay");
}

void UHUDWidget::OnTutorialToggleClicked()
{
	if (bIsTutorialOn)
	{
		bIsTutorialOn = false;
		// ShowTutorialText->SetText(FText::FromString("Off"));
	}
	else
	{
		bIsTutorialOn = true;
		// ShowTutorialText->SetText(FText::FromString("On"));
	}
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->GetCurrentSettings()->DisplayTutorial = bIsTutorialOn;
	}
}

void UHUDWidget::OnResumeButtonClicked()
{
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->SaveSettings();
	}
	SetActiveWidgetByName(WSHUDWidget, "GameOverlay");
}

void UHUDWidget::OnControlsButtonClicked()
{
	SetActiveWidgetByName(WSHUDWidget, "Controls");
}

void UHUDWidget::OnExitButtonClicked()
{
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->SaveSettings();
	}
	APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		PlayerController->LeaveSessionAndReturnToMenu();
	}
}

void UHUDWidget::OnMasterVolumeSliderValueChanged(float NewValue)
{
	UAkGameplayStatics::SetRTPCValue(MasterVolumeRTPC, NewValue, 0, nullptr);
	VolumeText->SetText(FText::FromString(FString::FromInt(NewValue)));
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->GetCurrentSettings()->MasterVolume = NewValue;
	}
}

void UHUDWidget::OnMusicSliderValueChanged(float NewValue)
{
	UAkGameplayStatics::SetRTPCValue(MusicVolumeRTPC, NewValue, 0, nullptr);
	MusicText->SetText(FText::FromString(FString::FromInt(NewValue)));
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->GetCurrentSettings()->MusicVolume = NewValue;
	}
}

void UHUDWidget::OnSFXSliderValueChanged(float NewValue)
{
	UAkGameplayStatics::SetRTPCValue(SFXVolumeRTPC, NewValue, 0, nullptr);
	SFXText->SetText(FText::FromString(FString::FromInt(NewValue)));
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->GetCurrentSettings()->SFXVolume = NewValue;
	}
}

void UHUDWidget::OnControlsToSettingsButtonClicked()
{
	SetActiveWidgetByName(WSHUDWidget, "InGameSettings");
}

void UHUDWidget::OnControlTipButtonClicked()
{
	if (ControlTips->GetVisibility() == ESlateVisibility::Hidden)
	{
		ControlTips->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		ControlTips->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UHUDWidget::AddTutorialFlipBookToViewport()
{
	if (TutorialClass)
	{
		UTutorialWidget* Tutorial = CreateWidget<UTutorialWidget>(GetWorld(), TutorialClass);
		Tutorial->SetWidgetInstigator(this);
		Tutorial->AddToViewport();
	}
}

void UHUDWidget::UpdateMapText(FString MapText)
{
	MapDetails->SetText(FText::FromString(MapText));
}

void UHUDWidget::OnEndGameContinueButtonClicked()
{
	SetActiveWidgetByName(WSEndGame, "EndGameLoop");
	
	EndGameContinueButton->SetVisibility(ESlateVisibility::Hidden);

	TArray<USizeBox*> VotedTroublemakerSizeboxes = {
		VotedTroublemakerSizebox1, VotedTroublemakerSizebox2, VotedTroublemakerSizebox3
	};

	if (APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		if (APotProbPlayerState* PlayerState = PlayerController->GetPlayerState<APotProbPlayerState>())
		{
			if (PlayerState->PotProbRole == EPotProbRoles::ROLE_APPRENTICE)
			{
				SetActiveWidgetByName(WSEndGameLoopStats, "EndGameApprenticeStats");
				ApprenticeStatsPlayerNameText->SetText(FText::FromString(PlayerState->GetPlayerName()));
				PotionsCreatedSlider->SetValue(PlayerState->NumPotionsCrafted);
				NumOfPotionsCreatedText->SetText(FText::FromString(FString::Printf(TEXT("%d"), PlayerState->NumPotionsCrafted)));
				if (APotProbGameState* ProbGameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
				{
					int TotalPotionsRequired = ProbGameState->NumApprentices * ProbGameState->GetApprenticeWinMultiplier();
					TotalNumOfPotionsCreatedText->SetText(FText::FromString(FString::Printf(TEXT("%d"), TotalPotionsRequired)));
					if (TotalPotionsRequired > 0)
					{
						PotionsCreatedSlider->SetMaxValue(TotalPotionsRequired);
						//NumOfPotionsCreatedText->SetRenderTranslation(FVector2D(PotionsCreatedSlider->GetRenderTransform().Translation.X + (1081 * (PlayerState->NumPotionsCrafted / TotalPotionsRequired)) + 45, NumOfPotionsCreatedText->GetRenderTransform().Translation.Y));
					}
					else
					{
						PotionsCreatedSlider->SetMaxValue(1);
						//NumOfPotionsCreatedText->SetRenderTranslation(FVector2D(PotionsCreatedSlider->GetRenderTransform().Translation.X + (1081 * (PlayerState->NumPotionsCrafted)) + 45, NumOfPotionsCreatedText->GetRenderTransform().Translation.Y));
					}
					if (TroublemakersVotedClass)
					{
						int i = 0;
						for (const auto& PlayerStates : ProbGameState->PlayerArray)
						{
							if (APotProbPlayerState* Player = Cast<APotProbPlayerState>(PlayerStates))
							{
									if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(Player->GetPawn()))
									{
										if (Player->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
										{
											UTroublemakersVotedWidget* TM = CreateWidget<UTroublemakersVotedWidget>(this, TroublemakersVotedClass);
											if (Player->bWasVotedOut)
											{
												TM->TMCheckImage->SetVisibility(ESlateVisibility::Visible);
											}
											else
											{
												TM->TMCheckImage->SetVisibility(ESlateVisibility::Hidden);
											}
											TM->TroublemakerSpriteImage->SetBrushFromTexture(Cast<UTexture2D>(CharacterTextures[PlayerCharacter->GetPlayerSelectedModel()]));
											VotedTroublemakerSizeboxes[i]->AddChild(TM);
											i++;
										}
									}
							}
						}
					}
				}
				if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(PlayerController->GetCharacter()))
				{
					ApprenticeStatsPlayerSprite->SetBrushFromTexture(Cast<UTexture2D>(CharacterTextures[PlayerCharacter->GetPlayerSelectedModel()]));
				}
			}
			else
			{
				SetActiveWidgetByName(WSEndGameLoopStats, "EndGameTroublemakerStats");
				TroublemakerStatsPlayerNameText->SetText(FText::FromString(PlayerState->GetPlayerName()));
				PlayersFroggedSlider->SetValue(PlayerState->NumApprenticesFrogged);
				NumPlayersFroggedText->SetText(FText::FromString(FString::Printf(TEXT("%d"), PlayerState->NumApprenticesFrogged)));
				VotingRoundsSurvivedSlider->SetValue(PlayerState->NumVotingRoundsSurvived);
				NumVotingRoundsSurvivedText->SetText(FText::FromString(FString::Printf(TEXT("%d"), PlayerState->NumVotingRoundsSurvived)));
				if (APotProbGameState* ProbGameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
				{
					int TotalFrogged = ProbGameState->numApprenticeFrogged + ProbGameState->numTroublemakerFrogged;
					TotalNumOfPlayersFroggedText->SetText(FText::FromString(FString::Printf(TEXT("%d"), TotalFrogged)));
					TotalNumOfRoundsSurvivedText->SetText(FText::FromString(FString::Printf(TEXT("%d"), ProbGameState->TotalVotingRounds)));
					FVector2D CurrentFroggedTextTranslation = NumPlayersFroggedText->GetRenderTransform().Translation;
					FVector2D CurrentVotingRoundsTextTranslation = NumVotingRoundsSurvivedText->GetRenderTransform().Translation;
					if (TotalFrogged > 0)
					{
						PlayersFroggedSlider->SetMaxValue(TotalFrogged);
						//NumPlayersFroggedText->SetRenderTranslation(FVector2D(PlayersFroggedSlider->GetRenderTransform().Translation.X + (1081 * (PlayerState->NumVotingRoundsSurvived / TotalFrogged)) + 45, CurrentFroggedTextTranslation.Y));
					}
					else
					{
						PlayersFroggedSlider->SetMaxValue(TotalFrogged + 1);
						//NumPlayersFroggedText->SetRenderTranslation(FVector2D(PlayersFroggedSlider->GetRenderTransform().Translation.X + (1081 * (PlayerState->NumVotingRoundsSurvived)) + 45, CurrentFroggedTextTranslation.Y));
					}
					if (ProbGameState->TotalVotingRounds > 0)
					{
						VotingRoundsSurvivedSlider->SetMaxValue(ProbGameState->TotalVotingRounds);
						//NumVotingRoundsSurvivedText->SetRenderTranslation(FVector2D(VotingRoundsSurvivedSlider->GetRenderTransform().Translation.X + (1081 * (PlayerState->NumApprenticesFrogged / ProbGameState->TotalVotingRounds)) + 45, CurrentVotingRoundsTextTranslation.Y));
					}
					else
					{
						VotingRoundsSurvivedSlider->SetMaxValue(1);
						///NumVotingRoundsSurvivedText->SetRenderTranslation(FVector2D(VotingRoundsSurvivedSlider->GetRenderTransform().Translation.X + (1081 * (PlayerState->NumApprenticesFrogged)) + 45, CurrentVotingRoundsTextTranslation.Y));
					}
				}
				if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(PlayerController->GetCharacter()))
				{
					TroublemakerStatsPlayerSprite->SetBrushFromTexture(Cast<UTexture2D>(CharacterTextures[PlayerCharacter->GetPlayerSelectedModel()]));
				}
			}
		}
	}
	
	//IncrementEndGameScreen();
}

void UHUDWidget::UpdateProgressBar()
{
	APotProbGameState* PotionGameState = Cast<APotProbGameState>(GetGameInstance()->GetWorld()->GetGameState());
	if (!PotionGameState)
	{
		return;
	}
	
	if (PotionGameState->GetNumberTotalPotionRecipes() == 0)
	{
		return;
	}
	int PotionTotalRecipes = PotionGameState->GetNumberTotalPotionRecipes() > 1
		                           ? PotionGameState->GetNumberTotalPotionRecipes() - 1
		                           : PotionGameState->GetNumberTotalPotionRecipes();


	TargetPercentage = (float)PotionGameState->GetCurrentlyCraftedNumApprenticePotions() / (PotionGameState->NumApprentices * PotionGameState->GetApprenticeWinMultiplier());//cast to potprob and get the num players * mult
	
	
	if (TargetPercentage != PreviousPercentage)
	{
		PreviousPercentage = FMath::InterpEaseOut(PreviousPercentage, TargetPercentage, AlphaValue, DegreeOfLerp);
		PotionCompletionProgress->SetPercent(PreviousPercentage);
	} 

	
	const int PotionNum = PotionGameState->GetCurrentlyCraftedNumApprenticePotions();
	const FString Disp = FString::FromInt(PotionNum);
	const FString DispMaxPot = FString::FromInt((PotionGameState->NumApprentices * PotionGameState->GetApprenticeWinMultiplier()));
	const FString DisplayCompletionText = Disp + FString::Printf(TEXT(" / ")) + DispMaxPot + FString::Printf(TEXT(" Potions Crafted"));
	CompletionProgress->SetText(FText::FromString(DisplayCompletionText));

}

void UHUDWidget::OnMapButtonClicked()
{
	APotProbGameState* PotionGameState = Cast<APotProbGameState>(GetGameInstance()->GetWorld()->GetGameState());
	if (!PotionGameState)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC);
	
	if (!bHasOpenedMap)
	{
		bHasOpenedMap = true;

		Map->SetVisibility(ESlateVisibility::Visible);
		
		FString LevelName = PotionGameState->GetWorld()->GetMapName();
		if (LevelName.Contains(TEXT("MainLevel")))
		{
			MapImage->SetVisibility(ESlateVisibility::HitTestInvisible);
			SmallMapImage->SetVisibility(ESlateVisibility::Collapsed);
		} else
		{
			MapImage->SetVisibility(ESlateVisibility::Collapsed);
			SmallMapImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}


		
		int counter = 0;
		for (AActor* Cauldron : PotionGameState->CauldronActors)
		{
			FVector UIPos = ConvertFromWorldspaceToUISpace(Cauldron->GetActorLocation());
			FVector2D UIPos2D(UIPos.X, UIPos.Y);
			Cast<UCanvasPanelSlot>(CauldronIcons[counter]->Slot)->SetPosition(UIPos2D);
			counter++;
		}
	}
	else
	{
		Map->SetVisibility(ESlateVisibility::Hidden);
		bHasOpenedMap = false;
	}
}

void UHUDWidget::OnCloseButtonClicked()
{
	Map->SetVisibility(ESlateVisibility::Hidden);
	bHasOpenedMap = false;
	
	APlayerController* PC = GetOwningPlayer();
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC);
}

void UHUDWidget::OnReadyButtonClicked()
{
	if (APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		if (APotProbPlayerState* PlayerState = PlayerController->GetPlayerState<APotProbPlayerState>())
		{
			if (PlayerState->bIsReady)
			{
				// If the player is currently ready, pressing the button will make them "Not Ready"
				// Therefore the button text should become "READY!"
				ReadyButtonText->SetText(FText::FromString(FString::Printf(TEXT("READY!"))));
				CharacterCustomizeButton->SetIsEnabled(true);
			} else
			{
				// and vice versa
				ReadyButtonText->SetText(FText::FromString(FString::Printf(TEXT("UNREADY"))));
				CharacterCustomizeButton->SetIsEnabled(false);
			}
			PlayerState->Server_SetReady(!PlayerState->bIsReady);
		}
	}
}


void UHUDWidget::OnChracterCustomizeButtonClicked()
{
	if (APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		if (PlayerController->CharacterCustomizeWidgetInstance)
		{
			PlayerController->CharacterCustomizeWidgetInstance->UpdateCharCustomStatus();
			PlayerController->CharacterCustomizeWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UHUDWidget::CreatePotionWidgetPopup(UPotionObject* Potion)
{
	if (!PotionWidgetInstance && PotionWidgetClass)
	{
		PotionWidgetInstance = CreateWidget<UPotionWidget>(this, PotionWidgetClass);
		PotionWidgetInstance->SetPotionText(Potion);
		PotionWidgetInstance->AddToViewport();
	}
}

void UHUDWidget::UpdatePlayerPositionIcon(FVector PlayerPos)
{
	FVector UIPos = ConvertFromWorldspaceToUISpace(PlayerPos);
	FVector2D UIPos2D(UIPos.X, UIPos.Y);
	Cast<UCanvasPanelSlot>(PlayerLocationImage->Slot)->SetPosition(UIPos2D);
}

FVector UHUDWidget::ConvertFromWorldspaceToUISpace(FVector WorldspacePos)
{
	if (!MapImage)
	{
		return FVector::ZeroVector;
	}

	// Step 1: Calculate the world-to-UI scale factor
	FVector2D MapImageSize = SmallMapImage->GetDesiredSize();
	FVector2D WorldToUIScale;
	WorldToUIScale.X = MapSize.X / MapImageSize.X * 6;
	WorldToUIScale.Y = MapSize.Y / MapImageSize.Y * 5;

	// Step 2: Offset and scale WorldspacePos to map it to UI space
	FVector2D UISpacePos;
	UISpacePos.X = (WorldspacePos.X - MapPosition.X) * WorldToUIScale.X * -1.0;
	UISpacePos.Y = (WorldspacePos.Y - MapPosition.Y) * WorldToUIScale.Y * -1.0;

	// Optional: Log for debugging
	//UE_LOG(LogTemp, Warning, TEXT("WorldspacePos: %s, MapPosition: %s, MapSize: %s"), *WorldspacePos.ToString(), *MapPosition.ToString(), *MapSize.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("MapImageSize: %s, WorldToUIScale: %s"), *MapImageSize.ToString(), *WorldToUIScale.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("UISpacePos: %s"), *UISpacePos.ToString());

	return FVector(UISpacePos.X, UISpacePos.Y, 0.0);
	/*if (!MapImage)
	{
		// Return a zero vector if MapImage is null to avoid a crash
		return FVector::ZeroVector;
	}

	// Calculate the normalized position of the WorldspacePos within the MapPosition and MapSize
	FVector NormalizedPosition;
	NormalizedPosition.X = (WorldspacePos.X - MapPosition.X) / MapSize.X;
	NormalizedPosition.Y = (WorldspacePos.Y - MapPosition.Y) / MapSize.Y;

	// Map the normalized position to the MapImage size (UI space)
	FVector2D MapImageSize = MapImage->GetDesiredSize();
	FVector UISpacePos;
	UISpacePos.X = NormalizedPosition.X * MapImageSize.X;
	UISpacePos.Y = NormalizedPosition.Y * MapImageSize.Y;

	// Set Z to 0 since this is a 2D UI position
	UISpacePos.Z = 0.0f;

	return UISpacePos;*/
}

void UHUDWidget::AddRecipeInformationToHUD(const FRecipeStruct& Recipe)
{
	//PotionRecipeInfo->SetVisibility(ESlateVisibility::HitTestInvisible);
	OpenMapText->SetText(FText::FromString("Open map to find ingredients"));
	//RecipeDetailText->SetVisibility(ESlateVisibility::Visible);
	//RecipeDetailArrow->SetVisibility(ESlateVisibility::Visible);
	//MovementText->SetVisibility(ESlateVisibility::Visible);
	//MovementSprite->SetVisibility(ESlateVisibility::Visible);
	//PotionRecipeInfo->InitializeTextAndIcons(Recipe);
}

void UHUDWidget::AddFrogRecipeInformationToHUD(const FRecipeStruct& FrogRecipe)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		APotProbPlayerState* PS = Cast<APotProbPlayerState>(PlayerController->PlayerState);
		if (PS && PS->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER && FrogRecipe.Potion != nullptr) {
			FrogRecipeInfo->SetVisibility(ESlateVisibility::HitTestInvisible);
			FrogRecipeInfo->InitializeTextAndIcons(FrogRecipe);
		}
	}
}

void UHUDWidget::DisplayCootiesData(bool bShouldDisplay)
{
	CootiesText->SetVisibility(bShouldDisplay? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}


void UHUDWidget::DisplayFrogTunnelMessage()
{
	FrogTunnelText->SetVisibility(ESlateVisibility::Visible); // Show it
	FrogTunnelText->SetText(FText::FromString("Only Frogs Can Fit Here")); // Update text
}

void UHUDWidget::HideFrogTunnelMessage()
{
	FrogTunnelText->SetVisibility(ESlateVisibility::Hidden); // Hide it

}

void UHUDWidget::OnLobbyButtonClicked()
{
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->SaveSettings();
	}
	APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		PlayerController->LeaveSessionAndReturnToMenu();
	}
}

void UHUDWidget::OnQuitGameButtonClicked()
{
	if (UPotProbSettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UPotProbSettingsSubsystem>())
	{
		SettingsSubsystem->SaveSettings();
	}
	APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		PlayerController->LeaveSessionAndCloseGame();
	}
}


void UHUDWidget::OnPlayAgainButtonClicked()
{
	bPlayAgainButtonActive = !bPlayAgainButtonActive;

	if (PlayAgainButtonText)
	{
		if (bPlayAgainButtonActive)
		{
			PlayAgainButtonText->SetText(FText::FromString("CANCEL"));
		}
		else
		{
			PlayAgainButtonText->SetText(FText::FromString("PLAY AGAIN!"));
		}
	}

	// Update Player State Looping Flag
	APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		if (APotProbPlayerState* PlayerState = PlayerController->GetPlayerState<APotProbPlayerState>())
		{
			PlayerState->Server_SetIsPlayingAgain(bPlayAgainButtonActive);
		}
	}
}

void UHUDWidget::UpdateNumInGamePlayers(int NewNumPlayers)
{
	PlayersInGameText->SetText(FText::FromString(FString::FromInt(NewNumPlayers)));

	// Change All Children of Looping Player Text to Red when under minimum required number
	if (APotProbGameState* GameState = GetWorld()->GetGameState<APotProbGameState>())
	{
		if (NewNumPlayers < /*GameState->MinPlayers*/ 4) // TODO: Change back to variable (also ask design)
		{
			for (UWidget* Widget : PlayAgainHorizontal->GetAllChildren())
			{
				if (UTextBlock* Text = Cast<UTextBlock>(Widget))
				{
					Text->SetColorAndOpacity(FColor::Red);
				}
			}
		}
	}
}

void UHUDWidget::UpdateNumLoopingPlayers()
{
	if (APotProbGameState* GameState = GetWorld()->GetGameState<APotProbGameState>())
	{
		if (PlayersReadiedAgainText)
		{
			PlayersReadiedAgainText->SetText(FText::FromString(FString::FromInt(GameState->GetNumLoopingPlayers())));
		}
	}
}

void UHUDWidget::UpdateLoopReason(bool bKickAll)
{
	if (APotProbPlayerState* PlayerState = GetOwningPlayerState<APotProbPlayerState>())
	{
		FString Reason;
		if (bKickAll)
		{
			Reason = InsufficientPlayersForLoop;
		}
		else
		{
			if (PlayerState->GetIsPlayAgain())
			{
				Reason = LoadingNewLevelText;
			}
			else
			{
				Reason = NotPlayingAgainKick;
			}
		}
		if (TimerReasonText)
		{
			TimerReasonText->SetText(FText::FromString(Reason));
		}
	}
}

void UHUDWidget::UpdateCharacterSprite(const FCharacterData& CharacterData)
{
	CharacterSprite->SetBrushFromTexture(Cast<UTexture2D>(CharacterData.HeadOnlyTexture));
}


//###########################################################################
/*
 *	TODO: Debug Functions Need to be removed for Final Product
 */
//########################################################################### 

void UHUDWidget::DebugChangeHUD(FName NewScreen)
{
	if (NewScreen == FName("Apprentice"))
	{
		SetActiveWidgetByName(WSHUDWidget,"GameOverlay");
		SetActiveWidgetByName(WSGameStates,"InGameOverlay");
		SetActiveWidgetByName(WSPlayerType,"HUDApprentice");
	}
	else if (NewScreen == FName("Troublemaker"))
	{
		SetActiveWidgetByName(WSHUDWidget,"GameOverlay");
		SetActiveWidgetByName(WSGameStates,"InGameOverlay");
		SetActiveWidgetByName(WSPlayerType,"HUDTroublemaker");
	}
	else if (NewScreen == FName("Lobby"))
	{
		SetActiveWidgetByName(WSHUDWidget,"GameOverlay");
		SetActiveWidgetByName(WSGameStates,"Lobby");
	}
	else if (NewScreen == FName("InGame"))
	{
		SetActiveWidgetByName(WSHUDWidget,"GameOverlay");
		SetActiveWidgetByName(WSGameStates,"InGameOverlay");
	}
	else if (NewScreen == FName("EndGame"))
	{
		SetActiveWidgetByName(WSHUDWidget,"GameOverlay");
		SetActiveWidgetByName(WSGameStates,"EndGame");
		//UpdateEndGameScreen(true, true);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Invalid Command Argument"));
	}
}
