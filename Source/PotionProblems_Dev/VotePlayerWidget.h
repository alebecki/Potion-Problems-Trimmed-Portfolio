// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VotePlayerWidget.generated.h"

class UWrapBox;
class UHorizontalBox;
class UImage;
class UCheckBox;
class UButton;
class UTextBlock;
class UVoteWidget;
class UToggleButton;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UVotePlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Meta = (BindWidget))
	UToggleButton* VoteButton;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PlayerName;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* VoteCount;
	
	UPROPERTY(Meta = (BindWidget))
	UCheckBox* CheckBoxVoting;
	
	UPROPERTY(Meta = (BindWidget))
	UImage* PlayerHead;
	
	UPROPERTY(Meta = (BindWidget))
	UWrapBox* VoteDots;

	TObjectPtr<UVoteWidget> VoteWidget;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void SetVoteCounts(int32 Votes);

protected:
	UFUNCTION(BlueprintCallable)
	void OnClickedAction();
};
