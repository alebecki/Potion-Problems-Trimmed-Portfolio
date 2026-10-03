// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomAnimWidget.h"
#include "PlayerRoleWidget.generated.h"

class UScaleBox;
class UFellowTBMKRWidget;
class UVerticalBox;
class UTextBlock;
class UButton;
class UImage;

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UPlayerRoleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (BindWidget))
	UCustomAnimWidget* CloseButtonAnim;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PlayerRole;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ActionOne;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ActionTwo;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ActionThree;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TroublemakerCount;

	UPROPERTY(Meta = (BindWidget))
	UButton* CloseButton;

	UPROPERTY(Meta = (BindWidget))
	UImage* Apprentice;

	UPROPERTY(Meta = (BindWidget))
	UImage* Troublemaker;

	UPROPERTY(Meta = (BindWidget))
	UImage* Frog;

	UPROPERTY(Meta = (BindWidget))
	UScaleBox* SBFellowTBMKRImage;

	UPROPERTY(Meta = (BindWidget))
	UScaleBox* SBFellowTBMKRList;

	UPROPERTY(Meta = (BindWidget))
	UVerticalBox* FellowTBMKRList;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFellowTBMKRWidget> FellowTBMKRWidgetClass;
	
	bool bAlertOnClose = false;
	
	void SetPlayerInformation(const FString& PlayerRoleText, const FString& ActionOneText, const FString& ActionTwoText,
	                          const FString& ActionThreeText, const FString& TroublemakerCountText);

private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void OnClose();
	void BuildFellowTBMKRList();
};
