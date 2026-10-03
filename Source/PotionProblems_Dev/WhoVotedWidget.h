// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PotProbGameState.h"
#include "WhoVotedWidget.generated.h"

class UWrapBox;
enum class EAnimModels : uint8;
class UTextBlock;
class UVoteRevealWidget;

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UWhoVotedWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Meta = (BindWidget))
	UWrapBox* VotePlayers;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* VoteTimer;

	

	UFUNCTION()
	void BuildVotingList();

	UFUNCTION()
	void UpdateVoteText();

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	void SetVotedOutPlayerName(FString PlayerName) { VotedOutPlayerName = PlayerName; }

	void SetVoteRevealedClass(TSubclassOf<UVoteRevealWidget> VoteClass) { VoteRevealWidgetClass = VoteClass; }

	void SetWhoVotedMap(TArray<FVotesCast> WhoVotedMap) { WhoVotedForWhoMap = WhoVotedMap; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* CharacterDataTable;

	UPROPERTY(Transient)
	TMap<EAnimModels, UTexture*> CharacterTextures;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPlayersThatVotedWidget> VotePlayerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float VoteRevealDuration = 3.0f;

	FString VotedOutPlayerName;

	TSubclassOf<UVoteRevealWidget> VoteRevealWidgetClass;

	TArray<FVotesCast> WhoVotedForWhoMap;

private:

	float Timer = 0.0f;

	void DestroySelf();

};
