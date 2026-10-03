#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterInfo.generated.h"

enum class EAnimModels : uint8;
class UImage;
class UTextBlock;
class UButton;
struct FCharacterData;
class UToggleButton;
/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UCharacterInfo : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	UPROPERTY(meta = (BindWidget))
	UImage* CharacterImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterName;
	UPROPERTY(meta = (BindWidget))
	UToggleButton* CharacterButton;

	FText CharacterLore;
	EAnimModels CharacterModel;

	void InitializeTextAndIcons(const FCharacterData& CharacterInfo);
	void SetWidgetInstigator(UUserWidget* Widget) { WidgetInstigator = Widget; }

protected:
	UFUNCTION()
	void OnCharacterWidgetClicked();


	UPROPERTY()
	TObjectPtr<UUserWidget> WidgetInstigator;
};
