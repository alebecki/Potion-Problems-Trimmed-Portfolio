// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressBarWidget.h"

void UProgressBarWidget::SetPercent(float NewPercent)
{
    Percent = NewPercent;
    OnPercentChanged.Broadcast(Percent);
}