// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomAnimWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "DisguiseHUDWidget.generated.h"

class UButton;
class UDisguiseUIButton;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UDisguiseHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* PlayerSelectionBox;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDisguiseUIButton> DisguiseButtonClass;
private:
	UFUNCTION()
	void CreatePlayerSelectionBox(ESlateVisibility VisibilityStatus);
};
