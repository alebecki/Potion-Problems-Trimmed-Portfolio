// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProxyChatWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEditableTextBoxCommittedEvent, const FText&, Text, ETextCommit::Type, CommitMethod);

/**
 * 
 */
UCLASS(Blueprintable)
class POTIONPROBLEMS_DEV_API UProxyChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	void AddChatMessage(const FString& Message);

	UFUNCTION()
	void HandleSendMessage(const FText& Text, ETextCommit::Type CommitMethod);
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UVerticalBox> ProxyChatBox;
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UVerticalBox> ProxyMessages;
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> ProxyTextBox;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UProxyMessageWidget> ProxyMessageWidgetClass;
	UPROPERTY(Transient)
	TObjectPtr<class UProxyMessageWidget> ProxyMessageWidgetInstance;
};
