// Fill out your copyright notice in the Description page of Project Settings.


#include "ObservatoryScreenWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"


void UObservatoryScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StarSlot = Cast<UCanvasPanelSlot>(StarWidget->Slot);
}


void UObservatoryScreenWidget::NativeDestruct()
{
	Super::NativeDestruct();
}


void UObservatoryScreenWidget::UpdateStarLocation(FVector2D NewLocation) const
{
	if (!StarSlot)
	{
		UE_LOG(LogTemp, Warning, TEXT("StarSlot not set."));
		return;
	}
	
	StarSlot->SetPosition(NewLocation);
}





