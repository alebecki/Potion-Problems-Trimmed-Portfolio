// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FriendButtonWidget.generated.h"

class UButton;
class UTextBlock;
/**
 *
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UFriendButtonWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(Meta = (BindWidget))
	UButton* JoinButton;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* FriendName;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void SetFriendIndex(int Index) { FriendIndex = Index; }
protected:
	int FriendIndex;
	UFUNCTION(BlueprintCallable)
	void OnClickedAction();
};
