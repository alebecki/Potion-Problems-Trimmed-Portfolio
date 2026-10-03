// Fill out your copyright notice in the Description page of Project Settings.


#include "PotProbGameState.h"

#include "Kismet/GameplayStatics.h"
#include "PotProbGameMode.h"
#include "PotProbPlayerController.h"
#include "PotProbPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "PotProbZDCharacter.h"
#include "FOVComponent.h"
#include "HUDWidget.h"
#include "VoteRevealWidget.h"
#include "WhoVotedWidget.h"
#include "PotProbOnlineSubsystem.h"
#include "Net/Core/PushModel/PushModel.h"

APotProbGameState::APotProbGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Initialize the array with enough elements for all character models (0-9)
	// Set all to false (unselected)
	ModelOwnership.Init(-1, 10); // 10 elements for CHAR_MODEL_0 through CHAR_MODEL_9 and a dummy value for updating the array
}

void APotProbGameState::StartTieBreaker_Implementation()
{
	if (CurrentPhase != EPotProbPhases::PHASE_VOTE)
	{
		return;
	}

	VoteTimer = TieBreakerPhase_Duration;
	
	for (const auto it : PlayerArray)
	{
		if (APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(it.Get()))
		{
			PlayerState->bHasVoted = false;
			PlayerState->VoteCount = 0;
			VotesCast.Empty();
		}
	}
	
	UpdateVotingStatus(true, true, true);
	
}

void APotProbGameState::ProcessVote_Implementation(const FString& PlayerName, const FString& VoterName)
{
	bool bAllPlayersVoted = true;
	for (const auto it : PlayerArray)
	{
		if (APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(it.Get()))
		{
			if (PlayerState->GetPlayerName() == PlayerName)
			{
				PlayerState->VoteCount++;
				bool b_PrevVoted = false;
				for (FVotesCast& PreviouslyVoted : VotesCast) {
					if (PreviouslyVoted.Player == PlayerState) {
						b_PrevVoted = true;
						for (const auto itr : PlayerArray)
						{
							if (APotProbPlayerState* VoterState = Cast<APotProbPlayerState>(itr.Get()))
							{
								if (VoterState->GetPlayerName() == VoterName)
								{
									PreviouslyVoted.Votes.Add(VoterState);
								}
							}
						}
					}
				}
				if (!b_PrevVoted) {
					FVotesCast NewVotes;
					NewVotes.Player = PlayerState;
					for (const auto itr : PlayerArray)
					{
						if (APotProbPlayerState* VoterState = Cast<APotProbPlayerState>(itr.Get()))
						{
							if (VoterState->GetPlayerName() == VoterName)
							{
								NewVotes.Votes.Add(VoterState);
							}
						}
					}
					VotesCast.Add(NewVotes);
				}
			} else if (PlayerState->GetPlayerName() == VoterName)
			{
				PlayerState->bHasVoted = true;
			}

			// Determine if this player has not voted
			if (!PlayerState->bHasVoted)
			{
				if (!PlayerState->bIsFrogged) {
					bAllPlayersVoted = false;
				}
			}
		}
	}
	
	if (bAllPlayersVoted)
	{
		EndVotingPhase();
	}
}


void APotProbGameState::UpdateVotingStatus_Implementation(bool bUpdateVotingList, bool bCanPlayerVote, bool bResetPlayerSelected)
{
	for (const auto it : PlayerArray)
	{
		if (APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(it.Get()))
		{
			if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(PlayerState->GetOwningController()))
			{
				PC->Client_UpdateLocalVoteWidget(bUpdateVotingList, bCanPlayerVote, bIsTieBreaker, TiedPlayers, bResetPlayerSelected);
			}
		}
	}
}

void APotProbGameState::OnRep_LobbyStatus()
{
	for (const auto it : PlayerArray)
	{
		if (APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(it.Get()))
		{
			if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(PlayerState->GetOwningController()))
			{
				PC->Client_UpdateLocalLobbyStatusUI();
			}
		}
	}
}


void APotProbGameState::EndGame(bool ApprenticeWin, bool bApprenticeWonByVoting)
{
	CurrentPhase = EPotProbPhases::PHASE_END;
	if (ApprenticeWin) {
		currentWinner = EPotProbWinningTeam::APPRENTICE_WIN;
	}
	else {
		currentWinner = EPotProbWinningTeam::TROUBLEMAKER_WIN;
	}
	WonByVoting = bApprenticeWonByVoting;
	OnRep_GameOver();
}

void APotProbGameState::OnRep_NewPlayer()
{
	for (TObjectPtr<APlayerState> PlayerState : PlayerArray)
	{
		if (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(PlayerState->GetPawn()))
		{
			/* Whenever anyone joins */
			// Update Everyone's "Other Players Array"
			if (UFOVComponent* FOV = Character->GetComponentByClass<UFOVComponent>())
			{
				FOV->GenerateParams(Players);
			}
			UPotProbOnlineSubsystem* PotionsOnlineSubsys = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
			if (!PotionsOnlineSubsys || !PotionsOnlineSubsys->SessionInterface || !PotionsOnlineSubsys->IdentityInterface)
			{
				return;
			}
			if (FOnlineSessionSettings* SessionSettings = PotionsOnlineSubsys->SessionInterface->GetSessionSettings(FName("GameSession")))
			{
				PotionsOnlineSubsys->SessionInterface->UpdateSession(FName("GameSession"), *SessionSettings);
			}
		}
	}
}

void APotProbGameState::OnRep_LoopingStatus()
{
	// Start timer to kick non readied players
	// Start timer to load new level
	if (HasAuthority())
	{
		if (APotProbGameMode* GameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (GetNumLoopingPlayers() >= MinPlayers)
			{
				GameMode->UpdateLoopTimers(false);
			}
			else
			{
				GameMode->ClearLoopTimers();
			}
		}
	}
	
	APotProbPlayerController* PC = Cast<APotProbPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->Client_UpdateLocalLoopStatusUI();
	}
}

void APotProbGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APotProbGameState, CurrentPhase);
	DOREPLIFETIME(APotProbGameState, VoteTimer);
	DOREPLIFETIME(APotProbGameState, bCanVote);
	DOREPLIFETIME(APotProbGameState, NumPlayers);
	DOREPLIFETIME(APotProbGameState, NumApprentices);
	DOREPLIFETIME(APotProbGameState, NumTroublemakers);
	DOREPLIFETIME(APotProbGameState, MinPlayers);
	DOREPLIFETIME(APotProbGameState, RemainingCountdownTime);
	DOREPLIFETIME(APotProbGameState, bIsCountdownActive);
	DOREPLIFETIME(APotProbGameState, numApprenticeFrogged);
	DOREPLIFETIME(APotProbGameState, numTroublemakerFrogged);
	DOREPLIFETIME(APotProbGameState, TotalRecipes);
	DOREPLIFETIME(APotProbGameState, CurrentlyCraftedRecipes);
	DOREPLIFETIME(APotProbGameState, CurrentlyCraftedApprenticeRecipes);
	DOREPLIFETIME(APotProbGameState, currentWinner)
	DOREPLIFETIME(APotProbGameState, Players);
	DOREPLIFETIME(APotProbGameState, PlayerVotedName);
	DOREPLIFETIME(APotProbGameState, VotesCast);
	DOREPLIFETIME(APotProbGameState, bIsTieBreaker);
	DOREPLIFETIME(APotProbGameState, TiedPlayers);
	DOREPLIFETIME(APotProbGameState, CauldronActors);
	DOREPLIFETIME(APotProbGameState, TelescopeSuccesses);
	DOREPLIFETIME(APotProbGameState, NumLoopingPlayers);
	DOREPLIFETIME(APotProbGameState, VotedOutHistory);
	DOREPLIFETIME(APotProbGameState, WonByVoting);
	DOREPLIFETIME(APotProbGameState, TotalVotingRounds)
    	DOREPLIFETIME_CONDITION_NOTIFY(APotProbGameState, ModelOwnership, COND_None, REPNOTIFY_Always);
}


void APotProbGameState::OnRep_GameOver()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->IsLocalController()) // Optional: Only target local players
		{
			// Increment HUD to End Game
			if (APotProbPlayerController* PotProbController = Cast<APotProbPlayerController>(PC))
			{
				PotProbController->ChangeMusic(EPotProbPhases::PHASE_INGAME);
				if (UHUDWidget* HUD = Cast<UHUDWidget>(PotProbController->GetHUDWidgetInstance()))
				{
					if (currentWinner == EPotProbWinningTeam::APPRENTICE_WIN)
					{
						PotProbController->PlayGameOverSound(true);
						HUD->UpdateEndGameScreen(true, WonByVoting);
					}
					else if(currentWinner == EPotProbWinningTeam::TROUBLEMAKER_WIN)
					{
						PotProbController->PlayGameOverSound(false);
						HUD->UpdateEndGameScreen(false, WonByVoting);
					}
					HUD->IncrementHUDGameState();
				}
			}
		}
	}
}

void APotProbGameState::PauseTimers()
{
	// Cooties
	if (const APotProbGameMode* PotGameMode = Cast<APotProbGameMode>(GetDefaultGameMode()))
	{
		GetWorldTimerManager().PauseTimer(PotGameMode->CootiesTimerHandle);
	}

	if (APotProbPlayerController* PotPlayer = Cast<APotProbPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		if (APotProbPlayerState* PotPlayerState = PotPlayer->GetPlayerState<APotProbPlayerState>())
		{
			PotPlayerState->SetCanInteractWithObjects(false);
			// Unstable
			GetWorldTimerManager().PauseTimer(PotPlayerState->UnstablePotionTimer);
			// Questionmark
			GetWorldTimerManager().PauseTimer(PotPlayerState->QuestionmarkPotionTimer);
			// Translocation
			GetWorldTimerManager().PauseTimer(PotPlayerState->TranslocationPotionTimer);
			// Disguise
			GetWorldTimerManager().PauseTimer(PotPlayerState->DisguisePotionHandle);
			// Ego
			GetWorldTimerManager().PauseTimer(PotPlayerState->EgoPotionHandle);
		}
		if (APotProbZDCharacter* ZDCharacter = Cast<APotProbZDCharacter>(PotPlayer->GetPawn()))
		{
			// Camouflage
			GetWorldTimerManager().PauseTimer(ZDCharacter->CamouflageTimer);
			// Clarivoyance
			GetWorldTimerManager().PauseTimer(ZDCharacter->ClarivoyanceTimer);
			// Werefrog
			GetWorldTimerManager().PauseTimer(ZDCharacter->WerefrogTimer);
		}
	}
}

void APotProbGameState::UnPauseTimers()
{
	// Cooties
	if (const APotProbGameMode* PotGameMode = Cast<APotProbGameMode>(GetDefaultGameMode()))
	{
		GetWorldTimerManager().UnPauseTimer(PotGameMode->CootiesTimerHandle);
	}
	
	if (APotProbPlayerController* PotPlayer = Cast<APotProbPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		if (APotProbPlayerState* PotPlayerState = PotPlayer->GetPlayerState<APotProbPlayerState>())
		{
			PotPlayerState->SetCanInteractWithObjects(true);
			// Unstable
			GetWorldTimerManager().UnPauseTimer(PotPlayerState->UnstablePotionTimer);
			// Questionmark
			GetWorldTimerManager().UnPauseTimer(PotPlayerState->QuestionmarkPotionTimer);
			// Translocation
			GetWorldTimerManager().UnPauseTimer(PotPlayerState->TranslocationPotionTimer);
			// Disguise
			GetWorldTimerManager().UnPauseTimer(PotPlayerState->DisguisePotionHandle);
			// Ego
			GetWorldTimerManager().UnPauseTimer(PotPlayerState->EgoPotionHandle);
		}
		if (APotProbZDCharacter* ZDCharacter = Cast<APotProbZDCharacter>(PotPlayer->GetPawn()))
		{
			// Camouflage
			GetWorldTimerManager().UnPauseTimer(ZDCharacter->CamouflageTimer);
			// Clarivoyance
			GetWorldTimerManager().UnPauseTimer(ZDCharacter->ClarivoyanceTimer);
			// Werefrog
			GetWorldTimerManager().UnPauseTimer(ZDCharacter->WerefrogTimer);
		}
	}
}

void APotProbGameState::UpdateVoteTimer()
{
	if (CurrentPhase != EPotProbPhases::PHASE_VOTE)
	{
		return;
	}
	
	VoteTimer -= VoteTimer_UpdateInterval;
	
	if (VoteTimer <= 0.f)
	{
		EndVotingPhase();
	} else if (VoteTimer <= (VotePhase_Duration - DiscussionPhase_Duration))
	{
		if (!bCanVote)
		{
			bCanVote = true;
			UpdateVotingStatus(true, true, false);
		}
		UpdateVotingStatus(false, bCanVote, false);
	} else
	{
		UpdateVotingStatus(false, bCanVote, false);
	}
}

void APotProbGameState::StartVotingPhase_Implementation()
{
	if (CurrentPhase == EPotProbPhases::PHASE_VOTE)
	{
		return;
	}
	if (CurrentPhase == EPotProbPhases::PHASE_END)
	{
		
		return;
	}
	CurrentPhase = EPotProbPhases::PHASE_VOTE;
	bIsTieBreaker = false;

	for (const auto it : PlayerArray)
	{
		if (APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(it.Get()))
		{
			PlayerState->bHasVoted = false;
			PlayerState->VoteCount = 0;
			VotesCast.Empty();
			PlayerState->SetVotingPhase(true);
		}
	}
	//Start Vote timer
	VoteTimer = VotePhase_Duration;
	GetWorld()->GetTimerManager().SetTimer(VoteTimerHandle, this, &APotProbGameState::UpdateVoteTimer, VoteTimer_UpdateInterval, true);

}

void APotProbGameState::EndVotingPhase_Implementation()
{
	if (CurrentPhase != EPotProbPhases::PHASE_VOTE)
	{
		return;
	}
	PlayerVotedName = "";
	
	int MaxVote = -1;
	for (const auto it : PlayerArray)
	{
		if (APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(it.Get()))
		{
			MaxVote = FMath::Max(MaxVote, PlayerState->VoteCount);
			Cast<APotProbPlayerController>(PlayerState->GetPlayerController())->ChangeMusic(EPotProbPhases::PHASE_INGAME);
			Cast<APotProbPlayerController>(PlayerState->GetPlayerController())->PlayVotedOutSFX();
		}
	}

	int TiedPlayersCount = 0;
	for (const auto it : PlayerArray)
	{
		if (APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(it.Get()))
		{
			if (PlayerState->VoteCount == MaxVote)
			{
				TiedPlayersCount++;
			}
		}
	}

	if (TiedPlayersCount > 1 && MaxVote > 0)
	{
		if (!bIsTieBreaker)
		{
			bIsTieBreaker = true;
			TiedPlayers.Empty();
			for (const auto it : PlayerArray)
			{
				if (APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(it.Get()))
				{
					if (PlayerState->VoteCount == MaxVote)
					{
						TiedPlayers.Add(PlayerState);
					}
				}
			}
			StartTieBreaker();
			return;
		} else
		{
			VotedOutHistory.Add(nullptr);
			for (const auto it : PlayerArray)
			{
				if (APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(it.Get()))
				{
					PlayerState->SetVotingPhase(false);
				}
			}
		}
	}
	// If there is no tie, we can proceed to frog the player with the most votes
	// We don't want any one to be frogged if each player gets exactly one vote (if all players voted).
	else { 
		// TArray<APotProbPlayerState*> PlayersToBeFrogged;
		bool bFroggedPlayerDuringVoting = false;
		bool bFroggedTroublemakerDuringVoting = false;
		for (const auto it : PlayerArray)
		{
			if (APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(it.Get()))
			{
				PlayerState->SetVotingPhase(false);
				//WhoVotedForWhoMap.Add(PlayerState, PlayerState->PlayersThatVoted);
				if (MaxVote > 1 && PlayerState->VoteCount == MaxVote)
				{
					if (PlayerState->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
					{
						bFroggedTroublemakerDuringVoting = true;
					}
					VotedOutHistory.Add(PlayerState);
					PlayerState->NumRoundVotedOutOn = TotalVotingRounds + 1;
					PlayerState->bWasVotedOut = true;
					PlayerState->SetIsFroggedVoting(true, false);
					PlayerVotedName = PlayerState->GetPlayerName();
					bFroggedPlayerDuringVoting = true;
				}
				else
				{
					PlayerState->NumVotingRoundsSurvived++;
				}
			}
		}
		if (bFroggedTroublemakerDuringVoting)
		{
			for (const auto it : PlayerArray)
			{
				if (APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(it.Get()))
				{
					PlayerState->NumTroublemakersVoted++;
				}
			}
		}
		/*if (!bFroggedPlayerDuringVoting)
		{
			if (APotProbGameMode* GameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode()))
			{
				GameMode->CheckWin();
			}
		}*/
	}
	
	//End vote timer
	VoteTimer = 0.f;
	GetWorld()->GetTimerManager().ClearTimer(VoteTimerHandle);

	//End vote phase
	CurrentPhase = EPotProbPhases::PHASE_INGAME;
	bCanVote = false;
	TotalVotingRounds++;

	// We are restarting so we should set the distribution variable back to false
	APotProbGameMode* ProbGameMode = Cast<APotProbGameMode>(GetWorld()->GetAuthGameMode());
	if(!ProbGameMode)
	{
		return;
	}

	//Lin: not sure what this does???
	ProbGameMode->SetHasDistributedPotions(false);

	ProbGameMode->TeleportPlayersToStart();
	ProbGameMode->ResetCanVoteInitCountdown();
	
	if (HasAuthority())
	{
	    ServerShowVoteRevealPopup(PlayerVotedName, VotesCast);
	    PlayerVotedName = "";
	}
}

void APotProbGameState::ServerShowVoteRevealPopup_Implementation(const FString& PlayerName, const TArray<FVotesCast>& WhoVotedMap)
{
    if (HasAuthority())
    {
        MulticastShowVoteRevealPopup(PlayerName, WhoVotedMap);
    }
}

void APotProbGameState::MulticastShowVoteRevealPopup_Implementation(const FString& PlayerName, const TArray<FVotesCast>& WhoVotedMap)
{
    APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetWorld()->GetFirstPlayerController());
	UWhoVotedWidget* VotedWidget = CreateWidget<UWhoVotedWidget>(PC, WhoVotedWidgetClass);
	if (VotedWidget) {
		VotedWidget->SetVotedOutPlayerName(PlayerName);
		VotedWidget->SetVoteRevealedClass(VoteRevealWidgetClass);
		VotedWidget->SetWhoVotedMap(WhoVotedMap);
		VotedWidget->AddToViewport(100);
		VotedWidget->BuildVotingList();
	}
}

void APotProbGameState::SetCauldronActors(const TArray<class ACauldronActor*>& Cauldrons)
{
	CauldronActors = Cauldrons;
}

bool APotProbGameState::IncrementTelescopeSuccesses()
{
	TelescopeSuccesses++;
	if (TelescopeSuccesses >= TelescopeSuccessThreshold)
	{
		ActivateTroublemakerWinCon();
		return true;
	}
	return false;
}

void APotProbGameState::ActivateTroublemakerWinCon()
{
	TelescopeSuccesses = 0;
	
	// switcheroo effect on cauldrons
	SwitcherooAllCauldrons();
	
	// turn troublemakers into werefrogs and activate switcheroo warning
	for (APlayerState* PS : PlayerArray)
	{
		APotProbPlayerState* MyPS = Cast<APotProbPlayerState>(PS);

		MyPS->ActivateSwitcherooWarning();
		
		if (MyPS->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
		{
			MyPS->ActivateWerefrogWinConCountdown(FrogPotionClass);
		}
	}
}

void APotProbGameState::SwitcherooAllCauldrons()
{
	// switch all cauldrons
	if (APotProbPlayerState* PS = Cast<APotProbPlayerState>(PlayerArray[0]))
	{
		for (ACauldronActor* Caul : CauldronActors)
		{
			int RandCaulIdx = rand() % CauldronActors.Num();
			// get random bool between 0 and sizeof(CaulAct)
			PS->Multicast_SwitchPlacesCauldron(Caul, CauldronActors[RandCaulIdx]);
		}
	}
	
}

void APotProbGameState::AssignRandomModel(class APotProbZDCharacter* Character)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		// Only acceptable on server.
		return;
	}
	
	TArray<int32> UnselectedModels;
	for (int32 i = 0; i < ModelOwnership.Num(); i++)
	{
		if (ModelOwnership[i] == -1)
		{
			UnselectedModels.Push(i);
		}
	}

	if (UnselectedModels.Num() > 0)
	{
		RequestModelUpdate(Character, UnselectedModels[FMath::RandRange(0, UnselectedModels.Num() - 1)]);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to assign random character model! All models are selected."));
	}
}


void APotProbGameState::BeginPlay()
{
	Super::BeginPlay();
}

int32 APotProbGameState::GetCharacterModelIndex(const APotProbZDCharacter* Character) const
{
	for (int32 i = 0; i < ModelOwnership.Num(); i++)
	{
		if (ModelOwnership[i] == Character->ModelOwnerId)
		{
			return i;
		}
	}

	// Character not found!
	return -1;
}

void APotProbGameState::RequestModelUpdate(APotProbZDCharacter* Character, int32 ModelIndex)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		// Only valid on server
		return;
	}
	
	if (ModelOwnership[ModelIndex] == -1)
	{
		// Character is allowed to take model.
		int32 CurrentModel = GetCharacterModelIndex(Character);
		if (CurrentModel >= 0)
		{
			// Clear previous ownership
			ModelOwnership[CurrentModel] = -1;
		}
		
		ModelOwnership[ModelIndex] = Character->ModelOwnerId;
	}
}

void APotProbGameState::ReleaseModel(class APotProbZDCharacter* Character)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		// Only valid on server
		return;
	}
	
	int32 CurrentModel = GetCharacterModelIndex(Character);
	if (CurrentModel >= 0)
	{
		ModelOwnership[CurrentModel] = -1;
	}
}

