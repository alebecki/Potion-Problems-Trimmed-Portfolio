
#include "AlertWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

void UAlertWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UAlertWidget::InitializeAlert(const FString& Message, float Duration)
{
    // Set the total and remaining duration
    // subtract 0.1 to 
    TotalDuration = Duration - 0.1f;
    RemainingTime = Duration - 0.1f;

    // Set the alert message
    if (AlertMessage)
    {
        AlertMessage->SetText(FText::FromString(Message));
    }

    // Start a repeating timer to update the progress bar
    if (TotalDuration > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            this,
            &UAlertWidget::UpdateTimer,
            0.1f, // Update every 0.1 seconds
            true
        );
    }
    else
    {
        OnTimerEnd();
    }
}

void UAlertWidget::UpdateTimer()
{
    if (RemainingTime > 0.0f)
    {
        // Decrease the remaining time
        RemainingTime -= 0.1f;
    }
    else
    {
        // Timer has run out
        OnTimerEnd();
    }
}

void UAlertWidget::OnTimerEnd()
{
    // Stop the timer
    GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

    // Clear the text immediately
    if (AlertMessage)
    {
        AlertMessage->SetText(FText::GetEmpty());
    }  
}

void UAlertWidget::ClearAlertWidget()
{
    // Remove the widget from the viewport
    RemoveFromParent();
}