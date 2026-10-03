// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	//Back to Main Menu
	UPROPERTY(Meta = (BindWidget))
	UButton* MainMenuButton;

	// Public vs Private
	UPROPERTY(Meta = (BindWidget))
	UButton* PublicJoinButton;

	UPROPERTY(Meta = (BindWidget))
	UButton* PublicCreateButton;

	UPROPERTY(Meta = (BindWidget))
	UButton* PrivateJoinButton;
	
	UPROPERTY(Meta = (BindWidget))
	UButton* PrivateCreateButton;

	UPROPERTY(Meta = (BindWidget))
	UButton* JoinActualPublicLobbyButton;

protected:
	UFUNCTION()
	void ToMainMenuButton();

	UFUNCTION()
	void SeePublicLobbyList();

	UFUNCTION()
	void CreatePublicLobby();

	UFUNCTION()
	void JoinPrivateLobby();

	UFUNCTION()
	void CreatePrivateLobby();
};
