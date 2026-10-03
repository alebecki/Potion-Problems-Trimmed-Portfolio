// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PaperZDCharacter.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "PaperFlipbookComponent.h"
#include "CoreMinimal.h"
#include "PotProbZDCharacter.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EAnimModels : uint8
{
	CHAR_MODEL_0 = 0,
	CHAR_MODEL_1 = 1,
	CHAR_MODEL_2 = 2,
	CHAR_MODEL_3 = 3,
	CHAR_MODEL_4 = 4,
	CHAR_MODEL_5 = 5,
	CHAR_MODEL_6 = 6,
	CHAR_MODEL_7 = 7,
	CHAR_MODEL_8 = 8,
	CHAR_MODEL_9 = 9,
	FROGGED_MODEL = 10,
	WEREFROG_MODEL = 11,
	CAMO_MODEL_0 = 12,
	CAMO_MODEL_1 = 13,
	CAMO_MODEL_2 = 14
};

class UPlayerNameTagWidget;
class UWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIngredientPickup, bool, bHasPickedUp);

UCLASS()
class POTIONPROBLEMS_DEV_API APotProbZDCharacter : public APaperZDCharacter
{
	GENERATED_BODY()

public:
	APotProbZDCharacter();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	void UpdateRenderPriorityBasedOnPosition();
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// Updates Sprite Color of this Character
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void UpdateSpriteColor(FLinearColor Color) const;
	// Behavior for Werefrog Potion
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerTransformIntoWerefrog();
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientUpdateWerefrogStatusOnHUD(bool bCreateNew);
	// Alert everyone a frog potion was consumed
	UFUNCTION(BlueprintCallable)
	void AlertDrinkFrogPotion();
	// Behavior for Untransforming
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerUntransformIntoWerefrog();
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void UpdateSpritePosition();

	void SetIsHoldingMove(bool Status);
	void SetCharacterDirectionality(FVector2D Direction);

	//Camouflage Potion Functions
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetIsCamouflaged(bool bVal);
	UFUNCTION(Client, Reliable)
	void ClientSetIsCamouflaged(bool bVal);
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void EndCamouflaged();
	UFUNCTION(Client, Reliable)
	void ClientEndCamouflaged();
	bool GetIsCamouflaged() const { return bIsCamouflaged; }
	//Clarivoyance Potion Functions
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerClarivoyanceStart(bool bVal);
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerEndClarivoyance();
	UFUNCTION(Client, Reliable)
	void ClientClairvoyanceStart(bool bVal);
	UFUNCTION(Client, Reliable)
	void ClientEndClairvoyance();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerUpdateCapsuleRadius(float Radius);
	//Shrinking Potion Status Function
	bool GetIsShrinking() const { return bIsShrinking; }
	//Werefrog Status
	bool GetIsWerefrog() const { return bIsWerefrog; }
	// Character Skin
	UPROPERTY(Replicated)
	int32 ModelOwnerId = -1;
	UPROPERTY(Replicated)
	int32 ModelOverride = -1;
	int32 PreviousModel = -1; // Model last frame

	UFUNCTION(Server, Reliable)
	void Server_SetModelOverride(EAnimModels Model);

	UFUNCTION(Server, Reliable)
	void Server_ClearModelOverride();
	
	// Function that returns the character's selected model index
	UFUNCTION(BlueprintCallable)
	EAnimModels GetPlayerSelectedModel() const;

	// Gets the in-game presentation model of the character. This accounts for frogging, etc.
	UFUNCTION(BlueprintCallable)
	EAnimModels GetPlayerDisplayModel() const;

	UFUNCTION(Server, Reliable)
	void Server_SetIngredientSprite(UPaperSprite* NewSprite);
	void SetIngredientSprite(UPaperSprite* NewSprite);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UWidgetComponent* PlayerText;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UWidgetComponent* EgoPlayerNameTag;
	bool bHidePlayerText = false;
	UFUNCTION(Server, Reliable)
	void SetPlayerText(const FString& NewText);
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPlayerNameTagWidget> PlayerNametagClass;
	void EgoOverridePlayerText(const FString& PlayerNameText, bool bIsUsingEgo);
	UFUNCTION(Server, BlueprintCallable, Reliable)
	void ServerChangeMovementSpeed(float Val);

	bool GetIsClarivoyant(){return bIsClarivoyant;}

	// Frog Functions
	void ToggleFrogProperties();
	
	// FOV / ROV
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEnablePostProcessComponent();
	void EnablePostProcessComponent();
	void HidePlayerComponents();
	void RevealPlayerComponents();

	/* Potion Variables */
	UFUNCTION(Client, Reliable)
	void ClientToggleCootiesEffects(bool HasCooties);
	UFUNCTION(Client, Reliable)
	void ClientUpdateDrinkText(bool bEffectActive);
	
	// Shrinking Potion
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Shrinking Potion")
	float ShrinkPotionPlayerScale = 0.5f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Shrinking Potion")
	float ShrinkPotionCameraFov = 70.0f;
	// TODO: This duration is not used to actually set the potion end timer. That is handled in blueprint.
	// Instead, this value must be updated to match the blueprint value and is exposed for convenience. We should
	// probably set the timer in C++ in the future for consistency.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Shrinking Potion")
	float ShrinkPotionDuration = 180.0f;
	// TODO: Player speed (move from blueprint)
	// TODO: Can fit through frog tunnels

	// Frog Potion
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Frog Potion")
	float FrogPotionSpeedMult = 1.75f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Frog Potion")
	float FrogPotionDuration  = 60.0f;

	// Camouflage Potion
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Camouflage Potion")
	float CamouflagePotionPlayerSpeed = 1.5f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Camouflage Potion")
	float CamouflagePotionDuration = 180.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Camouflage Potion")
	float CamouflagePotionCameraFov = 500.0f;
	// TODO: Can fit through frog tunnels

	// Clairvoyance Potion
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Clairvoyance Potion")
	float ClairvoyancePotionDuration = 180.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Clairvoyance Potion")
	float ClairvoyancePotionCameraFov = 500.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Clairvoyance Potion")
	float ClairvoyancePotionPlayerSpeed = 1.0f;

	// Disguise Potion
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Disguise Potion")
	float DisguisePotionDuration = 180.0f;
	
	// Love Potion
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Love Potion")
	float LovePotionDuration = 180.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Love Potion")
	float LovePotionPlayerSpeed = 1.5f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Designer Variables: Love Potion")
	float LovePotionCameraFov = 500.0f;

	// Public Potion Functions
	FString GetOriginalPlayerName() const { return OriginalPlayerName; }
	
	/* Ingredient Effects Deprecated*/
	FVector LastPosition;
	bool bIsChillyBoosted = false;
	FTimerHandle ChillyBoostTimerHandle;
	FTimerHandle MiniChillyBoostTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Designer Variables: Chilly Pepper")
	float ChillyBoostMultiplier = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Designer Variables: Chilly Pepper")
	float ChillyBoostSpawnTime = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Designer Variables: Chilly Pepper")
	float ChillyBoostDecayTime = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AActor> ChillyBoostTriggerClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinimumBoostSpawnDist = 5.0f;

	// Toggle SpeedBoost of 
	void ToggleChillyBoost(bool bNewStatus);
	void ActivateChillyMiniBoost();
	// Periodically spawns chilly boost triggers 
	void HandleSpawnChillyBoost();
	// Reset Speed after Delay for chilly boost
	void HandleMiniChillyBoost();

	// Proxy Chat
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UWidgetComponent* ProxyChatComponent;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UProxyChatWidget> ProxyChatWidgetClass;

	// Proxy Chat
	UFUNCTION(Server, Unreliable)
	void ServerProxyMessage(const FString& Message);
	UFUNCTION(NetMulticast, Unreliable)
	void ClientProxyMessage(const FString& Message);
	
	
	UFUNCTION(NetMulticast, Reliable)
	void UpdatePlayerNameTagColor_Multicast();

	UPROPERTY(BlueprintAssignable)
	FOnIngredientPickup OnIngredientPickup;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PotionStatusWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* WerefrogStatusIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* DisguiseStatusIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* EgoStatusIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* CootiesStatusIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* UnstableStatusIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* CamoStatusIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* ClairvoyanceStatusIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* ShrinkingStatusIconTexture;

	// Public Timers
	FTimerHandle CamouflageTimer;
	FTimerHandle ClarivoyanceTimer;
	FTimerHandle WerefrogTimer;
	
protected:
	// Different Animations for the character
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	TArray<TSubclassOf<UPaperZDAnimInstance>> Anims;
	// How fast the player should move
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	float DefaultMovementSpeed = 200.0f;
	UPROPERTY(ReplicatedUsing=OnRep_MovementSpeed)
	float MovementSpeed = 200.0f;
	UFUNCTION()
	void OnRep_MovementSpeed();

	float DefaultCameraFov = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	float CameraZoomOutLevel = 500.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	FLinearColor TroublemakerNametagColor;
	UPROPERTY(BlueprintReadOnly, Replicated)
	FVector2D Directionality;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bIsHoldingMove;
	
	//Camouflage Potion stuff
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_IsCamouFlaged)
	bool bIsCamouflaged;
	UFUNCTION()
	void OnRep_IsCamouflaged();

	//Clarivoyance Potion Stuff
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_IsClarivoyant)
	bool bIsClarivoyant;
	UFUNCTION()
	void OnRep_IsClarivoyant();

	// Shrinking Potion
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsShrinking)
	bool bIsShrinking = false;
	
	UFUNCTION()
	void OnRep_IsShrinking();
	UFUNCTION(BlueprintCallable)
	void SetIsShrinking(bool isShrinking);
	FTimerHandle ShrinkingPotionTimer;
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ClearShrinkingPotionEffects();
	UFUNCTION(Client,Reliable)
	void ClientSetIsShrinking();
	
	// Werefrog stuff
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bIsWerefrog;
	void EndWerefrogTime();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", Replicated)
	UPaperSpriteComponent* IngredientSprite;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Designer Variables")
	float UnfroggedRadius = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Designer Variables")
	float FroggedRadius = 5.0f;
	UPROPERTY()
	UUserWidget* WerefrogStatusWidgetInstance;
	UPROPERTY()
	UUserWidget* CamoStatusWidgetInstance;
	UPROPERTY()
	UUserWidget* ClairvoyanceStatusWidgetInstance;
	
	UPROPERTY(ReplicatedUsing = OnRep_IngredientSprite)
	UPaperSprite* CurrentIngredientSprite;

	UFUNCTION()
	void OnRep_IngredientSprite();

	void SetCameraFov(float FOV);
	void ResetCameraFov();
	void ToggleShrinkCamera(bool shrunk);
	
	UPROPERTY(ReplicatedUsing = OnRep_PlayerNameTag)
	FString PlayerNameTag;
	FString OriginalPlayerName = "";
	UFUNCTION()
	void OnRep_PlayerNameTag();
};
