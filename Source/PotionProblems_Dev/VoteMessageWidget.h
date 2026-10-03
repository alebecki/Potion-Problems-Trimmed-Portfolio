// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VoteMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UVoteMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPlayerImage(class UTexture2D* NewPlayerImage);
	void SetMessageText(const FString& NewText);
	void SetPlayerName(const FString& NewName);
	
protected:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UImage> PlayerImage;
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerName;
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UTextBlock> Message;
};
