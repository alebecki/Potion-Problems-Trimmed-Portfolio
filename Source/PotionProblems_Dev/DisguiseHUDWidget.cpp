// Fill out your copyright notice in the Description page of Project Settings.


#include "DisguiseHUDWidget.h"
#include "DisguiseUIButton.h"
#include "PotProbGameState.h"
#include "PotProbPlayerState.h"
#include "PotProbZDCharacter.h"
#include "Animation/AnimNode_StateMachine.h"
#include "Components/VerticalBox.h"
#include "GameFramework/PlayerState.h"

void UDisguiseHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OnVisibilityChanged.AddDynamic(this, &ThisClass::CreatePlayerSelectionBox);
}

void UDisguiseHUDWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UDisguiseHUDWidget::CreatePlayerSelectionBox(ESlateVisibility VisibilityStatus)
{
	APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState());

	if(!GameState)
	{
		return;
	}
	PlayerSelectionBox->ClearChildren();
	int i = 0;
	for(const auto& Iter : GameState->PlayerArray)
	{
		if(!Iter)
		{
			continue;
		}
		APotProbZDCharacter* PotProbZdCharacter = Cast<APotProbZDCharacter>(Iter.Get()->GetPawn());
		if(!PotProbZdCharacter)
		{
			continue;
		}
		APotProbPlayerState* PlayerState = Cast<APotProbPlayerState>(Iter.Get());
		if(!PlayerState)
		{
			continue;
		}
		/*if (PotProbZdCharacter->GetIsWerefrog() || PlayerState->bIsFrogged || Iter.Get() == UGameplayStatics::GetPlayerState(GetWorld(), 0))
        {
            continue;
        }*/
		
		TObjectPtr<UDisguiseUIButton> NewButton = NewObject<UDisguiseUIButton>(this, DisguiseButtonClass);
		PlayerSelectionBox->AddChildToUniformGrid(NewButton, i / 5, i % 5);
		NewButton->SetPlayerName(Iter.Get()->GetPlayerName());
		NewButton->SetAnimModel(PotProbZdCharacter->GetPlayerSelectedModel());
		NewButton->SetParentHUDWidget(this);
		if (PotProbZdCharacter->GetIsWerefrog() || PlayerState->bIsFrogged || PotProbZdCharacter->IsLocallyControlled())
		{
			NewButton->SetIsEnabled(false);
		}
		++i;
	}
}
