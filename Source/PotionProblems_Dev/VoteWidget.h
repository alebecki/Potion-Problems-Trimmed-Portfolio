// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VoteWidget.generated.h"

/**
 * 
 */

class APotProbPlayerState;
class UWrapBox;
enum class EAnimModels : uint8;
class UTextBlock;
class UVerticalBox;
class UHorizontalBox;
class UButton;
class UScrollBox;
class UVerticalBox;
class UEditableTextBox;
class UVotePlayerWidget;


UCLASS()
class POTIONPROBLEMS_DEV_API UVoteWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(Meta = (BindWidget))
	UWrapBox* VotePlayers;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* VoteTimer;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* VoteInsturctions;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* VotedText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TroublemakerText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* FrogText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TroublemakerNumText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PlayerSelectedText;
	UPROPERTY(Meta = (BindWidget))
	UButton* LockVoteButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* SkipButton;

	/* Chat Assets */
	UPROPERTY(Meta = (BindWidget))
	UScrollBox* ChatScrollBox;
	UPROPERTY(Meta = (BindWidget))
	UVerticalBox* ChatMessages;
	UPROPERTY(Meta = (BindWidget))
	UEditableTextBox* VoteChatInput;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UVoteMessageWidget> SendMessageWidgetClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UVoteMessageWidget> RecieveMessageWidgetClass;
	/* End Chat Assets */

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void BuildVotingList(bool bCanVote, bool bIsTieBreaker, const TArray<APotProbPlayerState*>& TiedPlayers);

	UFUNCTION()
	void UpdateVoteText();

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerSelectedText(const FString& PlayerName);
	
	UFUNCTION(BlueprintCallable)
	void SetPlayerSelected(const FString& PlayerName);
	
	/* Chat Functions */
	void AddChatMessage(const FString& MessageRecieverName, const FString& MessageSenderName, EAnimModels Model, const FString& MessageContent);
	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	/* End Chat Functions */
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* CharacterDataTable;

	UPROPERTY(Transient)
	TMap<EAnimModels, UTexture*> CharacterTextures;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UVotePlayerWidget> VotePlayerClass;

	UPROPERTY(BlueprintReadWrite)
	FString PlayerSelected = "";

	UFUNCTION(BlueprintCallable)
	void LockInVote();
	
	UFUNCTION(BlueprintCallable)
	void SkipVote();

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	TArray<APotProbPlayerState*> ConvertPlayerArray(const TArray<APlayerState*>& PlayerArray);
};
