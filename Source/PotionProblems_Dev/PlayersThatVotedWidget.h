// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayersThatVotedWidget.generated.h"

class UWrapBox;
class UImage;
class UTextBlock;
class UHorizontalBox;
class UWhoVotedWidget;

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UPlayersThatVotedWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PlayerName;

	UPROPERTY(Meta = (BindWidget))
	UImage* PlayerHead;


	UPROPERTY(Meta = (BindWidget))
	UWrapBox* HeadHolder;

	TObjectPtr<UWhoVotedWidget> VoteWidget;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
