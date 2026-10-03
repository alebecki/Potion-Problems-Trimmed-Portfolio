// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "PotionObject.h"
#include "PotProbGameMode.h"
#include "PotProbGameState.generated.h"

class UPotionObject;
class UVoteRevealWidget;
class UWhoVotedWidget;
class APotProbPlayerState;

UENUM(BlueprintType)
enum class EPotProbPhases : uint8
{
	/* NONE phase - should not be used unless for NULL checks */
	PHASE_NONE,
	/* LOBBY phase - Players are in the waiting lobby, waiting for the game to start */
	PHASE_LOBBY,
	/* INGAME phsae - Players are navigating in the scene, collecting ingredients and collecting potions */
	PHASE_INGAME,
	/* VOTE phase - Players are discussing and choose the player to be voted out (frogged) */
	PHASE_VOTE,
	/* END phase - game ended */
	PHASE_END
};
UENUM(BlueprintType)
enum class EPotProbWinningTeam : uint8
{
	NONE_WIN,
	APPRENTICE_WIN,
	TROUBLEMAKER_WIN
};
/**
 *
 */

USTRUCT()
struct FVotesCast
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<APotProbPlayerState*> Votes;

	UPROPERTY()
	APotProbPlayerState* Player;
};
UCLASS()
class POTIONPROBLEMS_DEV_API APotProbGameState : public AGameState
{
	GENERATED_BODY()
public:
	APotProbGameState();

	UPROPERTY(Replicated)
	EPotProbPhases CurrentPhase = EPotProbPhases::PHASE_NONE;

	UPROPERTY(ReplicatedUsing=OnRep_GameOver)
	EPotProbWinningTeam currentWinner = EPotProbWinningTeam::NONE_WIN;

	UPROPERTY(Replicated)
	bool WonByVoting = false;

	/* Voting phase related functions */
	
	UFUNCTION(Server, Reliable)
	void StartVotingPhase();

	UFUNCTION(Server, Reliable)
	void StartTieBreaker();
	
	UFUNCTION(Server, Reliable)
	void EndVotingPhase();

	// the server processes a vote from a player, and determines if ending the voting phase is necessary
	UFUNCTION(Server, Reliable)
	void ProcessVote(const FString& PlayerName, const FString& VoterName);

	// the server broadcasts to all clients asking them to update voting widgets
	UFUNCTION(Server, Reliable)
	void UpdateVotingStatus(bool bUpdateVotingList = false, bool bCanPlayerVote = false, bool bResetPlayerSelected = false);
	
	UPROPERTY(Replicated)
	float VoteTimer = 0.0f;

	UPROPERTY(Replicated)
	bool bCanVote = false;

	UPROPERTY(Replicated)
	bool bIsTieBreaker = false;

	UPROPERTY(Replicated)
	TArray<APotProbPlayerState*> TiedPlayers;
	
	UFUNCTION()
	void UpdateVoteTimer();

	// This variable specified how long the tie-breaker phase should last
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float TieBreakerPhase_Duration = 30.0f;
	
	// This variable specifies how long the discussion phase should last
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DiscussionPhase_Duration = 120.0f;
	
	// This variable specifies how long the voting phase should last
	// (INCLUDING the discussion phase where you can't cast a vote)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float VotePhase_Duration = DiscussionPhase_Duration + 60.0f;
	
	// This variable specifies how often should the Vote Timer to be updated
	// * Default should works fine
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float VoteTimer_UpdateInterval = 0.1f;
	
	UPROPERTY(BlueprintReadWrite, Replicated)
    FString PlayerVotedName = "";
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UVoteRevealWidget> VoteRevealWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UWhoVotedWidget> WhoVotedWidgetClass;
    
    UFUNCTION(Server, Reliable)
    void ServerShowVoteRevealPopup(const FString& PlayerName, const TArray<FVotesCast>& WhoVotedMap);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastShowVoteRevealPopup(const FString& PlayerName, const TArray<FVotesCast>& WhoVotedMap);
    
	/* End voting phase related functions */
	
	/* Lobby related functions */
	UPROPERTY(ReplicatedUsing=OnRep_LobbyStatus)
	int32 MinPlayers;
	
	UPROPERTY(ReplicatedUsing=OnRep_LobbyStatus)
	int32 NumPlayers;

	UPROPERTY(Replicated)
	int32 NumApprentices;

	UPROPERTY(Replicated)
	int32 NumTroublemakers;


	UPROPERTY(ReplicatedUsing=OnRep_LobbyStatus)
	float RemainingCountdownTime;
	
	UPROPERTY(ReplicatedUsing=OnRep_LobbyStatus)
	bool bIsCountdownActive;

	UPROPERTY(ReplicatedUsing=OnRep_LobbyStatus)
	int32 numApprenticeFrogged;

	UPROPERTY(ReplicatedUsing=OnRep_LobbyStatus)
	int32 numTroublemakerFrogged;

	// In GameState
	UFUNCTION()
	void UpdateLobbyStatus(bool bCountdownActive, float CountdownTime, int32 CurrNumPlayers, int32 MinPlayerCount, int32 ApprenticeCount, int32 TroublemakerCount)
	{
		bIsCountdownActive = bCountdownActive;
		RemainingCountdownTime = CountdownTime;
		NumPlayers = CurrNumPlayers;
		MinPlayers = MinPlayerCount;
		NumApprentices = ApprenticeCount;
		NumTroublemakers = TroublemakerCount;
		OnRep_LobbyStatus();
	}
	UFUNCTION()
	void OnRep_LobbyStatus();
	/* End Lobby related functions */

	/* Loop Game Related Functionality */
	const int GetNumLoopingPlayers() const { return NumLoopingPlayers; }
	void SetNumLoopingPlayers(const int NewNumLoopingPlayers) { NumLoopingPlayers = NewNumLoopingPlayers; }
	UFUNCTION()
	void OnRep_LoopingStatus();
	/* End Loop Game Related Functionality */
	
	/* Potion Related Functionality */
	int GetNumberTotalPotionRecipes() { return TotalRecipes; }
	void SetNumberTotalPotionRecipes(int Total) { TotalRecipes = Total; }
	int GetCurrentlyCraftedNumPotions() { return CurrentlyCraftedRecipes; }
	void SetCurrentlyCraftedNumPotions(int Num) { CurrentlyCraftedRecipes = Num; }
	int GetCurrentlyCraftedNumApprenticePotions() { return CurrentlyCraftedApprenticeRecipes; }
	void SetCurrentlyCraftedNumApprenticePotions(int Num) { CurrentlyCraftedApprenticeRecipes = Num; }
	int GetApprenticeWinMultiplier() { return ApprenticePotionMultiplier; }
	FRecipeStruct FrogRecipe;

	/* End Potion Related Functionality */

	void EndGame(bool ApprenticeWin, bool bApprenticeWonByVoting);

	// FOV
	UPROPERTY(ReplicatedUsing=OnRep_NewPlayer)
	TArray<TObjectPtr<class AActor>> Players;
	
	UFUNCTION()
	void OnRep_NewPlayer();

	UPROPERTY(Replicated)
	TArray<FVotesCast> VotesCast;

	// Telescope Troublemaker Win Condition
	UPROPERTY(Replicated)
	TArray<class ACauldronActor*> CauldronActors;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UPotionObject> FrogPotionClass;
	UFUNCTION()
	void SetCauldronActors(const TArray<class ACauldronActor*>& Cauldrons);
	// returns true if win con is activated
	UFUNCTION()
	bool IncrementTelescopeSuccesses();
	UFUNCTION()
	void ActivateTroublemakerWinCon();
	UFUNCTION()
	void SwitcherooAllCauldrons();

	UFUNCTION()
	void AssignRandomModel(class APotProbZDCharacter* Character);

	void RequestModelUpdate(class APotProbZDCharacter* Character, int32 ModelIndex);
	void ReleaseModel(class APotProbZDCharacter* Character);

	UFUNCTION()
	int32 GetCharacterModelIndex(const class APotProbZDCharacter* Character) const;
	
	UFUNCTION()
	const TArray<int32>& GetModelOwnership() {return ModelOwnership;}

	UPROPERTY(Replicated)
	TArray<int32> ModelOwnership;

	// End Game Screen
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<APotProbPlayerState*> VotedOutHistory;
	UPROPERTY(BlueprintReadWrite, Replicated)
	int TotalVotingRounds = 0;

	// Pausing Timers
	void PauseTimers();
	void UnPauseTimers();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(Replicated)
	int TotalRecipes = 0;
	UPROPERTY(Replicated)
	int CurrentlyCraftedRecipes = 0;
	UPROPERTY(Replicated)
	int CurrentlyCraftedApprenticeRecipes = 0;
    
	// Loop game
	UPROPERTY(ReplicatedUsing=OnRep_LoopingStatus)
	int NumLoopingPlayers = 0;
	
	// Telescope minigame win con
	UPROPERTY(Replicated)
	int TelescopeSuccesses = 0;
	UPROPERTY(EditDefaultsOnly)
	int TelescopeSuccessThreshold = 2;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;

	UPROPERTY(EditDefaultsOnly, Category = "Wincon")
	int32 ApprenticePotionMultiplier;

	UPROPERTY()
	FTimerHandle VoteTimerHandle;
	UFUNCTION()
    void OnRep_GameOver();


};
