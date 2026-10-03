// Fill out your copyright notice in the Description page of Project Settings.


#include "VoteMessageWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UVoteMessageWidget::SetPlayerImage(class UTexture2D* NewPlayerImage)
{
	if (PlayerImage)
	{
		PlayerImage->SetBrushFromTexture(NewPlayerImage);
	}
}

void UVoteMessageWidget::SetMessageText(const FString& NewText)
{
	if (Message)
	{
		Message->SetText(FText::FromString(NewText));
	}
}

void UVoteMessageWidget::SetPlayerName(const FString& NewName)
{
	if (PlayerName)
	{
		PlayerName->SetText(FText::FromString(NewName));
	}
}
