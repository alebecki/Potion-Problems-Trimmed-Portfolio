// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageWidget.generated.h"

class UHorizontalBox;
class UTextBlock;

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Channel;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Sender;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Message;
};
