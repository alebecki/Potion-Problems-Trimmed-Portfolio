// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include "Components/Button.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MainMenuButton->OnClicked.AddDynamic(this, &ULobbyWidget::ToMainMenuButton);
	PublicJoinButton->OnClicked.AddDynamic(this, &ULobbyWidget::SeePublicLobbyList);
	PublicJoinButton->OnClicked.AddDynamic(this, &ULobbyWidget::CreatePublicLobby);
	PublicJoinButton->OnClicked.AddDynamic(this, &ULobbyWidget::JoinPrivateLobby);
	PublicJoinButton->OnClicked.AddDynamic(this, &ULobbyWidget::CreatePrivateLobby);
}

void ULobbyWidget::NativeDestruct()
{
	Super::NativeDestruct();
	MainMenuButton->OnClicked.RemoveDynamic(this, &ULobbyWidget::ToMainMenuButton);
	PublicJoinButton->OnClicked.RemoveDynamic(this, &ULobbyWidget::SeePublicLobbyList);
	PublicJoinButton->OnClicked.RemoveDynamic(this, &ULobbyWidget::CreatePublicLobby);
	PublicJoinButton->OnClicked.RemoveDynamic(this, &ULobbyWidget::JoinPrivateLobby);
	PublicJoinButton->OnClicked.RemoveDynamic(this, &ULobbyWidget::CreatePrivateLobby);
}

void ULobbyWidget::ToMainMenuButton()
{
	// TODO: Takes the player back to the main menu
}

void ULobbyWidget::SeePublicLobbyList()
{
	// TODO: Opens the "AllPublicLobbies" canvas panel
}

void ULobbyWidget::CreatePublicLobby()
{
	// TODO: Opens the "PublicLobbyCreation" canvas panel
}

void ULobbyWidget::JoinPrivateLobby()
{
	// TODO: Opens the "JoinPrivateLobby" canvas panel

}

void ULobbyWidget::CreatePrivateLobby()
{
	// TODO: Open the "CreatePrivateLobby" canvas panel
}