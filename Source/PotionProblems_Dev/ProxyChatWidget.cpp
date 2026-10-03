// Fill out your copyright notice in the Description page of Project Settings.
#include "ProxyChatWidget.h"

#include "ChatFilterLibrary.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/EditableTextBox.h"

#include "PotProbPlayerState.h"
#include "PotProbZDCharacter.h"
#include "ProxyMessageWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

void UProxyChatWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ProxyTextBox->OnTextCommitted.AddDynamic(this, &UProxyChatWidget::HandleSendMessage);
}

void UProxyChatWidget::NativeDestruct()
{
	Super::NativeDestruct();

	ProxyTextBox->OnTextCommitted.RemoveDynamic(this, &UProxyChatWidget::HandleSendMessage);
}

void UProxyChatWidget::AddChatMessage(const FString& Message)
{
	// Delete messages if exceeds x number of messages
	if (ProxyMessages->GetChildrenCount() >= 3)
	{
		// Remove from vertical box
		ProxyMessages->RemoveChildAt(0);
		
		/*
		 * It could be entirely possible that there's some weird edge case that when we press enter at a specific time,
		 * the messages array is empty. This might require a small refactor of the messages. Feel free to contact me
		 * (andy) about this.
		 */
	}
	
	TObjectPtr<class UProxyMessageWidget> ProxyMessageInstance = CreateWidget<UProxyMessageWidget>(this, ProxyMessageWidgetClass);
	ProxyMessageInstance->SetMessageText(FText::FromString(Message));
	ProxyMessages->AddChildToVerticalBox(ProxyMessageInstance);
}

void UProxyChatWidget::HandleSendMessage(const FText& Text, ETextCommit::Type CommitMethod)
{
	//FText Input = Text;
	
	if (APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(GetOwningPlayer()->GetPawn()))
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			FString Input = Text.ToString().TrimStartAndEnd();
			if (!Input.IsEmpty())
			{
				Input = UChatFilterLibrary::CensorMessage(Input);
				Character->ServerProxyMessage(Input);
			}
			//EMultiTeam ChatChannel = ChatTeam->GetText().CompareToCaseIgnored(FText::FromString("[Team]")) ? EMultiTeam::None : PlayerState->Team;
			//PlayerState->ServerReceiveProxyChatMessage(Input.ToString());
		}

		if (Character->IsLocallyControlled())
		{
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(),0));
		}
	}
	if (ProxyTextBox)
	{
		ProxyTextBox->SetText(FText::FromString(""));
	}


	/* Set the proxy text box to remain focused after committing a message*/
	//ProxyTextBox->SetKeyboardFocus();
}
