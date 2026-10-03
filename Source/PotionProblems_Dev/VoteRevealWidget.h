// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VoteRevealWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UVoteRevealWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    
    virtual void NativeConstruct() override;

    UPROPERTY(Meta = (BindWidget))
    UTextBlock* VoteRevealText;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float VoteRevealDuration = 3.0f;
    
    void UpdateRevealText(bool playerFrogged, FString playerName = FString(TEXT("")));

private:

    FTimerHandle DestructionTimerHandle;

    void DestroySelf();
};
