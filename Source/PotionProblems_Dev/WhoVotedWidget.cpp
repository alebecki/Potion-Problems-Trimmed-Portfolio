// Fill out your copyright notice in the Description page of Project Settings.


#include "WhoVotedWidget.h"

#include "CharacterData.h"
#include "PlayersThatVotedWidget.h"
#include "PotProbPlayerState.h"
#include "PotProbGameState.h"
#include "PotProbZDCharacter.h"
#include "PotProbPlayerController.h"
#include "VoteRevealWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Components/Image.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/ScaleBox.h"
#include "Components/ScaleBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/WrapBoxSlot.h"

void UWhoVotedWidget::BuildVotingList()
{
	VotePlayers->ClearChildren();

	if (const APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
	{
		for (const auto it : GameState->PlayerArray)
		{
			if (APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(it.Get()))
			{
				TObjectPtr<UPlayersThatVotedWidget> NewVotePlayerWidget = NewObject<UPlayersThatVotedWidget>(this, VotePlayerClass);
				NewVotePlayerWidget->VoteWidget = this;
				VotePlayers->AddChildToWrapBox(NewVotePlayerWidget);
				FString PlayerName = PlayerState->GetPlayerName();
				NewVotePlayerWidget->PlayerName->SetText(FText::FromString(PlayerName));
				
				if (APotProbZDCharacter* PlayerCharacter = Cast<APotProbZDCharacter>(PlayerState->GetPawn()))
				{
					NewVotePlayerWidget->PlayerHead->SetBrushFromTexture(Cast<UTexture2D>(CharacterTextures[PlayerCharacter->GetPlayerSelectedModel()]));
				}
				//NewVotePlayerWidget->SetVoteCounts(PlayerState->VoteCount);
				//add heads of all voters similar to the number of players that voted in vote 
				TArray<APotProbPlayerState*> VotingPlayer;
				for (FVotesCast Votes : WhoVotedForWhoMap) {
					if (Votes.Player == PlayerState) {
						VotingPlayer = Votes.Votes;
					}
				}
				//TArray<APotProbPlayerState*> VotingPlayer = WhoVotedForWhoMap.Find(PlayerState)->Votes;
				for (APotProbPlayerState* Voter : VotingPlayer) {
					Voter->PlayersVotingHistory.Add(PlayerState);
					UImage* NewHead = NewObject<UImage>(NewVotePlayerWidget);
					NewHead->SetBrushFromTexture(Cast<UTexture2D>(CharacterTextures[Cast<APotProbZDCharacter>(Voter->GetPawn())->GetPlayerSelectedModel()]));
					NewHead->SetDesiredSizeOverride(FVector2D(56.0f, 56.0f));
					NewHead->SetDesiredSizeOverride(FVector2D(56.0f, 56.0f));
					
					USizeBox* SizeBox = NewObject<USizeBox>(NewVotePlayerWidget);
					// Add the image to the SizeBox
					SizeBox->AddChild(NewHead);
					SizeBox->SetWidthOverride(56.0f);
					SizeBox->SetHeightOverride(56.0f);

					// Add the SizeBox to the WrapBox and configure the slot
					UWrapBoxSlot* WrapSlot = NewVotePlayerWidget->HeadHolder->AddChildToWrapBox(SizeBox);
					
				}
			}
		}
	}
	
}

void UWhoVotedWidget::UpdateVoteText()
{
	/*if (const APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
	{
		bool bIsInDiscussionPhase = GameState->VoteTimer > (GameState->VotePhase_Duration - GameState->DiscussionPhase_Duration);
		if (bIsInDiscussionPhase)
		{
			FString VoteTimerText = "Moving on in " +
				FString::FromInt(GameState->VoteTimer - GameState->VotePhase_Duration + GameState->DiscussionPhase_Duration) + " seconds";
			VoteTimer->SetText(FText::FromString(VoteTimerText));
		}
		else
		{
			FString VoteTimerText = "Voting ends in: " + FString::FromInt(GameState->VoteTimer) + " seconds";
			VoteTimer->SetText(FText::FromString(VoteTimerText));
		}
	}*/
}

void UWhoVotedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	bHasScriptImplementedTick = true;
	Timer = VoteRevealDuration;
	TArray<FCharacterData*> Rows;
	static const FString ContextString(TEXT("Character Data Context"));
	CharacterDataTable->GetAllRows<FCharacterData>(ContextString, Rows);

	for (const FCharacterData* Row : Rows)
	{
		if (Row)
		{
			CharacterTextures.Add(Row->CharacterModel, Row->HeadOnlyTexture);
		}
	}
}

void UWhoVotedWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Timer -= InDeltaTime;
	FString VoteTimerText = "Moving on in " +
		FString::FromInt(Timer) + " seconds";
	VoteTimer->SetText(FText::FromString(VoteTimerText));
	if (Timer <= 0.0f)
	{
		Timer = FLT_MAX;
		DestroySelf();
	}
}

void UWhoVotedWidget::DestroySelf()
{
	APlayerController* PC = GetOwningPlayer();
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC);
	
	UVoteRevealWidget* Widget = CreateWidget<UVoteRevealWidget>(Cast<APotProbPlayerController>(GetOwningPlayer()), VoteRevealWidgetClass);
	if (Widget)
	{
		if (VotedOutPlayerName != "")
		{
			Widget->UpdateRevealText(true, VotedOutPlayerName);
		}
		else
		{
			Widget->UpdateRevealText(false);
		}
		Widget->AddToViewport(100);
	}
	RemoveFromParent();
}
