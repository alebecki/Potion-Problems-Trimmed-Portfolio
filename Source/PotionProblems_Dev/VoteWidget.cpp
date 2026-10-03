// Fill out your copyright notice in the Description page of Project Settings.


#include "VoteWidget.h"

//#include "AsyncTreeDifferences.h"
#include "CharacterData.h"
#include "ChatFilterLibrary.h"
#include "PotProbGameState.h"
#include "PotProbPlayerController.h"
#include "PotProbPlayerState.h"
#include "PotProbZDCharacter.h"
#include "ToggleButton.h"
#include "VoteMessageWidget.h"
#include "VotePlayerWidget.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UVoteWidget::BuildVotingList(bool bCanVote, bool bIsTieBreaker, const TArray<APotProbPlayerState*>& TiedPlayers)
{
	VotePlayers->ClearChildren();
	
	if (APotProbPlayerState* OwningPlayerState = Cast<APotProbPlayerState>(GetOwningPlayerState()))
	{
		bool bHasVoted = false;
		if (OwningPlayerState->bHasVoted)
		{
			bHasVoted = true;
		}
		// Check if the player's role is ROLE_TROUBLEMAKER or if they are frogged
		if (bHasVoted)
		{
			TroublemakerText->SetVisibility(ESlateVisibility::Collapsed);
			FrogText->SetVisibility(ESlateVisibility::Collapsed);
			VoteInsturctions->SetVisibility(ESlateVisibility::Collapsed);
			VotedText->SetVisibility(ESlateVisibility::Visible);
		}
		else if (OwningPlayerState->bIsFrogged) {
			TroublemakerText->SetVisibility(ESlateVisibility::Collapsed);
			FrogText->SetVisibility(ESlateVisibility::Visible);
			VoteInsturctions->SetVisibility(ESlateVisibility::Collapsed);
			VotedText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else if (OwningPlayerState->PotProbRole == EPotProbRoles::ROLE_TROUBLEMAKER)
		{
			// The player is a Troublemaker, so execute the desired logic here
			TroublemakerText->SetVisibility(ESlateVisibility::Visible);
			FrogText->SetVisibility(ESlateVisibility::Collapsed);
			VoteInsturctions->SetVisibility(ESlateVisibility::Collapsed);
			VotedText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			TroublemakerText->SetVisibility(ESlateVisibility::Collapsed);
			FrogText->SetVisibility(ESlateVisibility::Collapsed);
			VoteInsturctions->SetVisibility(ESlateVisibility::Visible);
			VotedText->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (const APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
		{
			if (GameState->NumTroublemakers > 1)
			{
				TroublemakerNumText->SetText(FText::FromString(FString::Printf(TEXT("Tip: There are %d troublemakers in this game."), GameState->NumTroublemakers)));
			} else
			{
				TroublemakerNumText->SetText(FText::FromString(FString::Printf(TEXT("Tip: There is 1 troublemaker in this game."))));
			}
			
			if (bCanVote && !bHasVoted && !OwningPlayerState->bIsFrogged)
			{
				SkipButton->SetIsEnabled(true);
			} else
			{
				SkipButton->SetIsEnabled(false);
				LockVoteButton->SetIsEnabled(false);
			}
			
			const TArray<APotProbPlayerState*> PlayerArray = bIsTieBreaker ? TiedPlayers : ConvertPlayerArray(GameState->PlayerArray);
			for (const auto it : PlayerArray)
			{
				if (APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(it))
				{
					bool bIsSelf = OwningPlayerState->GetPlayerName() == PlayerState->GetPlayerName() ? true : false;
					TObjectPtr<UVotePlayerWidget> NewVotePlayerWidget = CreateWidget<UVotePlayerWidget>(this, VotePlayerClass);
					NewVotePlayerWidget->VoteWidget = this;
					UWrapBoxSlot* NewButtonSlot = VotePlayers->AddChildToWrapBox(NewVotePlayerWidget);
					NewVotePlayerWidget->CheckBoxVoting->SetCheckedState(PlayerState->bHasVoted ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
					NewVotePlayerWidget->PlayerName->SetText(FText::FromString(PlayerState->GetPlayerName()));
					NewVotePlayerWidget->VoteCount->SetText(FText::AsNumber(PlayerState->VoteCount));
					NewVotePlayerWidget->SetVoteCounts(PlayerState->VoteCount);
					// If the player has already voted for this round, is a frog, or not yet time to vote, disabling interaction
					if (!bCanVote || bHasVoted || OwningPlayerState->bIsFrogged || PlayerState->bIsFrogged || bIsSelf)
					{
						NewVotePlayerWidget->VoteButton->SetIsEnabled(false);
					}
					if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(PlayerState->GetPawn()))
					{
						if (PlayerState->bIsFrogged)
						{
							NewVotePlayerWidget->PlayerHead->SetBrushFromTexture(Cast<UTexture2D>(CharacterTextures[EAnimModels::FROGGED_MODEL]));
						}
						else
						{
							NewVotePlayerWidget->PlayerHead->SetBrushFromTexture(Cast<UTexture2D>(CharacterTextures[PlayerCharacter->GetPlayerSelectedModel()]));
						}
					}
					
				}
			}
		}
	}
	// Populating the voting list dynamically
	
}

void UVoteWidget::UpdateVoteText()
{
	// TODO: Lin  Ryan 02/09/2025
	// The actual text is here is pending changes.
	if (APotProbPlayerState* OwningPlayerState = Cast<APotProbPlayerState>(GetOwningPlayerState()))
	{
		if (const APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
		{
			bool bIsInDiscussionPhase = GameState->VoteTimer > (GameState->VotePhase_Duration - GameState->DiscussionPhase_Duration);
			if (OwningPlayerState->bIsFrogged)
			{
				FString FroggedText = "You won't be able to cast a vote because you are frogged.";
				PlayerSelectedText->SetText(FText::FromString(FroggedText));
			}
			if (GameState->bIsTieBreaker)
			{
				FString VoteTimerText = "Tie-breaker voting ends in: " + FString::FromInt(GameState->VoteTimer) + " seconds";
				VoteTimer->SetText(FText::FromString(VoteTimerText));
			}
			else if (bIsInDiscussionPhase)
			{
				FString VoteTimerText = "Discussion phase, you will be able to cast a vote in " +
					FString::FromInt(GameState->VoteTimer - GameState->VotePhase_Duration + GameState->DiscussionPhase_Duration) + " seconds";
				VoteTimer->SetText(FText::FromString(VoteTimerText));
			} else
			{
				FString VoteTimerText = "Voting ends in: " + FString::FromInt(GameState->VoteTimer) + " seconds";
				VoteTimer->SetText(FText::FromString(VoteTimerText));
			}
		}
	}
}

void UVoteWidget::UpdatePlayerSelectedText(const FString& PlayerName)
{
	PlayerSelectedText->SetText(FText::FromString("Player Selected: " + PlayerName));
}

void UVoteWidget::NativeConstruct()
{
	Super::NativeConstruct();

	VoteChatInput->OnTextCommitted.AddDynamic(this, &UVoteWidget::OnTextCommitted);
	
	SetIsFocusable(true);
	
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
}

void UVoteWidget::NativeDestruct()
{
	Super::NativeDestruct();

	VoteChatInput->OnTextCommitted.RemoveDynamic(this, &UVoteWidget::OnTextCommitted);
}

FReply UVoteWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Enter)
	{
		if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetOwningPlayer()))
		{
			VoteChatInput->SetUserFocus(PC);
			return FReply::Handled();
		}
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

TArray<APotProbPlayerState*> UVoteWidget::ConvertPlayerArray(const TArray<APlayerState*>& PlayerArray)
{
	TArray<APotProbPlayerState*> Result;
	for (APlayerState* Player : PlayerArray)
	{
		if (APotProbPlayerState* PotProbPlayer = Cast<APotProbPlayerState>(Player))
		{
			Result.Add(PotProbPlayer);
		}
	}
	return Result;
}

void UVoteWidget::SetPlayerSelected(const FString& PlayerName)
{
	LockVoteButton->SetIsEnabled(true);
	PlayerSelected = PlayerName;
	PlayerSelectedText->SetText(FText::FromString("Player Selected: " + PlayerName));
}

void UVoteWidget::AddChatMessage(const FString& MessageRecieverName, const FString& MessageSenderName, EAnimModels Model, const FString& MessageContent)
{
	TObjectPtr<UVoteMessageWidget> NewChatMessage;
	if (MessageRecieverName == MessageSenderName)
	{
		NewChatMessage = NewObject<UVoteMessageWidget>(this, SendMessageWidgetClass);
	}
	else
	{
		NewChatMessage = NewObject<UVoteMessageWidget>(this, RecieveMessageWidgetClass);
	}
	
	if (!NewChatMessage)
	{
		return;
	}

	ChatMessages->AddChildToVerticalBox(NewChatMessage);
	NewChatMessage->SetPlayerName(MessageSenderName);
	NewChatMessage->SetMessageText(MessageContent);
	NewChatMessage->SetPlayerImage(Cast<UTexture2D>(CharacterTextures[Model]));

	APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error,
			   TEXT("Warning: Player Controller Doesn't exist for this HUDWidget. Something has gone terribly wrong."));
		return;
	}

	//NewChatMessage->Sender->SetColorAndOpacity(AllChatColor);
	//NewChatMessage->Message->SetColorAndOpacity(AllChatColor);

	ChatScrollBox->ScrollToEnd();
}

void UVoteWidget::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	APotProbPlayerController* MultiController = Cast<APotProbPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APotProbPlayerState* PlayerState = MultiController->GetPlayerState<APotProbPlayerState>();

		if (!PlayerState)
		{
			UE_LOG(LogTemp, Error,
			       TEXT("Warning: PlayerState for PotionPlayerController is not APotProbPlayerState."
			       ));
			return;
		}
		if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(MultiController->GetPawn()))
		{
			VoteChatInput->SetText(FText::GetEmpty());
			FString Input = Text.ToString().TrimStartAndEnd();
			if (Input.IsEmpty())
			{
				return;
			}
			FString CensoredInput = UChatFilterLibrary::CensorMessage(Text.ToString());
			PlayerState->ServerReceiveChatMessage(PlayerState->GetPlayerName(), PlayerCharacter->GetPlayerSelectedModel(), CensoredInput);
		}
		/* Keep focus on vote chat input */
		VoteChatInput->SetUserFocus(MultiController);
	}

}

void UVoteWidget::LockInVote()
{
	if (GetOwningPlayer()->WasInputKeyJustReleased(EKeys::Enter))
	{
		return;
	}
	if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetOwningPlayer()))
	{
		if (PlayerSelected != "")
		{
			PC->Server_ProcessVote(PlayerSelected, GetOwningPlayerState()->GetPlayerName());
			LockVoteButton->SetIsEnabled(false);
			SkipButton->SetIsEnabled(false);
			PlayerSelectedText->SetText(FText::FromString("Player Voted: " + PlayerSelected));
			
			TroublemakerText->SetVisibility(ESlateVisibility::Collapsed);
			VoteInsturctions->SetVisibility(ESlateVisibility::Collapsed);
			VotedText->SetVisibility(ESlateVisibility::Visible);
			
			return;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Lock in vote failed."));
}

void UVoteWidget::SkipVote()
{
	if (GetOwningPlayer()->WasInputKeyJustReleased(EKeys::Enter))
	{
		return;
	}
	if (APotProbPlayerState* OwningPlayerState = Cast<APotProbPlayerState>(GetOwningPlayerState()))
	{
		OwningPlayerState->SkipVote();
		PlayerSelected = "";
		PlayerSelectedText->SetText(FText::FromString("You Skipped Voting"));

		if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(GetOwningPlayer()))
		{
			PC->Server_ProcessVote("", GetOwningPlayerState()->GetPlayerName());
		}
		
		LockVoteButton->SetIsEnabled(false);
		SkipButton->SetIsEnabled(false);
		
		TroublemakerText->SetVisibility(ESlateVisibility::Collapsed);
		VoteInsturctions->SetVisibility(ESlateVisibility::Collapsed);
		VotedText->SetVisibility(ESlateVisibility::Visible);
		
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Skip vote failed."));
}

