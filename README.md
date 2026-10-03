# Potion Problems

[![Potion Problems trailer](https://img.youtube.com/vi/TmR3AQp_Ekk/hqdefault.jpg)](https://www.youtube.com/watch?v=TmR3AQp_Ekk)

**[▶ Watch the trailer](https://www.youtube.com/watch?v=TmR3AQp_Ekk)** · **[Play it on Steam](https://store.steampowered.com/app/3306050/Potion_Problems/)**

Potion Problems is an online multiplayer social-deduction game built in **Unreal Engine 5.4**. Apprentices work together to brew potions while avoiding secret troublemakers trying to turn them into frogs.

This repository is a **code and Blueprint portfolio snapshot** of the final project. Art, audio and third-party marketplace content have been removed (see [What's not included](#whats-not-included)).

---

## My contributions

I was the lead engineer on a team of 6 engineers. I worked on the project from May 2024 to May 2025 and had 63 engineering tasks assigned to me.

### 1. Crafting system (core gameplay loop)
I built the crafting loop from the ground up: the cauldron object, picking up ingredients (with a hold-to-pick-up interaction), completing a recipe with three ingredients, potion rarity, rerolling recipes, crafted potions going into the player's inventory, and the crafted-potions list.

- **Author:** [`CauldronActor.cpp`](Source/PotionProblems_Dev/CauldronActor.cpp), [`IngredientActor.cpp`](Source/PotionProblems_Dev/IngredientActor.cpp), [`IngredientData.h`](Source/PotionProblems_Dev/IngredientData.h), [`RarityData.h`](Source/PotionProblems_Dev/RarityData.h), [`CraftedRecipeDisplayActor.cpp`](Source/PotionProblems_Dev/CraftedRecipeDisplayActor.cpp), [`PotionCraftedActor.cpp`](Source/PotionProblems_Dev/PotionCraftedActor.cpp)
- **Major contributor:** [`PotProbPlayerState.cpp`](Source/PotionProblems_Dev/PotProbPlayerState.cpp) (inventory, rarity, reroll), [`PotProbZDCharacter.cpp`](Source/PotionProblems_Dev/PotProbZDCharacter.cpp) (held-ingredient and bottled-potion display), [`InteractComponent.cpp`](Source/PotionProblems_Dev/InteractComponent.cpp) (pickup), [`RecipeWidget.cpp`](Source/PotionProblems_Dev/RecipeWidget.cpp), [`PotionRecipeSelectionWidget.cpp`](Source/PotionProblems_Dev/PotionRecipeSelectionWidget.cpp)
- **Blueprints:** [`BP_CauldronActor`](Content/PotionProblems/Blueprints/Actors/BP_CauldronActor.uasset), [`BP_IngredientActor`](Content/PotionProblems/Blueprints/Actors/BP_IngredientActor.uasset), [`BP_CraftedRecipeDisplayActor`](Content/PotionProblems/Blueprints/Actors/BP_CraftedRecipeDisplayActor.uasset), [`BP_PotionCraftedActor`](Content/PotionProblems/Blueprints/Actors/BP_PotionCraftedActor.uasset), [`WheelOfFortunePotion_BP`](Content/PotionProblems/Blueprints/Actors/Potions/WheelOfFortunePotion_BP.uasset), [`DataTables/`](Content/PotionProblems/Blueprints/DataTables)

### 2. Online sessions, lobby and platform builds
I worked on lobby setup and on finding and joining sessions, including searching for sessions when the page opens, loading indicators and loading-into-lobby feedback. I fixed rejoin and session bugs, made the Steam builds (with our Steam app ID), and researched and made the Epic Games Store build.

- **Major contributor:** [`PotProbOnlineSubsystem.cpp`](Source/PotionProblems_Dev/PotProbOnlineSubsystem.cpp), [`MainMenuWidget.cpp`](Source/PotionProblems_Dev/MainMenuWidget.cpp) (session search, loading indicator), [`FoundSessionWidget.cpp`](Source/PotionProblems_Dev/FoundSessionWidget.cpp)
- **Config:** [`DefaultEngine.ini`](Config/DefaultEngine.ini) (Steam online subsystem)
- **Blueprints:** [`MainMenu_BP`](Content/PotionProblems/Blueprints/UI/MainMenuUI/MainMenu_BP.uasset), [`FoundSessionWidget_BP`](Content/PotionProblems/Blueprints/UI/MainMenuUI/FoundSessionWidget_BP.uasset), [`Lobby_BP`](Content/PotionProblems/Blueprints/UI/LobbyUI/Lobby_BP.uasset)

### 3. HUD and in-game UI
- the in-game map, showing player, claimed-cauldron and ingredient locations, with variable cauldron icons
- the status effect indicator
- the Werefrog countdown timer
- the tutorial widget
- fixes for Mac-specific UI bugs

- **Author:** [`TutorialWidget.cpp`](Source/PotionProblems_Dev/TutorialWidget.cpp)
- **Major contributor:** [`HUDWidget.cpp`](Source/PotionProblems_Dev/HUDWidget.cpp) (map, cauldron icons, status effects)
- **Blueprints:** [`Minimap`](Content/PotionProblems/Blueprints/UI/HUD_UI/Minimap.uasset), [`WerefrogCountdownWidget_BP`](Content/PotionProblems/Blueprints/UI/HUD_UI/WerefrogCountdownWidget_BP.uasset), [`TutorialScreenWidget_BP`](Content/PotionProblems/Blueprints/UI/HUD_UI/TutorialComic/TutorialScreenWidget_BP.uasset)

### 4. Win conditions and end-of-match screen
- the Apprentice win condition
- the end-of-match screen system, built from scratch, with a screen for each team, including a "why your team won" breakdown
- fixing the win screen not matching between host and clients
- fixing a timing bug in the vote countdown

- **Author:** the end-of-match screen system: the end-game win, win-reason and stats panels in [`HUDWidget.cpp`](Source/PotionProblems_Dev/HUDWidget.cpp) / [`HUDWidget.h`](Source/PotionProblems_Dev/HUDWidget.h), and the [`UI/WinScreenUI/`](Content/PotionProblems/Blueprints/UI/WinScreenUI) Blueprints
- **Major contributor:** [`PotProbGameMode.cpp`](Source/PotionProblems_Dev/PotProbGameMode.cpp) (`CheckWin`, `EndGame`)
- **Blueprints:** [`WinScreenWidget_BP`](Content/PotionProblems/Blueprints/UI/WinScreenUI/WinScreenWidget_BP.uasset), [`EndGameStatCard`](Content/PotionProblems/Blueprints/UI/HUD_UI/EndGameStatCard.uasset)

### Pipeline and technical leadership
- maintained the technical design document
- set teammates up with Perforce and Unreal
- maintained the stable build
- bug fixed mac specific bugs
- profiled with Unreal Insights and optimized GPU memory
- fixed misc bugs that other engineers couldn't get to

## Project layout

```
PotionProblems_Dev.uproject
Config/                         Project settings (.ini)
Source/PotionProblems_Dev/      All gameplay C++ code
Content/PotionProblems/
  Blueprints/                   Gameplay & UI Blueprints, data tables
  Input/                        Enhanced Input actions & mapping contexts
  Levels/TestLevels/MiniLevel   Main gameplay map
```

## Building

- **Engine:** Unreal Engine 5.4
- **Plugins required:**
  - [PaperZD](https://www.fab.com/listings/6664e3b5-e376-47aa-a0dd-f7bbbd5b93c0) (marketplace)
  - [Async Loading Screen](https://www.fab.com/listings/f8aabb9a-7c96-4f79-97ff-04bcc146e595) (marketplace)
  - **Wwise 2024.1** (Audiokinetic). Install it through the Audiokinetic Launcher and integrate it into the project. The code depends on the `AkAudio` module.
- Online play uses `OnlineSubsystemSteam` (EOS is enabled as well).

Once the plugins are installed, right-click `PotionProblems_Dev.uproject` and choose **Generate Visual Studio project files**, then build the `PotionProblems_DevEditor` target.

## What's not included

To keep the repo small and respect licensing, these were removed:

- Art: sprites, textures, animations, environment art
- Audio: the Wwise project, sound banks, and the Wwise plugin (Audiokinetic's license doesn't allow redistributing them)
- Marketplace content: VFX packs and the PaperZD / Async Loading Screen plugins
- Template, test and prototype levels
- Editor-generated folders: `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`

The project still opens in the editor, but sprites and textures show up as missing references. The C++ code and the Blueprint logic are complete.
