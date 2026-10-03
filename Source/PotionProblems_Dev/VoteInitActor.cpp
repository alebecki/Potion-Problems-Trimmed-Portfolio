// Fill out your copyright notice in the Description page of Project Settings.


#include "VoteInitActor.h"

#include "PotProbPlayerController.h"
#include "PotProbPlayerState.h"
#include "PotProbZDCharacter.h"


AVoteInitActor::AVoteInitActor()
{
	PrimaryActorTick.bCanEverTick = false;
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);

	VoteInitSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("VoteInitSprite"));
	VoteInitSprite->SetupAttachment(RootComponent);
	InteractComponent = CreateDefaultSubobject<UInteractComponent>(TEXT("InteractComponent"));
	bReplicates = true;
}

void AVoteInitActor::InitiateVotingPhase(APawn* InstigatingPawn)
{
	if (APotProbZDCharacter* Player = Cast<APotProbZDCharacter>(InstigatingPawn))
	{
		
		if (APotProbPlayerController* PC = Cast<APotProbPlayerController>(Player->GetController()))
		{
			if (APotProbPlayerState* PS = PC->GetPlayerState<APotProbPlayerState>())
			{
				if (PS->bCanInitVote && !PS->bHasInitVote)
				{
					PS->bHasInitVote = true;
					if (APotProbGameState* GameState = Cast<APotProbGameState>(GetWorld()->GetGameState()))
					{
						for (APlayerState* CurrPS : GameState->PlayerArray)
						{
							if (APotProbPlayerController* CurrPC = Cast<APotProbPlayerController>(CurrPS->GetOwner()))
							{
								CurrPC->Server_StartVoteCountdown();
							}
						}
					}
				}
			}
		}
	}
}
