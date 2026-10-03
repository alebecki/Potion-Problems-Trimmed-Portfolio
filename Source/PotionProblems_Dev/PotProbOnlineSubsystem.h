// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/VoiceInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "PotProbOnlineSubsystem.generated.h"

/**
 *
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UPotProbOnlineSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UPotProbOnlineSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	UFUNCTION(BlueprintCallable)
	void HostSession();
	UFUNCTION(BlueprintCallable)
	void FindAllAvailableSessions();
	UFUNCTION(BlueprintCallable)
	void EndSession();
	UFUNCTION(BlueprintCallable)
	void RestartSessions();
	UFUNCTION(BlueprintCallable)
	FString GetCurrentPlayerName();
	// ONLINE SESSION INTERFACE
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);
	void OnFindSessionsCompleteDelegate(bool bWasSuccessful);
	void FindSessionsByPasscode(const FString& Passcode, FOnFindSessionsCompleteDelegate Delegate);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinSessionResult);
	void JoinFoundSession(int SessionIndex);
	void JoinSearchedSession(int SessionIndex);
	void SetPresenceJoinable(bool bIsJoinable);
	const TArray<FOnlineSessionSearchResult>& GetSessionSearch() { return SessionSearchParam->SearchResults; }
	const TArray<FOnlineSessionSearchResult>& GetPasscodeSearch() { return PasscodeSearchParam->SearchResults; }

	// Online Friends interface
	void ReadFriendsList(const FOnReadFriendsListComplete& Delegate = FOnReadFriendsListComplete());
	void OnFindFriendSessionComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& SearchResult);

	// Load New Level
	void LoadNewLevel();

	TArray<FString> GetFriendsListNames();
	void JoinFriendSession(int FriendNum);

	// Variables
	IOnlineSessionPtr SessionInterface;
	IOnlineFriendsPtr FriendsInterface;
	IOnlineIdentityPtr IdentityInterface;

	// Level Type Variable
	void SetLevelType(bool bShouldUseBigMap) { bUseBigMap = bShouldUseBigMap; }
	void SetMaxPlayers(int MaxPlayersSet) {MaxPlayers = MaxPlayersSet; }
	void SetLobbyPublic(bool bIsPublic) { bIsLobbyPublic = bIsPublic; }
	void SetPrivatePasscode(FString Passcode) { PrivatePasscode = Passcode; }
	bool bUseBigMap = false;
	bool bIsLobbyPublic = true;
	int MaxPlayers = 10;
	FString PrivatePasscode = "";

private:
	TSharedPtr<FOnlineSessionSearch> SessionSearchParam;
	TSharedPtr<FOnlineSessionSearch> PasscodeSearchParam;
};