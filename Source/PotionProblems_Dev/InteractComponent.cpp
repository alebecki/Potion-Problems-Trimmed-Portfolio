// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractComponent.h"

#include "CauldronActor.h"
#include "FrogDoorActor.h"
#include "InteractSubsystem.h"
#include "InteractWidget.h"
#include "MinigameInitActor.h"
#include "MinigameInitUIActor.h"
#include "Mirror.h"
#include "Components/WidgetComponent.h"
#include "NiagaraScript.h"
#include "Pedestal.h"
#include "PotionActor.h"
#include "PotProbPlayerState.h"
#include "PotProbZDCharacter.h"
#include "VoteInitActor.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Sight.h"
// Sets default values for this component's properties
UInteractComponent::UInteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


void UInteractComponent::DisplayInteractText(bool bShouldDisplay)
{
	if(!IsValid(InteractableWidget))
	{
		TArray<UActorComponent*> Components = GetOwner()->GetComponentsByTag(UWidgetComponent::StaticClass(), "Interact");

		// There should only be one interact WidgetComponent
		if(Components.Num() != 1)
		{
			return;
		}

		UWidgetComponent* WidgetComp = Cast<UWidgetComponent>(Components[0]);
		if(!WidgetComp)
		{
			return;
		}

		InteractableWidget = Cast<UInteractWidget>(WidgetComp->GetWidget());
	}
	
	FString DisplayText = DetermineTextToDisplay();

	bIsDisplayingInteractText = bShouldDisplay;
	
	if (!InteractableWidget.Get()) {
		return;
	}
	InteractableWidget.Get()->UpdateText(DisplayText);
	if(bShouldDisplay)
	{
		InteractableWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
	    bInitBottlePotionAlert = false;
	    bFrogDoorAlert = false;
		InteractableWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

FString UInteractComponent::DetermineTextToDisplay()
{
	APotProbZDCharacter* ClientCharacter = Cast<APotProbZDCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	APotProbPlayerState* ClientPS = ClientCharacter->GetPlayerState<APotProbPlayerState>();
	if(!ClientPS || !ClientCharacter)
	{
		return FString();
	}
	
	if(APotProbZDCharacter* Char = Cast<APotProbZDCharacter>(GetOwner()))
	{
	    bInitBottlePotionAlert = false;
	    bFrogDoorAlert = false;
		if(Char->GetPlayerState<APotProbPlayerState>()->bIsFrogged)
		{
			return TEXT("[E] Discover Frog!");
		}
		else if (ClientPS->GetHasCooties())
		{
			return TEXT("[E] Give This Player Cooties!");
		}
		else if(ClientPS->GetHasPotionBeenActivated())
		{
			return ClientCharacter->GetIsWerefrog() ? TEXT("[E] Frog This Player!") : TEXT("[E] Use Potion On This Player");
		}
	}
	else if(ACauldronActor* CauldronActor = Cast<ACauldronActor>(GetOwner()))
	{
	    bFrogDoorAlert = false;
		if (CauldronActor->GetNumberIngredientsInCauldron() >= 3 && ClientPS->PotProbRole != EPotProbRoles::ROLE_TROUBLEMAKER) 
		{
		    bInitBottlePotionAlert = false;
			return TEXT("Too many ingredients!");
		}		
		else if (CauldronActor->CanMakeFrogPotion(ClientPS))
		{
		    if (!bInitBottlePotionAlert)
		    {
		        bInitBottlePotionAlert = true;
		        ClientPS->ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "You can bottle\nThe Froggart Curse!");
		    }
		    return TEXT("[F] Bottle Potion");
		}
		else if (CauldronActor->GetNumberIngredientsInCauldron() >= 2) 
        {
            // tutorial alert for apprentices
            if (!bInitBottlePotionAlert && ClientPS->PotProbRole == EPotProbRoles::ROLE_APPRENTICE)
            {
                bInitBottlePotionAlert = true;
                ClientPS->ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "This cauldron has two ingredients!\nYou can bottle it.");
            }
            return TEXT("[F] Bottle Potion");
        }
		if(ClientPS->GetIngredientName().IsNone())
		{
		    bInitBottlePotionAlert = false;
			return TEXT("Find Ingredients!");
		}
		else
		{
		     bInitBottlePotionAlert = false;
			return TEXT("[E] Add Ingredient");
		}
	}
	else if(APotionActor* PotionActor = Cast<APotionActor>(GetOwner()))
	{
	    bInitBottlePotionAlert = false;
	    bFrogDoorAlert = false;
		return TEXT("[E] Pickup Potion");
	}
	else if(AMinigameInitActor* MinigameInitActor = Cast<AMinigameInitActor>(GetOwner()))
	{
	    bInitBottlePotionAlert = false;
	    bFrogDoorAlert = false;
		return ClientPS->GetHasCooties() ? TEXT("You cannot use, you are in love!") : TEXT("[E] Use");
	}
	else if(AMinigameInitUIActor* MinigameInitUIActor = Cast<AMinigameInitUIActor>(GetOwner()))
	{
	    bInitBottlePotionAlert = false;
	    bFrogDoorAlert = false;
		return ClientPS->GetHasCooties() ? TEXT("You cannot use, you are in love!") : TEXT("[E] Use");
	}
	else if(AFrogDoorActor* FrogDoorActor = Cast<AFrogDoorActor>(GetOwner()))
	{
	    bInitBottlePotionAlert = false;
	    if (!bFrogDoorAlert && ClientPS->NumFrogDoorAlerts < 2)
        {
            bFrogDoorAlert = true;
            ClientPS->ClientReceiveAlertMessage(EPotProbAlertTypes::TUTORIAL_ALERT, "Press E to use the frog tunnel\nwhen frogged!");
            ClientPS->NumFrogDoorAlerts ++;
        }
		// return (ClientPS->bIsFrogged || ClientCharacter->GetIsShrinking()) ? TEXT("[E] To Use Door") : TEXT("[E] To Use Door When Frogged");
		return TEXT("[E] Travel Through Frog Tunnel");
	}
	else if (APedestal* Pedestal = Cast<APedestal>(GetOwner()))
	{
	    bInitBottlePotionAlert = false; 
	    bFrogDoorAlert = false;
		return TEXT("[E] Read Orientation Scroll");
	}
	else if (AMirror* Mirror = Cast<AMirror>(GetOwner()))
	{
	    bInitBottlePotionAlert = false; 
	    bFrogDoorAlert = false;
		return TEXT("[E] Change Character");
	}
	else if (AVoteInitActor* VoteInitActor = Cast<AVoteInitActor>(GetOwner()))
	{
	    bInitBottlePotionAlert = false;
	    bFrogDoorAlert = false;
		if (ClientPS->bHasInitVote)
		{
			return TEXT("You've already called a vote!");
		} else if (!ClientPS->bCanInitVote)
		{
			return TEXT("Can't Ponder the Orb yet!");
		} else
		{
			return TEXT("[E] Ponder to call a Vote!");
		}
	}
	// Return an empty
	bInitBottlePotionAlert = false;
	bFrogDoorAlert = false;
	return TEXT("[E] Pick Up");
}

// Called when the game starts
void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();
	UInteractSubsystem* Subsystem = GetWorld()->GetSubsystem<UInteractSubsystem>();
	if(!Subsystem)
	{
		return;
	}
	Subsystem->AddComponent(this);
}

void UInteractComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UInteractSubsystem* Subsystem = GetWorld()->GetSubsystem<UInteractSubsystem>();
	if(!Subsystem)
	{
		return;
	}

	Subsystem->RemoveComponent(this);
	OnInteract.RemoveAll(this);
	OnInteractHeld.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}


// Called every frame
void UInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInteractComponent::NativeInteract(APawn* InstigatorPawn)
{
	if(GetOwnerRole() != ROLE_Authority)
	{
		return;
	}
	
	OnInteract.Broadcast(InstigatorPawn);
}

void UInteractComponent::NativeInteractHeld(APawn* InstigatorPawn)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	OnInteractHeld.Broadcast(InstigatorPawn);
}

void UInteractComponent::NativeInteractSuperHeld(APawn* InstigatorPawn)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	OnInteractSuperHeld.Broadcast(InstigatorPawn);
}
