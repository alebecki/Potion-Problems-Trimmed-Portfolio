// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PotProbGameState.h"
#include "PotProbEnums.h"
#include "GameFramework/PlayerState.h"
#include "PotProbPlayerState.generated.h"


class USpyglassMinigameWidget;
class UFrogDiscoveredWidget;
class UPotionObject;
class UPaperSprite;
class UTexture2D;
class ACauldronActor;
class AObservatoryWorldActor;
class APotProbPlayerController;

/**
 *
 */


UCLASS()
class POTIONPROBLEMS_DEV_API APotProbPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	APotProbPlayerState();
	/* Minigame Functions */
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientActivateInteractedMinigame(APawn* InstigatorPawn, TSubclassOf<UUserWidget> MinigameWidgetClass,
									  FName Name);
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientActivateInteractedMinigameWorld(APawn* InstigatorPawn,AMinigameInitActor* InitActor,  TSubclassOf<AWorldSpaceWidgetActor> MinigameWidgetActorClass,
										  FVector SpawnLocation, FRotator SpawnRotation, FName Name);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ResetStarMinigame(AObservatoryWorldActor* ObservatoryActor);
	UPROPERTY()
	USpyglassMinigameWidget* SpyglassWidget = nullptr;
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_RemoveMinigameInputBindings();
	bool bPlayedMinigame = false;
	/* End Minigame Functions */

	/** Potion/Interaction Functions Start */
	// Potion set/get
	void SetPlayerPotion(UPotionObject* Object) { PlayerPotion = Object; }
	TObjectPtr<UPotionObject> GetPlayerPotion() { return PlayerPotion; }

	// Potion activation settings
	void SetCanPotionBeActivated(const bool Input) { bHasPotionBeenActivated = Input; }
	bool GetCanPotionBeActivated() const { return bHasPotionBeenActivated; }

	// Potion Charges
	EPotionChargeType GetPotionChargesType() const { return CurrentPotionChargeType; }
	int32 GetNumPotionCharges() const { return NumPotionCharges; }
	void ReduceNumPotionCharges();
	
	// Server Sided function Cleanup after Potion completion
	UFUNCTION(BlueprintCallable)
	void ServerCleanupPotionActivation();
	// Server Sided function that attempts to receive potion
	UFUNCTION(BlueprintCallable)
	void ServerReceivePotion(TSubclassOf<UPotionObject> PotionType);
	// Server Sided Function that attempts to give charges to player
	UFUNCTION(BlueprintCallable)
	void ServerGivePlayerPotionCharges();
	UFUNCTION(BlueprintCallable)
	UPotionObject* GetPlayerPotionPtr() const { return PlayerPotion; }
	// Client Sided function to distribute potion recipe cards
	UFUNCTION(Client, Reliable)
	void GivePlayerStatePotionRecipes(const TArray<FRecipeStruct>& DistributedRecipes);
	
	UFUNCTION()
	void OnRep_PlayerPotion();
	void SetCanInteractWithObjects(bool Input);
	bool GetCanInteractWithObjects() const { return bCanInteractWithObjects; }
	
	UFUNCTION()
	void OnRep_IngredientName();
	void SetIngredientName(const FName NewIngredientName);
	FName GetIngredientName() const { return IngredientName; }
	void SetIsHoldingIngredient(const bool Input);
	bool GetIsHoldingIngredient() const { return bHoldingIngredient; }
	bool HasPickedUpFirstIngredient() const { return bHasPickedUpFirstIngredient; }
	
	UFUNCTION()
	void OnRep_IngredientIcon();
	void SetIngredientIcon(UPaperSprite* Texture);
	void SetPotionActivation(bool Input) { bShouldDisplayPotionAsActive = Input; }
	bool GetPotionActivation() const { return bShouldDisplayPotionAsActive; }

	UFUNCTION(Server, Reliable)
	void Server_SetCurrentRecipe(const FRecipeStruct& Recipe);
	//void SetCurrentRecipe(const FRecipeStruct& Recipe);
	UFUNCTION(Client, Reliable)
	void Client_SetCurrentRecipe(const FRecipeStruct& Recipe);
	UFUNCTION(Server, Reliable)
	void Server_SetFrogRecipe();
	FRecipeStruct GetCurrentRecipe();
	int32 GetNumRerolls() const { return NumTimesRecipeRerolled; }


	bool GetHasPotionBeenActivated() { return  bHasPotionBeenActivated; }
	UFUNCTION(Server, Reliable)
	void ServerRerollPotions();
	/** Potion/Interaction Functions End */


	/* Alert messages functions start*/
	// Send alert message Client to server
	UFUNCTION(Server, Reliable)
	void ServerReceiveAlertMessage(const EPotProbAlertTypes& AlertType, const FString& MessageContent, bool bTriggerOnInstigator = true);
	// Send Alert message Server to client
	UFUNCTION(Client, Reliable)
	void ClientReceiveAlertMessage(const EPotProbAlertTypes& AlertType, const FString& MessageContent);
	/* Alert messages functions end*/
	
	// Alert variables
    int NumFrogDoorAlerts = 0;
    bool bFirstIngredientAdded = false;

	/** Chat messaging functions start*/
	// Send Message Client to Server
	UFUNCTION(Server, Reliable)
	void ServerReceiveChatMessage(const FString& MessageSenderName, EAnimModels Model, const FString& MessageContent);
	// Send message Server to client
	UFUNCTION(Client, Reliable)
	void ClientReceiveChatMessage(const FString& MessageSenderName, EAnimModels Model, const FString& MessageContent);
	/** Chat messaging functions end */

	/** Player Roles Function start*/
	UPROPERTY(ReplicatedUsing=OnRep_PotProbRole)
	EPotProbRoles PotProbRole = EPotProbRoles::ROLE_NONE;
	UFUNCTION()
	void OnRep_PotProbRole() const;

	UFUNCTION(Server, Reliable)
	void Server_SetRole(EPotProbRoles NewRole);

	UPROPERTY(ReplicatedUsing=OnRep_IsFrogged, BlueprintReadOnly)
	bool bIsFrogged = false;
	UFUNCTION()
	void OnRep_IsFrogged();
	// This bool represents if the frogged state is discoverable (i.e. frogged by potion, not by vote)
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bCanFroggedBeDiscovered = false;
	// Represents time until frog discovered ui pops up
	int FrogDiscoveredCountdown = 5;
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetIsFrogged(bool status, bool bDiscoverable = false);
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetIsFroggedVoting(bool status, bool bDiscoverable = false);
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OnFroggedDiscovered(TSubclassOf<UFrogDiscoveredWidget> FrogDiscoveredWidgetClass);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void MulticastShowFrogDiscoveredPopUp(TSubclassOf<UFrogDiscoveredWidget> FrogDiscoveredWidgetClass);

	// Camouflage Potion
	//Swifty (Switcharoo) Potion Stuff
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_Switcharoo();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SwitchPlacesCauldron(ACauldronActor* Cal1, ACauldronActor* Cal2);

	UFUNCTION(Client, Reliable)
	void Client_DisplaySwitcharooMessage(APotProbPlayerController* PC);
    
    // Love Potion Implementation
    UFUNCTION(Server, Reliable, BlueprintCallable)
    void Server_SetHasCooties(bool bValue);
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_SetHasCooties(bool bValue);
	UFUNCTION(Server, Reliable)
	void Server_GetCootiesTimerHandle();
	float CootiesTimeLeft;
    UFUNCTION(BlueprintCallable)
	bool GetHasCooties() { return bHasCooties; }
	UFUNCTION(BlueprintCallable)
	void ClearCooties();
	UFUNCTION(BlueprintCallable)
	bool GetHasCootiesBefore() { return bHasCootiesBefore; }
	UFUNCTION(BlueprintCallable)
	bool GetIsCootiesInstigator() { return bIsCootiesInstigator; }
	UFUNCTION(BlueprintCallable)
	void SetIsCootiesInstigator(bool bValue);

	// Unstable Potion Implementation
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void StartUnstablePotion();
	UFUNCTION(Client, Reliable)
	void ClientStartUnstablePotion();
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ClearUnstablePotionEffects();
	FTimerHandle UnstablePotionTimer;
	UPROPERTY(Replicated)
	bool bUnstablePotionActive = false;

	// Questionmark Potion Implementation
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void StartQuestionmarkPotion();
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ClearQuestionmarkPotionEffects();
	FTimerHandle QuestionmarkPotionTimer;
	UPROPERTY(Replicated)
	bool bQuestionmarkPotionActive = false;

	// Disguise Potion Implementation
	UFUNCTION(BlueprintCallable)
	void OnRep_IsDisguised();
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerSetIsDisguised(bool Input);
	UFUNCTION(Client, Reliable)
	void ClientSetIsDisguised(bool Input);
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerStartDisguise(float TimerDuration);
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientStartDisguise(float TimerDuration);
	UFUNCTION(BlueprintCallable)
	void StopDisguise();
	UFUNCTION(Client, Reliable)
	void ClientStopDisguise();

	// Ego Potion Implementation
	UFUNCTION( BlueprintCallable)
	void ServerStartEgo(float TimerDuration);
	void EndEgo();
	void SetIsUsingEgo(bool Input) { bIsUsingEgo = Input;}
	bool GetIsUsingEgo() const { return bIsUsingEgo; }
	UFUNCTION(Client, Reliable)
	void ClientOverridePlayerText(bool bIsEgoActive);
	
	UFUNCTION(Server, Reliable)
	void ActivatePotionEffectsFromBucket();
	UFUNCTION(Server, Reliable)
	void ClearPotionEffects();
	TQueue<EPotProbEffects> ActivatedEffects;
	// Translocation Potion Timer Start Func implementation
	UFUNCTION(BlueprintCallable)
	void TeleportToRandomPlayer();
	UFUNCTION(BlueprintCallable)
	void SetTranslocationTimerActive(float TimeValue);
	
	UFUNCTION(BlueprintCallable)
	void DoWheelOfFortune();

	UFUNCTION(BlueprintCallable)
	UUserWidget* AddPotionEffectIconToHUD(UTexture2D* StatusIconTexture, float EffectTime);
	UFUNCTION(BlueprintCallable)
	void RemovePotionEffectIconFromHUD(UUserWidget* StatusIconInstance);
	UFUNCTION(Client, Reliable)
	void ClientAddEgoPotionEffectToHUD(float EffectTime);

	//Troublemakers always get the Frog Recipe
	UPROPERTY(Replicated)
	FRecipeStruct FrogRecipe;

	// Telescope Win Con
	UPROPERTY()
	UUserWidget* WerefrogCountdownWidget;
	UPROPERTY()
	UUserWidget* SwitcherooWarningWidget;
	UFUNCTION(Server, Reliable)
	void Server_IncrementTelescopeSuccesses_GameState();
	UFUNCTION(Client, Reliable)
	void UpdateCauldronMapMarker();
	UFUNCTION(Client, Reliable)
	void ActivateSwitcherooWarning();
	UFUNCTION(Client, Reliable)
	void RemoveSwitcherooWarning();
	UFUNCTION(Client, Reliable)
	void ActivateWerefrogWinConCountdown(TSubclassOf<UPotionObject> FrogPotionClass);
	UFUNCTION(Server, Reliable)
	void ActivateWerefrogWinConEffect(TSubclassOf<UPotionObject> FrogPotionClass);
	float WerefrogCountdownTimer = 0.0f;

	// Lobby functions
	UFUNCTION(Server, Reliable)
	void Server_SetReady(bool status);
	UPROPERTY(ReplicatedUsing = OnRep_IsReady)
	bool bIsReady = false;
	UFUNCTION()
	void OnRep_IsReady();

	// Loop functions
	UFUNCTION(Server, Reliable)
	void Server_SetIsPlayingAgain(bool Status);
	UPROPERTY(Replicated)
	bool bPlayAgain = false;
	const bool GetIsPlayAgain() const { return bPlayAgain; }

	/** Voting Function Start **/
	// Function to be called from the server side to start/end the voting phase
	// Will pass down instructions accordingly to playerController to display UI & stuff
	UFUNCTION(Client, Reliable)
	void SetVotingPhase(bool status);
	UPROPERTY(ReplicatedUsing = OnRep_Vote)
	bool bHasVoted = false;
	UPROPERTY(ReplicatedUsing = OnRep_Vote)
	int VoteCount = 0;
	UFUNCTION()
	void OnRep_Vote();
	UFUNCTION(Server, Reliable)
	void SkipVote();
	/** Voting Function End*/

	UFUNCTION(Server, Reliable)
	void Server_CheckWinCon();
	void CheckWinCon();

	int GetNumRerolls() { return numRerolls;}
	void ResetRerolls() { numRerolls = 0; }

	// Vote Init Actor
	UPROPERTY(Replicated)
	bool bHasInitVote = false;
	UPROPERTY(Replicated)
	bool bCanInitVote = false;

	// End Screen Stats
	UPROPERTY(BlueprintReadWrite, Replicated)
	int NumApprenticesFrogged = 0;
	UPROPERTY(BlueprintReadWrite, Replicated)
	int NumVotingRoundsSurvived = 0;
	UPROPERTY(BlueprintReadWrite, Replicated)
	int NumPotionsCrafted = 0;
	UPROPERTY(BlueprintReadWrite, Replicated)
	int NumTroublemakersVoted = 0;
	UPROPERTY(BlueprintReadWrite, Replicated)
	int NumRoundVotedOutOn = 0;
	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bWasVotedOut = false;
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<APotProbPlayerState*> PlayersVotingHistory;

	// Exposed timers for pause state during voting
	// Translocation Potion Timer
	FTimerHandle TranslocationPotionTimer;
	FTimerHandle DisguisePotionHandle;
	FTimerHandle EgoPotionHandle;
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Potion Architecture */
	UPROPERTY(ReplicatedUsing=OnRep_PlayerPotion)
	TObjectPtr<UPotionObject> PlayerPotion;
	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bHasPotionBeenActivated = false;
	UPROPERTY(BlueprintReadOnly, Category = "Ingredient", ReplicatedUsing=OnRep_IngredientName)
	FName IngredientName;
	UPROPERTY(BlueprintReadOnly, Category = "Ingredient", Replicated)
	bool bHoldingIngredient = false;
	// tracks if first ingredient has been picked up for tutorial alerts
	bool bHasPickedUpFirstIngredient = false;
	UPROPERTY(ReplicatedUsing=OnRep_IngredientIcon)
	UPaperSprite* IngredientIcon;
	// Number of charges that this player has at the moment (We should make this onrep later)
	UPROPERTY(Replicated)
	int32 NumPotionCharges = 0;
	UPROPERTY(Replicated)
	EPotionChargeType CurrentPotionChargeType; 
	// For UI To display if potion is active
	bool bShouldDisplayPotionAsActive = false;
	TArray<FRecipeStruct> SelectedRecipes;
	UPROPERTY(Replicated)
	FRecipeStruct CurrentRecipe;
	
	// Translocation and End Function
    void EndTranslocationPotionTimer();

	/** End Potion Implementation */

	
	// Keeps track of how many times a recipe got rerolled
	UPROPERTY(Replicated)
	int NumTimesRecipeRerolled = 0;
	UPROPERTY(Replicated)
	bool bCanInteractWithObjects = true;
	UPROPERTY(Replicated)
	int numRerolls = 0;
	
	// Love Potion Bool (Cooties)
	UFUNCTION()
	void OnRep_HasCooties();
	UPROPERTY(ReplicatedUsing=OnRep_HasCooties)
	bool bHasCooties = false;
	UPROPERTY(Replicated)
	bool bIsCootiesInstigator = false;
	UPROPERTY(Replicated)
	bool bHasCootiesBefore = false;
	UPROPERTY()
	UUserWidget* CootiesStatusIcon = nullptr;

	// Disguise Potion
	UPROPERTY(ReplicatedUsing=OnRep_IsDisguised, BlueprintReadWrite)
	bool bIsDisguised = false;
	UPROPERTY()
	UUserWidget* DisguiseStatusIcon = nullptr;

	// Ego Potion
	UPROPERTY(Replicated)
	bool bIsUsingEgo = false;
	UPROPERTY()
	UUserWidget* EgoPotionStatusIcon = nullptr;
	
};
