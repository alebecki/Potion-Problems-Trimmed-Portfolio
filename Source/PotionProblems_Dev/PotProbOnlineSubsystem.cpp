// Fill out your copyright notice in the Description page of Project Settings.


#include "PotProbOnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
UPotProbOnlineSubsystem::UPotProbOnlineSubsystem()
{
}

void UPotProbOnlineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// Get the OnlineSubsystem
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	// Get the session interface, if it exists
	SessionInterface = OSS ? OSS->GetSessionInterface() : nullptr;
	if (SessionInterface)
	{
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPotProbOnlineSubsystem::OnCreateSessionComplete);
		SessionInterface->OnStartSessionCompleteDelegates.AddUObject(this, &UPotProbOnlineSubsystem::OnStartSessionComplete);
		SessionInterface->OnEndSessionCompleteDelegates.AddUObject(this, &UPotProbOnlineSubsystem::OnEndSessionComplete);
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPotProbOnlineSubsystem::OnDestroySessionComplete);
		SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UPotProbOnlineSubsystem::OnSessionUserInviteAccepted);
		SessionInterface->OnFindFriendSessionCompleteDelegates[0].AddUObject(this, &UPotProbOnlineSubsystem::OnFindFriendSessionComplete);
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPotProbOnlineSubsystem::OnJoinSessionComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPotProbOnlineSubsystem::OnFindSessionsCompleteDelegate);
	}

	FriendsInterface = OSS ? OSS->GetFriendsInterface() : nullptr;
	if (FriendsInterface)
	{
		FriendsInterface->ReadFriendsList(0, TEXT("inGameAndSessionPlayers"));
	}

	IdentityInterface = OSS ? OSS->GetIdentityInterface() : nullptr;
	if (!IdentityInterface)
	{
		UE_LOG(LogTemp, Error, TEXT("Identity Interface could not be instantiated"));
	}
	SetPresenceJoinable(false);
}

void UPotProbOnlineSubsystem::Deinitialize()
{
	if (SessionInterface)
	{
		SessionInterface->OnCreateSessionCompleteDelegates.RemoveAll(this);
		SessionInterface->OnStartSessionCompleteDelegates.RemoveAll(this);
		SessionInterface->OnEndSessionCompleteDelegates.RemoveAll(this);
		SessionInterface->OnDestroySessionCompleteDelegates.RemoveAll(this);
		SessionInterface->OnSessionUserInviteAcceptedDelegates.RemoveAll(this);
		SessionInterface->OnFindFriendSessionCompleteDelegates[0].RemoveAll(this);
		SessionInterface->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}
	if (FriendsInterface)
	{
	}
	// We no longer need to hold a strong reference to the interfaces
	SessionInterface = nullptr;
	FriendsInterface = nullptr;
	IdentityInterface = nullptr;
	Super::Deinitialize();
}

void UPotProbOnlineSubsystem::HostSession()//TODO: Need to add support for private lobbies accessible via PrivatePasscode
{
	FOnlineSessionSettings SessionSettings;

	// Whether this match is on a LAN or not
	SessionSettings.bIsLANMatch = false;
	// Allow up to 10 players MAX
	if (bIsLobbyPublic)
	{
		SessionSettings.NumPublicConnections = MaxPlayers;
	}
	else
	{
		SessionSettings.NumPublicConnections = MaxPlayers;
		//SessionSettings.NumPrivateConnections = MaxPlayers;
		// If private set the passcode
		SessionSettings.Set("PrivatePasscode", PrivatePasscode, EOnlineDataAdvertisementType::ViaOnlineService);
	}
	// Make this match advertised 
	SessionSettings.bShouldAdvertise = true;
	// This will announce the server using Steam's "presence"
	SessionSettings.bUsesPresence = true;
	// Allow users to join if the game is already in-progress
	SessionSettings.bAllowJoinInProgress = true;
	// Allow users to join via Steam presence if lobby is public
	SessionSettings.bAllowJoinViaPresence = true;
	// Don't require only friends to join from presence
	SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
	// Allow game invites
	SessionSettings.bAllowInvites = true;
	// Use lobbies
	SessionSettings.bUseLobbiesIfAvailable = true;

	//SessionSettings.Set(TEXT("bIsDedicated"), false, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	//SessionSettings.Set(TEXT("bIsLANMatch"), false, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	// Specify the game name so when testing on Spacewar there's no conflicts
	SessionSettings.Set(SETTING_GAMEMODE, FString(TEXT("PotionProblems")), EOnlineDataAdvertisementType::ViaOnlineService);
	
	// Specify the map name
	SessionSettings.Set(SETTING_MAPNAME, bUseBigMap ? FString(TEXT("BigMap")) : FString(TEXT("SmallMap")), EOnlineDataAdvertisementType::ViaOnlineService);

	// Create the session - use the default "GameSession"
	SessionInterface->CreateSession(0, "GameSession", SessionSettings);
}
void UPotProbOnlineSubsystem::FindAllAvailableSessions()
{
	if (!SessionInterface || !IdentityInterface)
	{
		UE_LOG(LogTemp, Error, TEXT("Identity Interface and Session Interface are nullptr"));
		return;
	}

	// We do not support split screen so just setting 0 is fine
	FUniqueNetIdPtr PlayerUniqueId = IdentityInterface->GetUniquePlayerId(0);
	if (!PlayerUniqueId)
	{
		UE_LOG(LogTemp, Error, TEXT("Player Unique Net Id is nullptr"));
		return;
	}
	if (!SessionSearchParam)
	{
		SessionSearchParam = MakeShareable(new FOnlineSessionSearch);
	}

	// We can make these modifiable later
	SessionSearchParam->bIsLanQuery = false;
	SessionSearchParam->MaxSearchResults = 100;

	// I imagine that we are only doing p2p unless we want to change this later. So we will be searching just lobbies
	SessionSearchParam->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// Only search for PotionProblems game instances
	SessionSearchParam->QuerySettings.Set(SETTING_GAMEMODE, FString(TEXT("PotionProblems")), EOnlineComparisonOp::Equals);
	
	// Add search for SETTING_MAPNAME
	SessionSearchParam->QuerySettings.Set(SETTING_MAPNAME, FString(""), EOnlineComparisonOp::NotEquals);
	SessionSearchParam->TimeoutInSeconds = 60.0f; // We can use a different value 
	SessionInterface->FindSessions(*PlayerUniqueId, SessionSearchParam.ToSharedRef());
}

void UPotProbOnlineSubsystem::EndSession()
{
	SessionInterface->DestroySession("GameSession");
}

void UPotProbOnlineSubsystem::RestartSessions()
{
	// When we are reloaded back into the interface, we should reset any sessions that we might still be attached to
	SessionInterface->DestroySession("GameSession");
}

FString UPotProbOnlineSubsystem::GetCurrentPlayerName()
{
	if (!IdentityInterface)
	{
		UE_LOG(LogTemp, Warning, TEXT("Identity Interface is nullptr, can't get player name"));
		return FString("Unknown Player");
	}

	// Get the local player's user ID (player index 0 for the first/only player)
	FUniqueNetIdPtr PlayerUniqueId = IdentityInterface->GetUniquePlayerId(0);
	if (!PlayerUniqueId.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Unique Net Id is invalid"));
		return FString("Unknown Player");
	}

	// Get the player's display name
	return IdentityInterface->GetPlayerNickname(*PlayerUniqueId);
}


void UPotProbOnlineSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		SetPresenceJoinable(true);

		// We need to update this to the level that we are using. Assuming it's MainLevel.
		if (bUseBigMap)
		{
			UGameplayStatics::OpenLevel(GetWorld(), "MainLevel", true, "listen");
		}
		else
		{
			UGameplayStatics::OpenLevel(GetWorld(), "MiniLevel", true, "listen");
		}
	}
}

void UPotProbOnlineSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Create Session Callback to create Session: %s %hs!"), *SessionName.ToString(), bWasSuccessful ? "was successful" : "failed");
	if (bWasSuccessful)
	{
		FString LevelPath = bUseBigMap ? 
			"/Game/PotionProblems/Levels/TestLevels/MainLevel" : 
			"/Game/PotionProblems/Levels/TestLevels/MiniLevel";

		if (!GetWorld()->ServerTravel(LevelPath + "?listen"))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to server travel to new map: %s"), *LevelPath);
		}
		SetPresenceJoinable(true);
		SessionInterface->StartSession(SessionName);
	}
}

void UPotProbOnlineSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinSessionResult)
{
	if (!SessionInterface || JoinSessionResult != EOnJoinSessionCompleteResult::Success)
	{
		return;
	}
	FString URL;
	if (SessionInterface->GetResolvedConnectString(SessionName, URL))
	{
		SetPresenceJoinable(true);

		// Now use ClientTravel to tell our local PlayerController to join the match
		auto PC = UGameplayStatics::GetPlayerController(GetGameInstance(), 0);
		PC->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
	}
}

void UPotProbOnlineSubsystem::JoinFoundSession(int SessionIndex)
{
	if (!SessionInterface || !IdentityInterface)
	{
		return;
	}
	// Get the list of friends who are in-game and in progress
	// This will return immediately since we already did ReadFriendsList

	if (!SessionSearchParam->SearchResults.IsValidIndex(SessionIndex) || SessionSearchParam->SearchResults.Num() == 0)
	{
		return;
	}
	SessionInterface->JoinSession(0, "GameSession", SessionSearchParam->SearchResults[SessionIndex]);
}

void UPotProbOnlineSubsystem::JoinSearchedSession(int SessionIndex)
{
	if (!SessionInterface || !IdentityInterface)
	{
		return;
	}
	// Get the list of friends who are in-game and in progress
	// This will return immediately since we already did ReadFriendsList

	if (!PasscodeSearchParam->SearchResults.IsValidIndex(SessionIndex) || PasscodeSearchParam->SearchResults.Num() == 0)
	{
		return;
	}

	SessionInterface->JoinSession(0, "GameSession", PasscodeSearchParam->SearchResults[SessionIndex]);
}

void UPotProbOnlineSubsystem::OnEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("On End Session Complete Delegate: %hs!"), bWasSuccessful ? "was successful" : "failed");
	if (!bWasSuccessful)
	{
		return;
	}
	SetPresenceJoinable(false);
	SessionInterface->DestroySession("GameSession");
}

void UPotProbOnlineSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Destroy Session Callback to destroy session: %s %hs!"), *SessionName.ToString(), bWasSuccessful ? "was successful" : "failed");
}

void UPotProbOnlineSubsystem::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
	if (!bWasSuccessful)
	{
		return;
	}
	SessionInterface->JoinSession(0, "GameSession", InviteResult);
}

void UPotProbOnlineSubsystem::OnFindSessionsCompleteDelegate(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("On Find Session Complete Delegate: %hs!"), bWasSuccessful ? "was successful" : "failed");
	if (!bWasSuccessful || !SessionSearchParam.IsValid())
	{
		return;
	}
	for (const FOnlineSessionSearchResult& SearchResult : SessionSearchParam->SearchResults)
	{
		FString SessionName = SearchResult.GetSessionIdStr();
		FString HostName;
		FString MapName;
		int32 MaxPlayersInLobby = SearchResult.Session.SessionSettings.NumPublicConnections;
		int32 CurrentPlayers = MaxPlayersInLobby - SearchResult.Session.NumOpenPublicConnections;

		SearchResult.Session.SessionSettings.Get(SETTING_MAPNAME, MapName);
		SearchResult.Session.SessionSettings.Get(FName("HostName"), HostName);

		UE_LOG(LogTemp, Log, TEXT("Found Session - Name: %s, Host: %s, Map: %s, Players: %d/%d"),
			*SessionName, *HostName, *MapName, CurrentPlayers, MaxPlayersInLobby);

		// Here you can populate your UI with the session information
	}
}

void UPotProbOnlineSubsystem::FindSessionsByPasscode(const FString& Passcode, FOnFindSessionsCompleteDelegate Delegate)
{
	PasscodeSearchParam = MakeShareable(new FOnlineSessionSearch);
	
	PasscodeSearchParam->bIsLanQuery = false;
	PasscodeSearchParam->MaxSearchResults = 1;
	
	PasscodeSearchParam->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	// Only search for PotionProblems game instances
	PasscodeSearchParam->QuerySettings.Set(SETTING_GAMEMODE, FString(TEXT("PotionProblems")), EOnlineComparisonOp::Equals);
	
	// Add search for SETTING_MAPNAME
	PasscodeSearchParam->QuerySettings.Set(SETTING_MAPNAME, FString(""), EOnlineComparisonOp::NotEquals);
	PasscodeSearchParam->TimeoutInSeconds = 60.0f; // We can use a different value 
	PasscodeSearchParam->QuerySettings.Set(FName("PrivatePasscode"), Passcode, EOnlineComparisonOp::Equals);

	FOnFindSessionsCompleteDelegate PasscodeSearchDelegate;
	PasscodeSearchDelegate.BindLambda([this, Delegate](bool bWasSuccessful) {
		Delegate.ExecuteIfBound(bWasSuccessful);
		if (SessionInterface)
		{
			SessionInterface->ClearOnFindSessionsCompleteDelegates(this);
		}
	});

	SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(PasscodeSearchDelegate);
	
	SessionInterface->FindSessions(*IdentityInterface->GetUniquePlayerId(0), PasscodeSearchParam.ToSharedRef());
}


void UPotProbOnlineSubsystem::SetPresenceJoinable(bool bIsJoinable)
{
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	// Query our presence and set the game to not joinable
	IOnlinePresencePtr PresenceInterface = OSS ? OSS->GetPresenceInterface() : nullptr;
	if (!PresenceInterface || !IdentityInterface)
	{
		return;
	}
	// Get the UniqueNetID for this player
	auto UserId = IdentityInterface->GetUniquePlayerId(0);
	// Query the presence (this will return instantly for the local player on Steam)
	PresenceInterface->QueryPresence(*UserId);

	// Now get the cached presence
	TSharedPtr<FOnlineUserPresence> Presence;
	if (PresenceInterface->GetCachedPresence(*UserId, Presence) == EOnlineCachedResult::Success)
	{
		// Set our new presence to the cached presence and update whether we're joinable
		FOnlineUserPresenceStatus Status = Presence->Status;
		Status.Properties.Add(TEXT("Joinable"), bIsJoinable);
		PresenceInterface->SetPresence(*UserId, Status);
	}
}

void UPotProbOnlineSubsystem::ReadFriendsList(const FOnReadFriendsListComplete& Delegate)
{
	if (!FriendsInterface || !SessionInterface)
	{
		return;
	}
	FriendsInterface->ReadFriendsList(0, TEXT("inGameAndSessionPlayers"), Delegate);
}

void UPotProbOnlineSubsystem::OnFindFriendSessionComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& SearchResult)
{
	if (!bWasSuccessful)
	{
		return;
	}

	if(SearchResult.Num() == 0)
	{
		return;
	}
	SessionInterface->JoinSession(0, "GameSession", SearchResult[0]);
}

void UPotProbOnlineSubsystem::LoadNewLevel()
{
	if (bUseBigMap)
	{
		GetWorld()->ServerTravel("MainLevel");
	}
	else
	{
		GetWorld()->ServerTravel("MiniLevel");
	}
}


TArray<FString> UPotProbOnlineSubsystem::GetFriendsListNames()
{
	TArray<TSharedRef<FOnlineFriend>> FriendsList;
	FriendsInterface->GetFriendsList(0, TEXT("inGameAndSessionPlayers"), FriendsList);

	TArray<FString> FriendNames;
	for (const auto& Iter : FriendsList)
	{
		FriendNames.Add(Iter.Get().GetDisplayName());
	}

	return FriendNames;
}

void UPotProbOnlineSubsystem::JoinFriendSession(int FriendNum)
{
	if (!FriendsInterface || !SessionInterface)
	{
		return;
	}
	// Get the list of friends who are in-game and in progress
	TArray<TSharedRef<FOnlineFriend>> FriendsList;
	FriendsInterface->GetFriendsList(0, TEXT("inGameAndSessionPlayers"), FriendsList);

	if (!FriendsList.IsValidIndex(FriendNum) || FriendsList.Num() == 0)
	{
		return;
	}
	SessionInterface->FindFriendSession(0, FriendsList[FriendNum].Get().GetUserId().Get());
}

