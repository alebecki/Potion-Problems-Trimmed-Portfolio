// Fill out your copyright notice in the Description page of Project Settings.


#include "PotProbGameMode.h"
#include "IngredientData.h"
#include "PotProbGameState.h"
#include "PotProbPlayerController.h"
#include "PotProbPlayerState.h"
#include "PotProbZDCharacter.h"
#include "PotProbOnlineSubsystem.h"
#include "Chaos/PBDRigidsSOAs.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/PlayerStartPIE.h"
#include "EngineUtils.h"
#include "HUDWidget.h"
#include "OnlineSubsystemUtils.h"
#include "PotProbGameInstance.h"
#include "GameFramework/GameSession.h"
#include "MinigameInitActor.h"

APotProbGameMode::APotProbGameMode()
{
	PlayerStateClass = APotProbPlayerState::StaticClass();
	GameStateClass = APotProbGameState::StaticClass();
	PlayerControllerClass = APotProbPlayerController::StaticClass();
	DefaultPawnClass = APotProbZDCharacter::StaticClass();
	FrogRecipe = FRecipeStruct();

	PrimaryActorTick.bCanEverTick = true;
}

void APotProbGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (!NewPlayer)
	{
		return;
	}
	FUniqueNetIdRepl UniqueNetIDRepl;
	if (NewPlayer->IsLocalController())
	{
		ULocalPlayer* LocalPlayerPtr = NewPlayer->GetLocalPlayer();
		if (LocalPlayerPtr)
		{
			UniqueNetIDRepl = LocalPlayerPtr->GetPreferredUniqueNetId();
		}
		else
		{
			UNetConnection* RemoteNetConnectionPtr = Cast<UNetConnection>(NewPlayer->Player);
			if (RemoteNetConnectionPtr)
			{
				UniqueNetIDRepl = RemoteNetConnectionPtr->PlayerId;
			}
		}
	}
	else
	{
		UNetConnection* RemoteNetConnectionPtr = Cast<UNetConnection>(NewPlayer->Player);
		if (RemoteNetConnectionPtr)
		{
			UniqueNetIDRepl = RemoteNetConnectionPtr->PlayerId;
		}
	}

	TSharedPtr<const FUniqueNetId> UniqueNetID = UniqueNetIDRepl.GetUniqueNetId();
	if (UniqueNetID.Get()->IsValid())
	{
		UPotProbOnlineSubsystem* PotionsOnlineSubsys = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
		if (!PotionsOnlineSubsys || !PotionsOnlineSubsys->SessionInterface || !PotionsOnlineSubsys->IdentityInterface)
		{
			return;
		}

		if (PotionsOnlineSubsys->SessionInterface->RegisterPlayer(FName("GameSession"), *UniqueNetID, false))
		{
			UE_LOG(LogTemp, Warning, TEXT("Successfully Registered Player!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to Register Player!"));
		}
	}

	// If the first player (i.e. the server), setup basic GameState
	if (APotProbGameState* PotProbGameState = Cast<APotProbGameState>(GameState.Get()))
	{
		PotProbGameState->CurrentPhase = EPotProbPhases::PHASE_LOBBY;
	}

	// Assign default values to the player state -> to trigger replication
	if (APotProbPlayerState* PotProbPlayerState = Cast<APotProbPlayerState>(NewPlayer->PlayerState))
	{
		// curr set to true since players are now in the lobby
		PotProbPlayerState->SetCanInteractWithObjects(true);
		PotProbPlayerState->PotProbRole = EPotProbRoles::ROLE_UNASSIGNED;
	}

	// Keep track of current number of players
	UpdateHUDNumPlayersInGame();
}

void APotProbGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	UE_LOG(LogTemp, Warning, TEXT("Player Logged Out! Current Player Count: %d"), NumPlayers);

	// Keep track of current number of players
	UpdateHUDNumPlayersInGame();

	APotProbPlayerState* PlayerState = Exiting->GetPlayerState<APotProbPlayerState>();
	APotProbGameState* ProbGameState = GetGameState<APotProbGameState>();
	
	// TODO: May remove if we end up disabling the quit buttons if you are queue to play again
	if (PlayerState && ProbGameState)
	{
		// Handle play again state
		if (PlayerState->GetIsPlayAgain())
		{
			PlayerState->Server_SetIsPlayingAgain(false);
		}
        
		// Check if we're in an active game (INGAME or VOTE phase)
		if (ProbGameState->CurrentPhase == EPotProbPhases::PHASE_INGAME || 
			ProbGameState->CurrentPhase == EPotProbPhases::PHASE_VOTE)
		{
			// Character will simply disappear when controller is destroyed
			// No additional code needed for that
            
			if (!Exiting->IsLocalPlayerController())
			{
				// Treat player leaving as getting frogged
				PlayerState->SetIsFrogged(true, false);
			}
		}
	}
	
	// This code will only execute if:
	// 1. We're in lobby phase, OR
	// 2. The host is the one leaving
	if (Exiting->IsLocalPlayerController())
	{
		UPotProbOnlineSubsystem* OnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
		if (!OnlineSubsystem)
		{
			return;
		}

		// We need to unregister every single player
		for (const auto& Iter : GetGameState<APotProbGameState>()->PlayerArray)
		{
			if (!Iter.Get())
			{
				continue;
			}

			OnlineSubsystem->SessionInterface->UnregisterPlayer("GameSession", *Iter.Get()->GetUniqueId().GetUniqueNetId());
		}
		OnlineSubsystem->EndSession();
	}
}

FName APotProbGameMode::GetVerifiedIngredientName(FName LookupName)
{
	for (const auto& Iter : IngredientNameSpriteMap)
	{
		if (LookupName == Iter.Key)
		{
			return Iter.Key;
		}
	}

	return NAME_None;
}

void APotProbGameMode::AddRecipeToCurrentlyCrafted(const FRecipeStruct& Recipe)
{
	APotProbGameState* PotionGameState = GetGameState<APotProbGameState>();
	if (!PotionGameState)
	{
		return;
	}
	PotionGameState->SetCurrentlyCraftedNumPotions(CurrentlyCraftedRecipes.Num() + 1);
	if (!CurrentlyCraftedRecipes.Contains(Recipe))
	{
		CurrentlyCraftedRecipes.Add(Recipe);

	}
}

void APotProbGameMode::AddRecipeToCurrentlyCraftedApprentice(const FRecipeStruct& Recipe)
{
	if (!HasAuthority())
	{
		return;
	}
	
	APotProbGameState* PotionGameState = GetGameState<APotProbGameState>();
	if (!PotionGameState)
	{
		return;
	}
	PotionGameState->SetCurrentlyCraftedNumApprenticePotions(PotionGameState->GetCurrentlyCraftedNumApprenticePotions() + 1);
	if (!CurrentlyCraftedApprenticeRecipes.Contains(Recipe))
	{
		CurrentlyCraftedApprenticeRecipes.Add(Recipe);
	}
}

UPaperSprite* APotProbGameMode::GetAssociatedIngredientSprite(FName LookupName)
{
	if (IngredientNameSpriteMap.Contains(LookupName))
	{
		return IngredientNameSpriteMap[LookupName];
	}

	return nullptr;
}

void APotProbGameMode::AddCauldrons(ACauldronActor* Caul)
{
	CauldronActors.Add(Caul);
	Caul->FrogRecipeIngredients = FrogRecipe.Ingredients;

	// set cauldron actors in game state
	if (APotProbGameState* GS = Cast<APotProbGameState>(GameState))
	{
		GS->SetCauldronActors(CauldronActors);
	}
}

void APotProbGameMode::StartTimerForCooties(float Duration)
{
	// First reset the timer since we are passing cooties onto someone 
	GetWorldTimerManager().ClearTimer(CootiesTimerHandle);
	// Set a timer to end cooties if this character cannot cooties someone else in time
	GetWorldTimerManager().SetTimer(CootiesTimerHandle, this, &APotProbGameMode::EndTimerForCooties, Duration );
}

void APotProbGameMode::EndTimerForCooties()
{
	for(const auto& Iter : GetGameState<APotProbGameState>()->PlayerArray)
	{
		if(!IsValid(Iter))
		{
			continue;
		}

		if(APotProbPlayerState* IterPlayerState = Cast<APotProbPlayerState>(Iter.Get()))
		{
			IterPlayerState->ClearCooties();
		}
	}
	GetWorldTimerManager().ClearTimer(CootiesTimerHandle);
}

float APotProbGameMode::GetCootiesTimeRemaining() const
{
	return GetWorldTimerManager().GetTimerRemaining(CootiesTimerHandle);
}


void APotProbGameMode::LoadNewLevel_Implementation()
{
	if (!bShouldLoop)
	{
		return;
	}
	if (UPotProbOnlineSubsystem* OnlineSubsystem = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>())
	{
		OnlineSubsystem->LoadNewLevel();
	}
}

void APotProbGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Initialize player start locations
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
		{
			// Select all player starts that are not in the lobby
			// Lobby player starts must have "Lobby" as their PlayerStartTag
			if (PlayerStart->PlayerStartTag != "Lobby")
			{
				PlayerStartLocations.Add(PlayerStart->GetActorLocation());
			}
		}
	}

	// Extract all ingredient names and put them into an array
	static const FString ContextString(TEXT("Ingredient Data Context"));

	if (!IngredientsDataTable)
	{
		return;
	}
	TArray<FIngredients*> Rows;
	TQueue<FName> IngredientNames;
	int IngredientNamesCount = 0;
	IngredientsDataTable->GetAllRows(ContextString, Rows);
	for (const FIngredients* Row : Rows)
	{
		if (Row)
		{
			IngredientNames.Enqueue(FName(Row->Ingredient));
			++IngredientNamesCount;
			IngredientNameSpriteMap.Add(Row->Ingredient, Row->IngredientSprite);
		}
	}

	static const FString RarityContextString(TEXT("Rarity Data Context"));

	if (!RarityDataTable) {
		return;
	}
	TArray<FRarities*> RarityRows;
	RarityDataTable->GetAllRows(RarityContextString, RarityRows);

	for (const FRarities* Row : RarityRows) {
		if (Row && Row->Rarity != "Special") {
			TotalRarityValue += Row->Percentage;
		}
	}
	
	// Generate 3 unique random numbers
	TArray<TArray<FName>> Combinations;
	TArray<int> FrogIngredientIndices;
	while (FrogIngredientIndices.Num() < FrogIngredientNum)
	{
		int RandNum = FMath::RandRange(0, IngredientNamesCount-1);
		if (!FrogIngredientIndices.Contains(RandNum))
		{
			FrogIngredientIndices.Add(RandNum);
		}
	}
	TArray<FName> FrogCombo;
	for (int k : FrogIngredientIndices)
	{
		FrogCombo.Add(Rows[k]->Ingredient);
	}
	FrogCombo.Sort(CustomRecipeStructSort);
	// Set first combination to be frog potion
	Combinations.Add(FrogCombo);

	// Generate a unique List of Combinations (size 2)
	while (!IngredientNames.IsEmpty())
	{
		FName Popped = *IngredientNames.Peek();
		IngredientNames.Pop();
		--IngredientNamesCount;
		for (int k = 0; k < IngredientNamesCount; ++k)
		{
			FName LocalPop = *IngredientNames.Peek();
			IngredientNames.Pop();
			TArray<FName> Combo = {Popped, LocalPop};
			Combo.Sort(CustomRecipeStructSort);
			Combinations.Add(Combo);
			IngredientNames.Enqueue(LocalPop);
		}
	}

	// Shuffle Order of the Recipe Combinations
	// Swap Shuffle
	// pre incremented to avoid frog potion
	FRandomStream RandomStream;
	RandomStream.GenerateNewSeed();
	for (int i = 1; i < Combinations.Num(); ++i)
	{
		Combinations.Swap(i, RandomStream.RandRange(1, Combinations.Num()-1));
	}
	
	// first index 1 is the # of FROG POTIONS
	int RarityIndex = 0;
	int CurrNumPotions = 0;
	for (int i = 0; i < Combinations.Num(); ++i)
	{
		FRecipeStruct Recipe;
		Recipe.Ingredients = Combinations[i];
		// Generate Sprites for ingredient and add to struct
		for (FName IngredientName : Recipe.Ingredients)
		{
			Recipe.IngredientSprites.Add(GetAssociatedIngredientSprite(IngredientName)->GetBakedTexture());
		}
		
		// Set Potion for Recipe
		if (RarityIndex < PotionsArray.Num() && CurrNumPotions < PotionsArray[RarityIndex].GetDefaultObject()->GetPotionQuantity())
		{
			if (RarityIndex < PotionsArray.Num())
			{
				Recipe.Potion = PotionsArray[RarityIndex];
			}
			++CurrNumPotions;
		}
		if (CurrNumPotions >= PotionsArray[RarityIndex].GetDefaultObject()->GetPotionQuantity())
		{
			++RarityIndex;
			CurrNumPotions = 0;
		}

		// Added Completed Recipe
		FString Print = Recipe.Ingredients[0].ToString() + " " + Recipe.Ingredients[1].ToString() + ": " + Recipe.Potion.Get()->GetName();
		UE_LOG(LogTemp, Log, TEXT("%s"), *Print);
		GeneratedRecipes.Add(Recipe);
	}

	FrogRecipe = GeneratedRecipes[0];

	// Set Total Number of Potion Recipes
	if (APotProbGameState* PotionGameState = GetGameState<APotProbGameState>())
	{
		PotionGameState->SetNumberTotalPotionRecipes(GeneratedRecipes.Num());
		PotionGameState->FrogRecipe = FrogRecipe;
	}

	// Start Timer to Update Lobby Status
	GetWorld()->GetTimerManager().SetTimer(LobbyUpdateTimer, this, &APotProbGameMode::UpdateLobbyStatus, 0.25f, true);
}

void APotProbGameMode::UpdateLobbyStatus()
{
	if (APotProbGameState* ProbGameState = GetGameState<APotProbGameState>())
	{
		if (ProbGameState->CurrentPhase == EPotProbPhases::PHASE_LOBBY)
		{
			FPotProbRoleAssignment* Assignment = RoleAssignments.Find(NumPlayers);
			
			CheckCountdown();
			// Update all properties at once
			ProbGameState->UpdateLobbyStatus(bIsCountdownActive, RemainingCountdownTime, NumPlayers, MinPlayers,
				Assignment != nullptr ? Assignment->ApprenticePlayers : 0,
				Assignment != nullptr ? Assignment->TroublemakerPlayers : 0);
		}
	}
}

void APotProbGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsCountdownActive)
	{
		RemainingCountdownTime -= DeltaSeconds;
		if (RemainingCountdownTime < 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Starting Game!"));
			StartGame();
		}
	}
}

void APotProbGameMode::CheckCountdown()
{
	if (APotProbGameState* ProbGameState = GetGameState<APotProbGameState>())
	{
		bool bAllPlayersReady = true; // Assume all players are ready initially
		for (auto PlayerState : ProbGameState->PlayerArray)
		{
			if (APotProbPlayerState* PotProbPlayerState = Cast<APotProbPlayerState>(PlayerState))
			{
				if (!PotProbPlayerState->bIsReady)
				{
					bAllPlayersReady = false;
					break;
				}
			}
		}
		if (bAllPlayersReady)
		{
			if (NumPlayers >= MaxPlayers && !bIsCountdownActive)
			{
				StartCountdown(LobbyCountdownTimeOnMaxPlayers);
			}
			else if (NumPlayers >= MinPlayers && !bIsCountdownActive)
			{
				StartCountdown(LobbyCountdownTime);
			}
			else if (NumPlayers < MinPlayers && bIsCountdownActive)
			{
				CancelCountdown();
			}
		} // If not all players are ready, cancel the countdown
		else if (bIsCountdownActive)
		{
			CancelCountdown();
		}
	}
}

const TArray<FRecipeStruct>& APotProbGameMode::GenerateSubsetOfPotionRecipes(APotProbPlayerState* PlayerState)
{
	EPotProbRoles CurrentPlayerRole = PlayerState->PotProbRole;
	TArray<FRecipeStruct>* PlayerRecipes = new TArray<FRecipeStruct>();
	while (PlayerRecipes->Num() < 3)
	{
		static const FString RarityContextString(TEXT("Rarity Data Context"));

		if (!RarityDataTable) {
			break;
		}
		TArray<FRarities*> RarityRows;
		RarityDataTable->GetAllRows(RarityContextString, RarityRows);
		int RaritySelector = FMath::RandRange(0, (int)TotalRarityValue);
		
		TArray<FRecipeStruct> RaritySubset;
		int RarityCounter = 0;
		while (RaritySubset.Num() == 0) {
			for (const FRarities* Row : RarityRows) {
				if (Row && Row->Rarity != "Special" && RaritySelector >= RarityCounter && RaritySelector <= RarityCounter + (int)(Row->Percentage)) {
					for (FRecipeStruct Recipe : GeneratedRecipes) {
						if (Recipe.Potion.GetDefaultObject()->GetPotionRarity() == Row->Rarity) {
							RaritySubset.Add(Recipe);
						}
					}
				}
				RarityCounter += (int)(Row->Percentage);
			}
		}
		int RandomIndex = FMath::RandRange(0, RaritySubset.Num() - 1);
		if (!PlayerRecipes->Contains(RaritySubset[RandomIndex]))
		{
			PlayerRecipes->Add(RaritySubset[RandomIndex]);
		}
		// If we have less than 3 recipes we literally cannot give the player recipes this many.
		// Expect the game to break 
		if(GeneratedRecipes.Num() < 3)
		{
			break;
		}
	}

	// frog potion recipe should always be available to troublemaker and does not need to be selected
	/*
	if (CurrentPlayerRole == EPotProbRoles::ROLE_TROUBLEMAKER)
	{
		for(const auto& Iter : GeneratedRecipes)
		{
			// We want to show the Frog Potion always for trouble makers as an option
			if(Iter.Potion.GetDefaultObject()->GetPotionName() == TEXT("Frog Potion"))
			{
				PlayerRecipes->Add(Iter);
				PlayerState->FrogRecipe = Iter;
				break;
			}
		}
	}
	*/
	
	return *PlayerRecipes;
}

void APotProbGameMode::GenerateRecipeAssignmentSheet(APotProbPlayerState* PotionState)
{
	TArray<FRecipeStruct> PlayerRecipes = GenerateSubsetOfPotionRecipes(PotionState);
	PotionState->GivePlayerStatePotionRecipes(PlayerRecipes);
}

void APotProbGameMode::StartCountdown(float CountdownTime)
{
	bIsCountdownActive = true;
	RemainingCountdownTime = CountdownTime;
}

void APotProbGameMode::CancelCountdown()
{
	bIsCountdownActive = false;
	RemainingCountdownTime = LobbyCountdownTime;
}

void APotProbGameMode::StartGame()
{
	CancelCountdown();
	
	//stop users from being able to join the session
	UPotProbOnlineSubsystem* PotionsOnlineSubsys = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
	if (!PotionsOnlineSubsys || !PotionsOnlineSubsys->SessionInterface || !PotionsOnlineSubsys->IdentityInterface)
	{
		return;
	}
	if (FOnlineSessionSettings* SessionSettings = PotionsOnlineSubsys->SessionInterface->GetSessionSettings(FName("GameSession")))
	{
		SessionSettings->bAllowJoinInProgress = false;
		SessionSettings->bShouldAdvertise = false;
		SessionSettings->bAllowJoinViaPresence = false; 
		SessionSettings->bAllowInvites = false;
		PotionsOnlineSubsys->SessionInterface->UpdateSession(FName("GameSession"), *SessionSettings);
	}

	//kick all users who are mid-joining
	/*for (const auto& Iter : GetGameState<APotProbGameState>()->PlayerArray)
	{
		if (!IsValid(Iter))
		{
			continue;
		}
		if (APotProbPlayerState* PS = Cast<APotProbPlayerState>(Iter.Get())){
			if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(PS->GetPlayerController()))
			{
				if (PC->GetNetConnection()->GetConnectionState() != USOCK_Open)
				{
					// Kick the player
					GameSession->KickPlayer(PC,  FText::FromString(TEXT("Game is starting, connection interrupted")));
				}
			}
		}
	}*/
	
	// Retrieve the role assignment for this number of players
	FPotProbRoleAssignment* RoleAssignment = RoleAssignments.Find(NumPlayers);
	if (!RoleAssignment)
	{
		UE_LOG(LogTemp, Warning, TEXT("No role assignment defined for %d players."), NumPlayers);
	}

	int32 NumApprentices = RoleAssignment->ApprenticePlayers;
	int32 NumTroublemakers = RoleAssignment->TroublemakerPlayers;
	if (NumApprentices + NumTroublemakers != NumPlayers)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Mismatch in role assignments: Total assigned roles (%d) does not equal number of players (%d)."),
		       NumApprentices + NumTroublemakers, NumPlayers);
	}

	if (APotProbGameState* ProbGameState = GetGameState<APotProbGameState>())
	{
		ProbGameState->CurrentPhase = EPotProbPhases::PHASE_INGAME;

		TArray<APotProbPlayerState*> ShuffledPlayerStates;
		for (TObjectPtr<APlayerState> CurrPlayerState : ProbGameState->PlayerArray)
		{
			if (APotProbPlayerState* CurrPotProbPlayerState = Cast<APotProbPlayerState>(CurrPlayerState.Get()))
			{
				// Enables interaction with objects
				Cast<APotProbPlayerController>(CurrPotProbPlayerState->GetPlayerController())->ChangeMusic(EPotProbPhases::PHASE_INGAME);
				CurrPotProbPlayerState->SetCanInteractWithObjects(true);
				CurrPotProbPlayerState->FrogRecipe = FrogRecipe;
				ShuffledPlayerStates.Add(CurrPotProbPlayerState);
			}
		}
		
		// Shuffle the player states to randomize the role assignment
		FMath::RandInit(FDateTime::Now().GetMillisecond());
		for (int32 i = 0; i < ShuffledPlayerStates.Num(); ++i)
		{
			int32 RandomIndex = UKismetMathLibrary::RandomIntegerInRange(i, ShuffledPlayerStates.Num() - 1);
			ShuffledPlayerStates.Swap(i, RandomIndex);
		}

		// Assign roles to players
		int CurrPlayerIndex = 0;
		while (CurrPlayerIndex < NumPlayers)
		{
			if (CurrPlayerIndex < NumApprentices)
			{
				ShuffledPlayerStates[CurrPlayerIndex]->PotProbRole = EPotProbRoles::ROLE_APPRENTICE;
			}
			else
			{
				ShuffledPlayerStates[CurrPlayerIndex]->PotProbRole = EPotProbRoles::ROLE_TROUBLEMAKER;
			}
			if (APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(
				ShuffledPlayerStates[CurrPlayerIndex]->GetPlayerController()))
			{
				PlayerController->Client_UpdateLocalLobbyStatusUI();
				PlayerController->StopMusic();
				PlayerController->PlayStartChime();
				if (PlayerController->IsLocalController())
				{
					ShuffledPlayerStates[CurrPlayerIndex]->OnRep_PotProbRole();
				}

				// Increment Player Screen to InGame
				PlayerController->Client_IncrementHUDGameState();
			}
			++CurrPlayerIndex;
		}

		FTimerHandle handle;
		GetWorldTimerManager().SetTimer(handle, this, &APotProbGameMode::UpdatePlayerNameTagColor, 1.0f, false);

		// set up telescopes
		TArray<AActor*> FoundTelescopeActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMinigameInitActor::StaticClass(), FoundTelescopeActors);
		for (AActor* Actor : FoundTelescopeActors)
		{
			if (AMinigameInitActor* Telescope = Cast<AMinigameInitActor>(Actor))
			{
				Telescope->SetRandomIngredient();
			}
		}

	}
	

	// Teleport players to their respective starting locations
	TeleportPlayersToStart();
	ResetCanVoteInitCountdown();
	UpdateHUDNumPlayersInGame();
}

void APotProbGameMode::UpdatePlayerNameTagColor()
{
	if (APotProbGameState* ProbGameState = GetGameState<APotProbGameState>())
	{
		for (TObjectPtr<APlayerState> CurrPlayerState : ProbGameState->PlayerArray)
		{
			if (APotProbZDCharacter* Char = Cast<APotProbZDCharacter>(CurrPlayerState->GetPawn()))
			{
				Char->UpdatePlayerNameTagColor_Multicast();
			}
		}
	}
}

void APotProbGameMode::TeleportPlayersToStart()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APotProbPlayerController::StaticClass(), FoundActors);
	
	for (AActor* Actor : FoundActors)
	{
		if (APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(Actor))
		{
			if (APotProbZDCharacter* PotProbZdCharacter = Cast<APotProbZDCharacter>(PlayerController->GetCharacter()))
			{
				PotProbZdCharacter->MulticastEnablePostProcessComponent();

				int RandomStartIndex = FMath::RandRange(0, PlayerStartLocations.Num() - 1);
				if (RandomStartIndex < PlayerStartLocations.Num())
				{
					FVector StartLocation = PlayerStartLocations[RandomStartIndex];
					PotProbZdCharacter->SetActorLocation(StartLocation, false, nullptr, ETeleportType::TeleportPhysics);

					// Reset velocity
					PotProbZdCharacter->GetCharacterMovement()->Velocity = FVector::ZeroVector;
				}
			}
		}
	}
}

AActor* APotProbGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// Choose a player start
	APlayerStart* FoundPlayerStart = nullptr;
	UClass* PawnClass = GetDefaultPawnClassForController(Player);
	APawn* PawnToFit = PawnClass ? PawnClass->GetDefaultObject<APawn>() : nullptr;
	TArray<APlayerStart*> UnOccupiedStartPoints;
	TArray<APlayerStart*> OccupiedStartPoints;
	UWorld* World = GetWorld();
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		APlayerStart* PlayerStart = *It;

		if (PlayerStart->IsA<APlayerStartPIE>())
		{
			// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
			FoundPlayerStart = PlayerStart;
			break;
		}
		if (PlayerStart->PlayerStartTag == "Lobby")
		{
			FVector ActorLocation = PlayerStart->GetActorLocation();
			const FRotator ActorRotation = PlayerStart->GetActorRotation();
			if (!World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation))
			{
				UnOccupiedStartPoints.Add(PlayerStart);
			}
			else if (World->FindTeleportSpot(PawnToFit, ActorLocation, ActorRotation))
			{
				OccupiedStartPoints.Add(PlayerStart);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Could not find any Lobby Spawns!"));
		}
	}
	if (FoundPlayerStart == nullptr)
	{
		if (UnOccupiedStartPoints.Num() > 0)
		{
			FoundPlayerStart = UnOccupiedStartPoints[FMath::RandRange(0, UnOccupiedStartPoints.Num() - 1)];
		}
		else if (OccupiedStartPoints.Num() > 0)
		{
			FoundPlayerStart = OccupiedStartPoints[FMath::RandRange(0, OccupiedStartPoints.Num() - 1)];
		}
	}
	return FoundPlayerStart;
}

bool APotProbGameMode::DetermineIfEndCooties()
{
	bool bHaveAllPlayersReceivedCooties = true;
	APotProbPlayerState* CootiesInstigator = nullptr;
	for(const auto& Iter : GetGameState<APotProbGameState>()->PlayerArray)
	{
		if(!IsValid(Iter))
		{
			continue;
		}

		if(APotProbPlayerState* IterPlayerState = Cast<APotProbPlayerState>(Iter.Get()))
		{
			if(IterPlayerState->GetIsCootiesInstigator())
			{
				CootiesInstigator = IterPlayerState;
			}
			if(!IterPlayerState->GetHasCootiesBefore())
			{
				bHaveAllPlayersReceivedCooties = false;
			}
		}
	}
	if(!bHaveAllPlayersReceivedCooties)
	{
		return false;
	}
	if (bHaveAllPlayersReceivedCooties && CootiesInstigator)
	{
		EndTimerForCooties();
	}
	return true;
}

void APotProbGameMode::CheckWin()
{
	//FPotProbRoleAssignment* RoleAssignment = RoleAssignments.Find(NumPlayers);
	if (APotProbGameState* ProbGameState = GetGameState<APotProbGameState>())
	{
		int NumNoneFroggedApprentices = ProbGameState->NumApprentices - ProbGameState->numApprenticeFrogged;
		int NumNoneFroggedTroublemakers = ProbGameState->NumTroublemakers - ProbGameState->numTroublemakerFrogged;
		if (NumNoneFroggedApprentices <= NumNoneFroggedTroublemakers)
		{
			if (bWinCon_TroublemakerShouldOutNumberApprentice &&
				NumNoneFroggedApprentices < NumNoneFroggedTroublemakers)
			{
				EndGame(false, false);
			} else if (!bWinCon_TroublemakerShouldOutNumberApprentice)
			{
				EndGame(false, false);
			}
		}
		else if (NumNoneFroggedTroublemakers <= 0)
		{
			EndGame(true, true);
		}
		else
		{
			// We compare the number of currently crafted recipes to the number of total recipes in the game
			const int32 CurrentlyCrafted = ProbGameState->GetCurrentlyCraftedNumApprenticePotions();
			// Offset by 1 to take account of App not being able to craft frog potions
			int32 TotalRecipes;
			if (NumPlayers <= 2) {
				TotalRecipes = 2;
			}
			else {
				TotalRecipes = ProbGameState->NumApprentices * ProbGameState->GetApprenticeWinMultiplier();
			}

			if (CurrentlyCrafted == TotalRecipes)
			{
				EndGame(true, false);
			}
		}
	}
}

void APotProbGameMode::EndGame(bool apprenticeWon, bool bApprenticeWonByVoting)
{
	if (APotProbGameState* ProbGameState = GetGameState<APotProbGameState>())
	{
		//re-allow players to join the session and make it discoverable
		/*UPotProbOnlineSubsystem* PotionsOnlineSubsys = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
		if (!PotionsOnlineSubsys || !PotionsOnlineSubsys->SessionInterface || !PotionsOnlineSubsys->IdentityInterface)
		{
			return;
		}
		if (FOnlineSessionSettings* SessionSettings = PotionsOnlineSubsys->SessionInterface->GetSessionSettings(FName("GameSession")))
		{
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true; 
			SessionSettings->bAllowInvites = true;
			PotionsOnlineSubsys->SessionInterface->UpdateSession(FName("GameSession"), *SessionSettings);
		}*/
		
		ProbGameState->EndGame(apprenticeWon, bApprenticeWonByVoting);
	}
}

void APotProbGameMode::UpdateHUDNumPlayersInGame()
{
	for (APlayerState* PlayerState : GetGameState<APotProbGameState>()->PlayerArray)
	{
		if (APotProbPlayerController* PotProbController = Cast<APotProbPlayerController>(PlayerState->GetPlayerController()))
		{
			PotProbController->Client_UpdateNumInGamePlayers(NumPlayers);
		}
	}

	// Start Timer to kick remaining players in game if we dont have enough to play again
	if (APotProbGameState* PotProbGameState = GetGameState<APotProbGameState>())
	{
		if (PotProbGameState->CurrentPhase == EPotProbPhases::PHASE_END)
		{
			if (NumPlayers < GetNumMinRequiredPlayers())
			{
				UpdateLoopTimers(true);
			}
		}
	}
}

void APotProbGameMode::ClearLoopTimers()
{
	GetWorldTimerManager().ClearTimer(KickAllTimerHandle);
	GetWorldTimerManager().ClearTimer(LoadNewLevelTimerHandle);
	GetWorldTimerManager().ClearTimer(HostMigrateTimerHandle);
}

void APotProbGameMode::UpdateLoopTimers(bool bKickAll)
{
	ClearLoopTimers();

	if (bKickAll)
	{
		GetWorldTimerManager().SetTimer(KickAllTimerHandle, this, &APotProbGameMode::KickAll, SecondsKickAll, false);
	}
	else
	{
		GetWorldTimerManager().SetTimer(LoadNewLevelTimerHandle, this, &APotProbGameMode::KickAndLoadNewLevel, SecondsLoadNewLevel, false);
	}

	for (APlayerState* PlayerState : GetGameState<APotProbGameState>()->PlayerArray)
	{
		if (APotProbPlayerController* PotProbController = Cast<APotProbPlayerController>(PlayerState->GetPlayerController()))
		{
			PotProbController->Client_UpdateLoopReason(bKickAll);
		}
	}
}

void APotProbGameMode::KickAll()
{
	for (APlayerState* PlayerState : GetGameState<APotProbGameState>()->PlayerArray)
	{
		if (APotProbPlayerController* PotProbController = Cast<APotProbPlayerController>(PlayerState->GetPlayerController()))
		{
			PotProbController->LeaveSessionAndReturnToMenu();
		}
	}
}

void APotProbGameMode::KickAndLoadNewLevel()
{
	// Do host migration
	APotProbPlayerState* HostingPlayer = Cast<APotProbPlayerState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
	bFoundNewHost = false; 
	if (HostingPlayer)
	{
		for (APlayerState* PlayerState : GetGameState<APotProbGameState>()->PlayerArray)
		{
			if (APotProbPlayerState* PotProbPlayerState = Cast<APotProbPlayerState>(PlayerState))
			{
				if (PotProbPlayerState != HostingPlayer)
				{
					if (PotProbPlayerState->GetIsPlayAgain())
					{
						if (UPotProbGameInstance* PotGameInstance = Cast<UPotProbGameInstance>(GetGameInstance()))
						{
							// Begin host migration
							if (!bFoundNewHost)
							{
								bFoundNewHost = true;
								NewHostStr = *PotProbPlayerState->GetUniqueId()->ToString();
								PotGameInstance->StartHost();
								GetWorld()->GetTimerManager().SetTimer(HostMigrateTimerHandle, this, &APotProbGameMode::KickAll, SecondsHostMigrate, false);
							}
						}
					}
					else
					{
						// Kick all non host players
						if (APotProbPlayerController* PotProbController = Cast<APotProbPlayerController>(PlayerState->GetPlayerController()))
						{
							PotProbController->LeaveSessionAndReturnToMenu();
						}
					}
				}
			}
		}
		
		if (HostingPlayer->GetIsPlayAgain())
		{
			UPotProbOnlineSubsystem* PotionsOnlineSubsys = GetGameInstance()->GetSubsystem<UPotProbOnlineSubsystem>();
			if (!PotionsOnlineSubsys || !PotionsOnlineSubsys->SessionInterface || !PotionsOnlineSubsys->IdentityInterface)
			{
				return;
			}
			if (FOnlineSessionSettings* SessionSettings = PotionsOnlineSubsys->SessionInterface->GetSessionSettings(FName("GameSession")))
			{
				SessionSettings->bAllowJoinInProgress = true;
				SessionSettings->bShouldAdvertise = true;
				SessionSettings->bAllowJoinViaPresence = true; 
				SessionSettings->bAllowInvites = true;
				PotionsOnlineSubsys->SessionInterface->UpdateSession(FName("GameSession"), *SessionSettings);
			}
			LoadNewLevel();
		}
	}
}

void APotProbGameMode::HostMigration(const FString& MigrationDir)
{
	for (APlayerState* PlayerState : GetGameState<APotProbGameState>()->PlayerArray)
	{
		if (APotProbPlayerState* PotProbPlayerState = Cast<APotProbPlayerState>(PlayerState))
		{
			if (!PotProbPlayerState->GetIsPlayAgain())
			{
				if (APotProbPlayerController* PotProbController = Cast<APotProbPlayerController>(PlayerState->GetPlayerController()))
				{
					PotProbController->ClientTravel(MigrationDir, TRAVEL_Absolute);
				}
			}
		}
	}
}

void APotProbGameMode::ResetCanVoteInitCountdown()
{
	if (AGameStateBase* GS = GetGameState<AGameStateBase>())
	{
		// Loop through all PlayerStates
		for (APlayerState* PlayerState : GS->PlayerArray)
		{
			if (APotProbPlayerState* PotProbPlayerState = Cast<APotProbPlayerState>(PlayerState))
			{
				PotProbPlayerState->bCanInitVote = false;
			}
		}

		// Start the countdown
		GetWorld()->GetTimerManager().SetTimer(CanVoteInitTimerHandle, this, &APotProbGameMode::AllowCanVoteInit, AllowVoteInitDuration, false);
	}
	
}

void APotProbGameMode::AllowCanVoteInit()
{
	if (AGameStateBase* GS = GetGameState<AGameStateBase>())
	{
		// Loop through all PlayerStates
		for (APlayerState* PlayerState : GS->PlayerArray)
		{
			if (APotProbPlayerState* PotProbPlayerState = Cast<APotProbPlayerState>(PlayerState))
			{
				PotProbPlayerState->bCanInitVote = true;
			}
		}
	}
}

void APotProbGameMode::SetPlayerStateFrogRecipe() const
{
	// set frog recipe of all player states
	if (AGameStateBase* GS = GetGameState<AGameStateBase>())
	{
		// Loop through all PlayerStates
		for (APlayerState* PlayerState : GS->PlayerArray)
		{
			if (APotProbPlayerState* PotProbPlayerState = Cast<APotProbPlayerState>(PlayerState))
			{
				PotProbPlayerState->FrogRecipe = FrogRecipe;
			}
		}
	}
}





