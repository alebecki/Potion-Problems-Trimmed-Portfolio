// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TroublemakersVotedWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UTroublemakersVotedWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(Meta = (BindWidget))
	UImage* TroublemakerSpriteImage;
	UPROPERTY(Meta = (BindWidget))
	UImage* TMCheckImage;
};
