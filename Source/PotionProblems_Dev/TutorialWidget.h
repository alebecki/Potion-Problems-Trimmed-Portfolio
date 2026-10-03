// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Slate/SlateBrushAsset.h"
#include "TutorialWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (BindWidget))
	UButton* BackButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* NextButton;
	UPROPERTY(Meta = (BindWidget))
	UButton* XButton;

	void SetWidgetInstigator(UUserWidget* Widget);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void NextSlide();
	UFUNCTION()
	void PrevSlide();
	UFUNCTION()
	void Close();

	//UFUNCTION()
	//USlateBrushAsset GetBrush();
	UPROPERTY(BlueprintReadWrite)
	int SlideIndex = 0;
	//UPROPERTY(BlueprintAssignable)
	int NumSlides = 4;

	UPROPERTY()
	TObjectPtr<UUserWidget> WidgetInstigator;
};
