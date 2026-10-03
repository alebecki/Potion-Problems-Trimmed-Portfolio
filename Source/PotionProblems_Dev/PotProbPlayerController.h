// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "InputAction.h"
#include "PotProbGameMode.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "PotProbPlayerController.generated.h"

/** FORWARD DECLARATIONS **/
enum class EPotProbRoles : uint8;

struct FCharacterData;

class AMinigameInitActor;
class APotProbPlayerState;
class UAkAudioEvent;
class UCharacterCustomizeWidget;
class UDisguiseHUDWidget;
class UHUDWidget;
class UInputMappingContext;
class UInputAction;
class UInteractComponent;
class UPotionRecipeSelectionWidget;
class UPlayerRoleWidget;
class UVoteWidget;
class UVoteCountdownWidget;
class UProgressBarWidget;


/**
 * Potion Problems Player Controller
 */
UCLASS()
class POTIONPROBLEMS_DEV_API APotProbPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	APotProbPlayerController();
	
	/** Input MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Widgets (Created from classes assigned in Blueprint)**/
	// Soft Ptr to the HUDWidget attached to this player controller
	UPROPERTY(Transient, BlueprintReadWrite)
	TObjectPtr<UHUDWidget> HUDWidgetInstance;
	// Soft Ptr to the VoteWidget attached to this player controller
	UPROPERTY(Transient)
	TObjectPtr<UVoteWidget> VoteWidgetInstance;
	// Soft Ptr to the Potion Recipe SelectionWidget attached to this player controller
	UPROPERTY(Transient)
	TObjectPtr<UPotionRecipeSelectionWidget> AssignmentSelectionInstance;
	// Soft Ptr to the Character Customize Widget attached to this player controller
	UPROPERTY(Transient)
	TObjectPtr<UCharacterCustomizeWidget> CharacterCustomizeWidgetInstance;
	// Soft Ptr to the Disguise HUD Widget attached to this player controller
	UPROPERTY(Transient)
	TObjectPtr<UDisguiseHUDWidget> DisguiseHUDWidgetInstance;

	/** Role classes (set in Blueprint) **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPlayerRoleWidget> PlayerRoleClass;
	UPlayerRoleWidget* PlayerRoleWidgetInstance = nullptr;
	bool bFirstRoleWidgetInstance = true;

	/** Voting properties **/
	// Replication for voting UI 
	UPROPERTY(Replicated)
	bool bVotingUIVisible = false;

	/** WWise Audio Events **/
	UPROPERTY(EditAnywhere)
	UAkAudioEvent* BottleEvent;
	UPROPERTY(EditAnywhere)
	UAkAudioEvent* PickupSound;
	UPROPERTY(EditAnywhere)
	UAkAudioEvent* DropSound;
	UPROPERTY(EditAnywhere)
	UAkAudioEvent* FrogSound;
	UPROPERTY(EditAnywhere)
	UAkAudioEvent* DrinkSound;
	UPROPERTY(EditAnywhere)
	UAkAudioEvent* VoteSound;
	UPROPERTY(EditAnywhere)
	UAkAudioEvent* StopLobbyMusic;

	UPROPERTY(EditAnywhere)
	UAkAudioEvent* StopRoomNoise;

	UPROPERTY(EditAnywhere)
	UAkAudioEvent* StartRoomNoise;

	UPROPERTY(EditAnywhere)
	UAkAudioEvent* StartChime;

	UPROPERTY(EditAnywhere)
	UAkAudioEvent* WinSoundApprentice;

	UPROPERTY(EditAnywhere)
	UAkAudioEvent* WinSoundTroublemaker;

	UPROPERTY(EditAnywhere)
	UAkAudioEvent* VoteEndSound;

	UPROPERTY(EditAnywhere)
	UAkAudioEvent* ReportStinger;

	UPROPERTY(EditAnywhere)
	UAkAudioEvent* StopWheelOfFortuneLoop;

	UPROPERTY(EditAnywhere)
	UAkAudioEvent* FootstepSound;


	UPROPERTY(EditAnywhere)
	UAkAudioEvent* FrogstepSound;

protected:
	/** Input Actions **/
	// Action when player moves
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	// Action when player opens chat
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ChatAction;
	// Action when player opens proxy chat
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ProxyChatAction;
	// Action when player drinks (Activates) their potion
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DrinkPotionAction;
	// Action when player interacts with something
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;
	// Action when player wants to open the map
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MapAction;
	// Action when player wants to use a charge of their potion (if they have any)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ChargeUseAction;
	// SuperHeld action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractHeldAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* BottleUpAction;
	//Action to discard ingredient
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DiscardIngredientAction;

	/** Ingredient Effects **/
	FName LastPickedUpIngredient = NAME_None;

	/** Interaction **/
	// The Distance from which the player controller will be able to interact with other items
	UPROPERTY(Replicated)
	float InteractionDistance = 10.0f;
	
	/** Designer Variables*/
	// The HUD Widget that player controller should create. Should be of type UHUDWidget
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	TSubclassOf<UHUDWidget> HUDWidgetClass;
	// The Voting Widget that player controller should create. Should be of type UVoteWidget
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	TSubclassOf<UVoteWidget> VoteWidgetClass;
	// The Voting Countdown Widget that player sees when voting starts. Should be of type UVoteCountdownWidget
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	TSubclassOf<UVoteCountdownWidget> VoteCountdownWidgetClass;
	// The RecipeSelection Widget that player controller should create. Should be of type UPotionRecipeSelectionWidget
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	TSubclassOf<UPotionRecipeSelectionWidget> AssignmentSelectionClass;
	// The CharacterCustomize Widget that player controller should create. Should be of type UCharacterCustomizeWidget
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	TSubclassOf<UCharacterCustomizeWidget> CharacterCustomizeWidgetClass;
	// The Disguise Widget that player controller should create. Should be of type UDisguiseHUDWidget
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	TSubclassOf<UDisguiseHUDWidget> DisguiseHUDWidgetClass;
	// The Frogged Tutorial Screen Widget that player controller should create
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	TSubclassOf<UUserWidget> FroggedTutorialScreenWidgetClass;
	// The Werefrog Countdown Widget Class for Telescope Win Con
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	TSubclassOf<UUserWidget> WerefrogCountdownWidgetClass;
	// The Cauldron Switcheroo Warning Widget Class for Telescope Win Con
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	TSubclassOf<UUserWidget> SwitcherooWarningWidgetClass;

	
	// Proxy Designer Variable
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	float ProxyTimeoutTime = 0.1f;

	// Interaction distances for different player states
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	float DefaultInteractDistance = 10.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Designer Variables")
	float WerefrogInteractDistance = 20.0f;
	
public:
	/** ENGINE API OVERRIDES **/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

	/** INTERACTION **/
	UFUNCTION(Server, Reliable)
	void Server_ChangeInteractDistance(bool bIsWerefrog);
	float GetInteractionDistance() const { return InteractionDistance; }

	/** UI DISPLAY **/
	UFUNCTION(BlueprintCallable)
	UHUDWidget* GetHUDWidgetInstance();
	void ToggleHUDVisibility();
	UFUNCTION(Server, Reliable)
    void Server_ToggleNametags();
	UFUNCTION(Client, Reliable)
	void Client_ToggleNametags(const TArray<APotProbZDCharacter*>& AllCharacters);
	bool bNametagsVisible = true;

	// Client UI-related RPCs
	// clear recipe info card ingredient highlights
	UFUNCTION(Client, Reliable)
	void Client_ClearHUDIngredientSelections();
	UFUNCTION(BlueprintCallable, Client, Reliable) // TODO: remove rpc
	void Client_ShowPlayerFroggedTutorial();
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_ShowPlayerStartRole(EPotProbRoles PlayerRole, int32 TroubleMakerCount);

	/** GAME LOOP FUNCTIONALITY */
	UFUNCTION(Client, Reliable)
	void Client_UpdateNumInGamePlayers(int NumPlayers);
	UFUNCTION(Client, Reliable)
	void Client_UpdateLocalLoopStatusUI();
	
	/** LOBBY FUNCTIONALITY */
	UFUNCTION(Client, Unreliable) // TODO: make this net multicast
	void Client_UpdateLocalLobbyStatusUI();

	/** DEBUG FUNCTIONS **/
	UFUNCTION(Server, Reliable)
	void DEBUG_Server_StartGame();
	UFUNCTION(Server, Reliable)
	void DEBUG_Server_AddApprenticeCraftedPotionByOne();
	
    /** VOTING FUNCTIONALITY */
	// Toggle voting UI visibility
	UFUNCTION(Client, Reliable)
	void Client_ToggleVotingUI(bool Visibility);
    // Client RPC that updates local widget
	UFUNCTION(Client, Reliable)
	void Client_UpdateLocalVoteWidget(bool bUpdateVotingList = false, bool bCanVote = false, bool bIsTieBreaker = false, const TArray<APotProbPlayerState*>& TiedPlayers = {}, bool bResetPlayerSelected = false);
	UFUNCTION(Server, Reliable)
    void Server_UpdateAllVoteWidgets();
    UFUNCTION(Server, Reliable)
    void Server_StartVotingPhase();
    UFUNCTION(Server, Reliable)
    void Server_ProcessVote(const FString& PlayerName, const FString& VoterName);
	UFUNCTION(Server, Reliable)
	void Server_StartVoteCountdown();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_VoteCountdownWidgetPopUp();
	
	// Server RPC that will change ingredient name and holding status
	// which will then in turn update local text
	UFUNCTION(Server, Reliable)
	void Server_ChangeIngredientStatus(const FName Name, bool bIsHoldingIngredient);
	
	/** POTIONS FUNCTIONALITY */ 
	void FilloutPotionDistributionUI(const TArray<FRecipeStruct>& DistributedRecipes);
	void AddBackInputComponent();

	/** CHARACTER CUSTOMIZATION FUNCTIONALITY */
	void SelectCharacter(const FCharacterData& CharacterData);
	UFUNCTION(Server, Reliable)
	void Server_RequestModelUpdate(int32 modelIndex);
	
	/** TELESCOPE WIN CON FUNCTIONALITY */
	UFUNCTION()
	TSubclassOf<UUserWidget> GetWerefrogCountdownWidgetClass();
	UFUNCTION()
	TSubclassOf<UUserWidget> GetSwitcherooWarningWidgetClass();
	UFUNCTION(Server, Reliable)
	void DisableTelescopeInitActor(AMinigameInitActor* InitActor);

	/** AUDIO **/
	UFUNCTION(Client, Reliable)
	void PlayPotionSound();
	
	UFUNCTION(Client, Reliable)
	void PlayFrogSound();

	UFUNCTION(Client, Reliable)
	void StopMusic();

	UFUNCTION(Client, Reliable)
	void PlayStartChime();

	UFUNCTION(Client, Reliable)
	void PlayPotionDrink(UAkAudioEvent* Drink);

	UFUNCTION(Client, Reliable)
	void PlayPickupNoise();

	UFUNCTION(Client, Reliable)
	void PlayReportStinger();

	UFUNCTION(Client, Reliable)
	void PlayStopWheel();

	UFUNCTION(Client, Reliable)
	void PlayVotedOutSFX();


	UFUNCTION(Client, Reliable, BlueprintCallable)
	void PlayFootstep();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void PlayFrogstep();

	UFUNCTION(Client, Reliable)
	void PlayGameOverSound(bool ApprenticeWin);

	void ChangeRoomNoise(FName Room);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ChangeMusic(EPotProbPhases NewPhase);

	/** SESSION MANAGEMENT **/
	UFUNCTION(Client, Reliable)
	void LeaveSessionAndReturnToMenu();
	void OnSessionDestroyed(FName SessionName, bool bWasSuccessful);
	void OnSessionLeft(FName SessionName, bool bWasSuccessful);
	UFUNCTION(Client, Reliable)
	void LeaveSessionAndCloseGame();
	void OnSessionDestroyedQuitGame(FName SessionName, bool bWasSuccessful);
	
	UFUNCTION(Client, Reliable)
	void Client_ReturnToMainMenu();

	UFUNCTION(Client, Reliable)
	void Client_UpdateLoopReason(bool bKickAll);
	UFUNCTION(Client, Reliable)
	void Client_IncrementHUDGameState();

	void UpdateLocalCharCustomUI();

	UFUNCTION(Server, Reliable)
	void Server_WipeIngredientName(APotProbPlayerState* PlayerS);

protected:
	/** Engine API Overrides **/
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	
	UFUNCTION(Client, Reliable)
	void Client_OnPossess();
	
	/** Local input handling functions */
	void OnInputStarted();

	// Proxy
	bool bCanProxyChat = true;
	void SetCanProxyChat() {bCanProxyChat = true;}
	
	void OnMove(const FInputActionInstance& Instance, const ETriggerEvent EventType);
	void OnProxyChatAction(const FInputActionInstance& Instance);
	void MoveCompleted(const FInputActionValue& Value, const ETriggerEvent EventType);
	void OnDrinkPotion(const FInputActionInstance& Instance, const ETriggerEvent EventType);
	void OnInteractAction(const FInputActionInstance& Instance, const ETriggerEvent EventType);
	void OnMapAction(const FInputActionInstance& Instance, const ETriggerEvent EventType);
	void OnUseChargesAction(const FInputActionInstance& Instance, const ETriggerEvent EventType);
	void OnInteractHeld(const FInputActionInstance& Instance, const ETriggerEvent EventType);
	void OnInteractSuperHeld(const FInputActionInstance& Instance, const ETriggerEvent EventType);
	void OnInteractSuperHeldStarted(const FInputActionInstance& Instance, const ETriggerEvent EventType);
	void OnInteractSuperHeldOngoing(const FInputActionInstance& Instance, const ETriggerEvent EventType);
	void OnInteractSuperHeldCanceled(const FInputActionInstance& Instance, const ETriggerEvent EventType);
	void OnDiscardIngredientAction(const FInputActionInstance& Instance, const ETriggerEvent EventType);

	// OnInteractSuperHeld members
	// will update to hold time threshold of the input action in editor
	float SuperHeldTimeThreshold = 2.0f;
	UPROPERTY()
	UProgressBarWidget* SuperHeldProgressBarWidget = nullptr;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UProgressBarWidget> SuperHeldProgressBarWidgetClass = nullptr;
	
	/** Input RPCs to communicate actions with server **/
	// Send info to server to tell move direction (for replicating movement)
    UFUNCTION(Server, Reliable)
    void Server_UpdateCharacterDirection(FVector2D MovementVector);
    // Send info to server to tell it that client is moving (for replicating animation)
    UFUNCTION(Server, Reliable)
    void Server_UpdateIsHoldingMove(ETriggerEvent Status);
	// Send info to server to tell it that Client is trying to interact with something
	UFUNCTION(Server, Reliable)
	void Server_AttemptInteractWithObject(APawn* InstigatorPawn, UInteractComponent* TargetInteract);
	// Send info to server to tell it that Client is trying to hold interact with something
	UFUNCTION(Server, Reliable)
	void Server_AttemptHoldInteractWithObject(APawn* InstigatorPawn, UInteractComponent* TargetInteract);
	UFUNCTION(Server, Reliable)
	void Server_AttemptSuperHoldInteractWithObject(APawn* InstigatorPawn, UInteractComponent* TargetInteract);
	// Send info to server to tell it that Client is trying to use its charges
	UFUNCTION(Server, Reliable)
	void Server_AttemptUseCharges(EPotionChargeType PotionChargeType, int32 NumCharges);

	/** Ingredient Effect Implement */
	bool HandleOnInteractIngredientEffect(FName Ingredient);
	void HandleOnInteractPlaceEffect();
	
	/** Potion Implementation */
	// Send info to server to request to activate potion (If we have any)
	UFUNCTION(Server, Reliable)
	void Server_AttemptActivatePotion();

	/** Client UI-related RPCs **/
	UFUNCTION(Client, Reliable)
	void Client_DisplayIngredientText(bool bVal);
	UFUNCTION(Client, Reliable)
	void Client_DisplaySwitcharooMessage();

	/* Console Debug Functions */ // #############################################################
	UFUNCTION(Exec)
	void DebugDisplayApprenticeHUD();
	UFUNCTION(Exec)
	void DebugDisplayTroublemakerHUD();
	UFUNCTION(Exec)
	void DebugDisplayLobbyHUD();
	UFUNCTION(Exec)
	void DebugDisplayInGameHUD();
	UFUNCTION(Exec)
	void DebugDisplayEndGameHUD();
	UFUNCTION(Exec)
	void DebugToggleHUD();
	UFUNCTION(Exec)
	void DebugToggleNametags();

	// Potions
	UFUNCTION(Exec)
	void DebugGiveCamouflagePotion();
	UFUNCTION(Exec)
	void DebugGiveClairvoyancePotion();
	UFUNCTION(Exec)
	void DebugGiveDisguisePotion();
	UFUNCTION(Exec)
	void DebugGiveEgoPotion();
	UFUNCTION(Exec)
	void DebugGiveLovePotion();
	UFUNCTION(Exec)
	void DebugGiveQuestionmarkPotion();
	UFUNCTION(Exec)
	void DebugGiveShrinkingPotion();
	UFUNCTION(Exec)
	void DebugGiveTranslocationPotion();
	UFUNCTION(Exec)
	void DebugGiveWheelOfFortunePotion();
	UFUNCTION()
	void DebugGiveFrogPotion();
	

	// Ingredients
	UFUNCTION(Exec)
	void DebugGiveSunFriedEyeball();
	UFUNCTION(Exec)
	void DebugGivePhoenixClippings();
	UFUNCTION(Exec)
	void DebugGiveJeff();
	UFUNCTION(Exec)
	void DebugGiveNothingBagel();
	UFUNCTION(Exec)
	void DebugGiveKidneyBean();
	UFUNCTION(Exec)
	void DebugGiveChillyPepper();
	UFUNCTION(Exec)
	void DebugGiveSpirits();
	UFUNCTION(Exec)
	void DebugGiveRomanceNovel();
	

	// Helper Debug Functions
	UFUNCTION(Server, Reliable)
	void DebugFindAndGivePotion(const FString& PotionName);

	// Vote Debug Function
	UFUNCTION(Exec)
	void DebugStartVotingPhase();
	
	/* End Console Debug Functions */ // #############################################################
};
