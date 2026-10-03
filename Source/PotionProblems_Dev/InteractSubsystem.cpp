// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractSubsystem.h"
#include "InteractComponent.h"
#include "MinigameInitActor.h"
#include "MinigameInitUIActor.h"
#include "IngredientActor.h"
#include "PotProbPlayerController.h"
#include "PotProbPlayerState.h"
#include "PotProbZDCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UInteractSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UInteractSubsystem::Deinitialize()
{
	Super::Deinitialize();
	InteractComponents.Empty();
}

void UInteractSubsystem::AddComponent(UInteractComponent* NewComponent)
{
	if (!InteractComponents.Contains(NewComponent))
	{
		InteractComponents.Add(NewComponent);
	}
}

void UInteractSubsystem::RemoveComponent(UInteractComponent* TargetComponent)
{
	InteractComponents.Remove(TargetComponent);
}

void UInteractSubsystem::PerformInteract(APawn* InstigatorPawn, UInteractComponent* InteractTarget)
{
	if (!InstigatorPawn || !InteractTarget)
	{
		return;
	}

	APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(InstigatorPawn);
	if(!Character)
	{
		return;
	}

	// If the Char is camou'd and it's interacting with a cauldron or a minigame return;
	if(Character->GetIsCamouflaged() && (Cast<ACauldronActor>(InteractTarget->GetOwner()) || Cast<AMinigameInitUIActor>(InteractTarget->GetOwner()) || Cast<AIngredientActor>(InteractTarget->GetOwner()) || Cast<AMinigameInitActor>(InteractTarget->GetOwner())))
	{
		// alert player on interact fail
		APotProbPlayerState* PS = Cast<APotProbPlayerState>(Character->GetPlayerState());
		if (PS)
		{
			PS->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "Inconspicuous objects can't interact...");
		}
		
		return;
	}
	InteractTarget->NativeInteract(InstigatorPawn);
}

void UInteractSubsystem::PerformInteractHeld(APawn* InstigatorPawn, UInteractComponent* InteractTarget)
{
	if (!InstigatorPawn || !InteractTarget)
	{
		return;
	}

	APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(InstigatorPawn);
	if (!Character)
	{
		return;
	}

	// If the Char is camou'd and it's interacting with a cauldron or a minigame return;
	if (Character->GetIsCamouflaged() && (Cast<ACauldronActor>(InteractTarget->GetOwner()) || Cast<AMinigameInitUIActor>(InteractTarget->GetOwner()) || Cast<AIngredientActor>(InteractTarget->GetOwner()) || Cast<AMinigameInitActor>(InteractTarget->GetOwner())))
	{
		// alert player on interact fail
		APotProbPlayerState* PS = Cast<APotProbPlayerState>(Character->GetPlayerState());
		if (PS)
		{
			PS->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "Inconspicuous objects can't interact...");
		}
		
		return;
	}
	InteractTarget->NativeInteractHeld(InstigatorPawn);
}

void UInteractSubsystem::PerformInteractSuperHeld(APawn* InstigatorPawn, UInteractComponent* InteractTarget)
{
	if (!InstigatorPawn || !InteractTarget)
	{
		return;
	}

	APotProbZDCharacter* Character = Cast<APotProbZDCharacter>(InstigatorPawn);
	if (!Character)
	{
		return;
	}

	// If the Char is camou'd and it's interacting with a cauldron or a minigame return;
	if (Character->GetIsCamouflaged() && (Cast<ACauldronActor>(InteractTarget->GetOwner()) || Cast<AMinigameInitUIActor>(InteractTarget->GetOwner()) || Cast<AIngredientActor>(InteractTarget->GetOwner()) || Cast<AMinigameInitActor>(InteractTarget->GetOwner())))
	{
		// alert player on interact fail
		APotProbPlayerState* PS = Cast<APotProbPlayerState>(Character->GetPlayerState());
		if (PS)
		{
			PS->ClientReceiveAlertMessage(EPotProbAlertTypes::LOW_PRIORITY, "Inconspicuous objects can't interact...");
		}
		
		return;
	}
	InteractTarget->NativeInteractSuperHeld(InstigatorPawn);
}

void UInteractSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	APotProbZDCharacter* PlayerChar = Cast<APotProbZDCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if(!PlayerChar)
	{
		return;
	}
	APotProbPlayerController* PlayerController = Cast<APotProbPlayerController>(PlayerChar->GetController());
	if(!PlayerController)
	{
		return;
	}
	APotProbPlayerState* PotPlayerState = Cast<APotProbPlayerState>(PlayerChar->GetPlayerState());
	if(!PotPlayerState)
	{
		return;
	}
	
	UInteractComponent* PlayerInteract = PlayerChar->GetComponentByClass<UInteractComponent>();
	if(!PlayerInteract)
	{
		UE_LOG(LogTemp, Error, TEXT("Player Character that is using the interact subsystem somehow doesn't have an interact component. This is really bad."));
		return;
	}
	
	FVector PlayerPos = PlayerChar->GetActorLocation();
	float InteractionDistance = PlayerController->GetInteractionDistance();
	TWeakObjectPtr<UInteractComponent> OldBestCandidate = BestCandidate;
	
	BestCandidate= nullptr;
	float BestDistance = TNumericLimits<float>::Max();
	APotProbZDCharacter* CandidatePlayer = nullptr;
	
	for (const auto& WeakPtr : InteractComponents)
	{
		if (UInteractComponent* Component = WeakPtr.Get())
		{
			if(Component == PlayerInteract)
			{
				continue;
			}

			// Use squared distance (since we're just comparing relative distances). Saves us several
			// square root calculations each frame.
			float DistSqr = FVector::DistSquared(PlayerPos, Component->GetOwner()->GetActorLocation());
			if (DistSqr < (InteractionDistance * InteractionDistance) && DistSqr < BestDistance)
			{
				APotProbZDCharacter* OtherPlayer = Cast<APotProbZDCharacter>((Component->GetOwner()));
				
				// This candidate is another player. Check if we can interact with them.
				if (OtherPlayer)
				{
					bool bCanInteractWith = false;
					APotProbPlayerState* OtherState = OtherPlayer->GetPlayerState<APotProbPlayerState>();

					if (PlayerChar->GetIsWerefrog())
					{
						// We can interact with this player if either:
						// A) They aren't frogged yet
						// B) Our other candidate isn't a player
						// C) Our other candidate is a player who is already frogged.
						if(
							!OtherState->bIsFrogged ||
							(BestCandidate.Get() && !CandidatePlayer) ||
							(CandidatePlayer && CandidatePlayer->GetPlayerState<APotProbPlayerState>()->bIsFrogged)
						)
						{
							bCanInteractWith = true;
						}
					}
					else if (PlayerChar->GetPlayerState<APotProbPlayerState>()->GetHasCooties())
					{
						// Cooties
						bCanInteractWith = true;
					}
					else if (OtherState->bIsFrogged && OtherState->bCanFroggedBeDiscovered)
					{
						// Player is frogged and discoverable
						// which means we can interact with them (to report)
						bCanInteractWith = true;
					}

					if (!bCanInteractWith)
						continue;
				}
				
				CandidatePlayer = OtherPlayer;
				BestDistance = DistSqr;
				BestCandidate = Component;
			}
		}
	}
	
	// clear old interaction target
	if(OldBestCandidate.IsValid() && OldBestCandidate != BestCandidate)
	{
		OldBestCandidate.Get()->DisplayInteractText(false);
	}

	if(BestCandidate.IsValid())
	{
		// display interaction text
		BestCandidate.Get()->DisplayInteractText(true);
	}
}

TStatId UInteractSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UInteractSubsystem, STATGROUP_Tickables);
}

bool UInteractSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}
