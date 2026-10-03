// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialWidget.h"

#include "HUDWidget.h"
#include "Components/Button.h"

void UTutorialWidget::SetWidgetInstigator(UUserWidget* Widget)
{
	WidgetInstigator = Widget;
	if (WidgetInstigator != nullptr)
	{
		WidgetInstigator->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UTutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();
	NextButton->OnClicked.AddDynamic(this, &UTutorialWidget::NextSlide);
	BackButton->OnClicked.AddDynamic(this, &UTutorialWidget::PrevSlide);
	XButton->OnClicked.AddDynamic(this, &UTutorialWidget::Close);
}

void UTutorialWidget::NativeDestruct()
{
	Super::NativeDestruct();
	NextButton->OnClicked.RemoveDynamic(this, &UTutorialWidget::NextSlide);
	BackButton->OnClicked.RemoveDynamic(this, &UTutorialWidget::PrevSlide);
	XButton->OnClicked.RemoveDynamic(this, &UTutorialWidget::Close);
}

void UTutorialWidget::NextSlide()
{
	if (++SlideIndex >= NumSlides)
	{
		SlideIndex = 0;
	}
}

void UTutorialWidget::PrevSlide()
{
	if (--SlideIndex < 0)
	{
		SlideIndex = NumSlides-1;
	}
}

void UTutorialWidget::Close()
{
	if (WidgetInstigator)
	{
		WidgetInstigator->SetVisibility(ESlateVisibility::Visible);
	}
	RemoveFromParent();
}
