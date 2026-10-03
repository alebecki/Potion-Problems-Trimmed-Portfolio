// Fill out your copyright notice in the Description page of Project Settings.


#include "CountdownWidget.h"
#include "PotProbPlayerController.h"
#include "Components/TextBlock.h"


void UCountdownWidget::NativeConstruct()
{
    Super::NativeConstruct();
    bHasScriptImplementedTick = true;
    Timer = CountdownDuration;
}

void UCountdownWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    Timer -= InDeltaTime;
    TimerText->SetText(FText::FromString(FString::Printf(TEXT("%.1f SECONDS!"), Timer)));
    if (Timer <= 0.0f)
    {
        RemoveFromParent();
        Timer = FLT_MAX;
    }
}
