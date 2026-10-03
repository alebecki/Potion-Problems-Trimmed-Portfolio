// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "FoundSessionWidget.generated.h"

class UCircularThrobber;
class UButton;
class UTextBlock;
/**
 *
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UFoundSessionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(Meta = (BindWidget))
	UButton* JoinButton;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* SessionName;

	UPROPERTY(Meta = (BindWidget))
	UCircularThrobber* SessionLoadingThrobber;

	UMainMenuWidget* MainMenu;

	/*UPROPERTY(Meta = (BindWidget))
	UTextBlock* PingAmount;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* MapName;*/

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PlayerCount;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void SetSessionIndex(int Index) { SessionIndex = Index; }
	void SetIsFriendSession(bool Value) { bIsFriendSession = Value; }
protected:
	int SessionIndex;
	bool bIsFriendSession = false;
	UFUNCTION(BlueprintCallable)
	void OnClickedAction();
};
