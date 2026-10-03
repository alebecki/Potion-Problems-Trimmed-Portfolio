// Fill out your copyright notice in the Description page of Project Settings.


#include "ToggleButton.h"

void UToggleButton::PostLoad()
{
	Super::PostLoad();

	OnClicked.RemoveDynamic(this, &UToggleButton::Toggle);
	OnClicked.AddDynamic(this, &UToggleButton::Toggle);
}

void UToggleButton::BeginDestroy()
{
	Super::BeginDestroy();

	OnClicked.RemoveDynamic(this, &UToggleButton::Toggle);
}

void UToggleButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!bIsToggled)
	{
		OriginalNormalBrush = GetStyle().Normal;
		OriginalHoveredBrush = GetStyle().Hovered;
	}
	
	UpdateButtonStyle();
}

void UToggleButton::Toggle()
{
	bIsToggled = !bIsToggled;
	UpdateButtonStyle();
}

void UToggleButton::UpdateButtonStyle()
{
	FButtonStyle ButtonStyle = GetStyle();
	
	ButtonStyle.SetNormal(bIsToggled ? ToggledBrush : OriginalNormalBrush);
	ButtonStyle.SetHovered(bIsToggled ? ToggledHoverBrush : OriginalHoveredBrush);
	
	SetStyle(ButtonStyle);
}
