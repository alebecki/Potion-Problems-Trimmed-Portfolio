// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyStatusWidget.generated.h"

class UTextBlock;
class UWidgetSwitcher;

/**
 * 
 */
UCLASS(Blueprintable)
class POTIONPROBLEMS_DEV_API ULobbyStatusWidget : public UUserWidget
{
	GENERATED_BODY()

	void NativeDestruct() override;

public:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> Username;
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> ToggleSwitcher;

	void SetName(const FString& InName);
	void SetActive(bool InActive);
};
