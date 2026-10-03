// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CauldronActor.h"
#include "PotProbEnums.h"
#include "PotionObject.h"
#include "GameFramework/GameMode.h"
#include "PotProbGameMode.generated.h"

class APotProbPlayerState;

USTRUCT(BlueprintType)
struct FRecipeStruct
{
	GENERATED_BODY();
	UPROPERTY(BlueprintreadWrite)
	TArray<FName> Ingredients;
	UPROPERTY(BlueprintreadWrite)
	TArray<UTexture2D*> IngredientSprites;
	UPROPERTY(BlueprintreadWrite)
	TSubclassOf<UPotionObject> Potion = nullptr;

	bool operator==(const FRecipeStruct& Other) const
	{
		return Potion == Other.Potion;
	}
};
USTRUCT(BlueprintType)
struct FPotProbRoleAssignment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ApprenticePlayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TroublemakerPlayers;
};
USTRUCT(BlueprintType)
struct FEffectBucket
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EPotProbEffects> Effects;
    
	// Optional: Override operator for easier access
	EPotProbEffects operator[](int32 Index) { return Effects[Index]; }
};

/**
 *
 */

class UPaperSprite;
UCLASS()
class POTIONPROBLEMS_DEV_API APotProbGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	APotProbGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	void CheckWin();
	TArray<FRecipeStruct> GetPotionRecipes() const { return GeneratedRecipes; }
	void SetPlayerStateFrogRecipe() const;
	TArray<FRecipeStruct> GetCurrentlyCraftedRecipes() const { return CurrentlyCraftedRecipes; }
	TArray<FRecipeStruct> GetCurrentlyCraftedApprenticeRecipes() const { return CurrentlyCraftedApprenticeRecipes; }
	TArray<FEffectBucket> GetUnstablePotionEffectBuckets() const { return UnstablePotionEffectBuckets; }
	FName GetVerifiedIngredientName(FName LookupName);
	void AddRecipeToCurrentlyCrafted(const FRecipeStruct& Recipe);
	void AddRecipeToCurrentlyCraftedApprentice(const FRecipeStruct& Recipe);
	UPaperSprite* GetAssociatedIngredientSprite(FName LookupName);
	void SetHasDistributedPotions(bool Input) { bHasDistributedRecipes = Input;}
	// Check whether start/cancel countdown based on playerCount and readyStatus
	void CheckCountdown();
	const TArray<FRecipeStruct>& GenerateSubsetOfPotionRecipes(APotProbPlayerState* PlayerState);
	void GenerateRecipeAssignmentSheet(APotProbPlayerState* PotionState);
	
	void TeleportPlayersToStart();
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	void DEBUG_StartGame() {StartGame();}

	int GetNumMinRequiredPlayers() const { return MinPlayers; }
	TArray<class ACauldronActor*> GetCauldrons() const {return CauldronActors;}
	void AddCauldrons(ACauldronActor* Caul);

	// Activates Cootie Timer
	UFUNCTION(BlueprintCallable)
	void StartTimerForCooties(float Duration);
	// End Timer Function for Cooties
	void EndTimerForCooties();
	bool DetermineIfEndCooties();

	TMap<int, FPotProbRoleAssignment> GetRoleAssignments() { return RoleAssignments; }
	
	// Vote Init Counters
	void ResetCanVoteInitCountdown();
	void AllowCanVoteInit();
	FTimerHandle CanVoteInitTimerHandle;

	// Potion Generation Function
	bool static CustomRecipeStructSort(const FName& A, const FName& B) { return A.ToString() < B.ToString(); }
	
	/* GameLoop Functions */
	void ClearLoopTimers();
	void UpdateLoopTimers(bool bKickAll);
	UFUNCTION(Server, Reliable)
	void LoadNewLevel();

	/* Debug Functions */
	// ##############################################################
	TArray<TSubclassOf<UPotionObject>> GetPotionsArray() { return PotionsArray; }
	// ##############################################################
	/* End Debug Function */

	// Timer Handle for keeping track of the duration of cooties
	FTimerHandle CootiesTimerHandle;
	float GetCootiesTimeRemaining() const;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	void UpdateLobbyStatus();
	FTimerHandle LobbyUpdateTimer;
	
	// The key is the number of players
	// The value is the desired assignment for the number of players givne
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PotionProblems")
	TMap<int ,FPotProbRoleAssignment> RoleAssignments;

	// The maximum amount of players that should be in the game
	// If the limit is reached, the game starts after a set time
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PotionProblems")
	int32 MaxPlayers = 10;

	// The timer will be set to this number (in seconds) if maximum amount of players has been reached
	// if set to 0, the game starts immediately after the maximum amount of players has been reached
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PotionProblems")
	float LobbyCountdownTimeOnMaxPlayers = 15.0f;

	// The minimum amount of players required to start the game (and the timer)
	// If player count drops below this number, the start countdown will be terminated
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PotionProblems")
	int32 MinPlayers = 10;

	// The timer will be set to this number (in seconds) if minimum amount of players required has been reached
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PotionProblems")
	float LobbyCountdownTime = 30.0f;

	// This is the boolean variable that determines whether the Troublemaker should outnumber the Apprentice for win con
	// If true, the number of unfrogged Troublemaker will only win if they outnumber (>) the number of unfrogged Apprentice
	// If false, the number of unfrogged Troublemaker will only when they equal or outnumber (>=) the number of the unfrogged Apprentice
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PotionProblems")
	bool bWinCon_TroublemakerShouldOutNumberApprentice = true;

	// This is the effect buckets that will be used to determine the effects of the unstable potion
	// Each bucket contains a list of possible effects that can be applied to the player
	// When drank, the player will receive one of the effects from EACH bucket
	// The number of effects the player receives is equal to the number of buckets
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PotionProblems")
	TArray<FEffectBucket> UnstablePotionEffectBuckets;

	// This is the variable that controls HOW LONG after each round starts
	// will the player be able to interact with the interactive objects
	// to initiate a vote. Each player can only initiate it once per GAME.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PotionProblems")
	float AllowVoteInitDuration = 45.0f;

	// boolean used to determine if the game should loop
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PotionProblems")
	bool bShouldLoop = false;
	
	UPROPERTY()
	TArray<FVector> PlayerStartLocations;

	UPROPERTY()
	bool bIsCountdownActive = false;

	UPROPERTY()
	float RemainingCountdownTime = LobbyCountdownTime;

	/* Major Game Loop Functions */
	void StartCountdown(float CountdownTime);
	void CancelCountdown();
	void StartGame();
	void UpdatePlayerNameTagColor();
	void EndGame(bool apprenticeWon, bool bApprenticeWonByVoting);
	
	// Loop Functions
	void UpdateHUDNumPlayersInGame();
	void KickAll();
	void KickAndLoadNewLevel();
	void HostMigration(const FString& MigrationDir);
	UPROPERTY(EditDefaultsOnly, Category = "Loop Game")
	float SecondsKickAll = 15.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Loop Game")
	float SecondsLoadNewLevel = 10.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Loop Game")
	float SecondsHostMigrate = 10.0f;

	// Loop Variables
	UPROPERTY()
	FString NewHostStr;
	bool bFoundNewHost = false;
	FTimerHandle KickAllTimerHandle;
	FTimerHandle LoadNewLevelTimerHandle;
	FTimerHandle HostMigrateTimerHandle;

	// Potion Generation and other information
	UPROPERTY(EditDefaultsOnly, Category = "Recipes")
	UDataTable* IngredientsDataTable;
	UPROPERTY(EditDefaultsOnly, Category = "Recipes")
	TArray<TSubclassOf<UPotionObject>> PotionsArray;
	// Number of recipes to initially generate
	UPROPERTY(EditDefaultsOnly, Category = "Recipes")
	int32 NumberPotionsToGen;
	// Total Recipes that have been generated
	TArray<FRecipeStruct> GeneratedRecipes;
	// Currently crafted recipes by the players
	TArray<FRecipeStruct> CurrentlyCraftedRecipes;
	//Currently crafted non-frog recipes
	TArray<FRecipeStruct> CurrentlyCraftedApprenticeRecipes;
	// Number of ingredients required to make the frog potion
	UPROPERTY(EditDefaultsOnly, Category = "Recipes")
	int FrogIngredientNum = 3;
	// Frog Potion Recipe
	FRecipeStruct FrogRecipe;
	TMap<FName, UPaperSprite*> IngredientNameSpriteMap;
	bool bHasDistributedRecipes = false;
	UPROPERTY()
	TArray<class ACauldronActor*> CauldronActors;

	float TotalRarityValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipes")
	UDataTable* RarityDataTable;
};
