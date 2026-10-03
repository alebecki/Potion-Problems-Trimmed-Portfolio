// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PotProbEnums.generated.h"

UENUM(BlueprintType)
enum class EPotProbRoles : uint8
{
	/* Default, INVALID role, should not be used due to ReplicatedUsing issues */
	ROLE_NONE        UMETA(DisplayName = "None"),
	/* Role unassigned yet (game not started) */
	ROLE_UNASSIGNED  UMETA(DisplayName = "Unassigned"),
	/* Apprentice role */
	ROLE_APPRENTICE  UMETA(DisplayName = "Apprentice"),
	/* Troublemaker role */
	ROLE_TROUBLEMAKER UMETA(DisplayName = "Troublemaker")
};

UENUM(BlueprintType)
enum class EPotProbEffects : uint8
{
	PLAYER_SPEED     UMETA(DisplayName = "Player Speed"),
	PLAYER_FOV       UMETA(DisplayName = "Player FOV"),
	SPRITE_SIZE      UMETA(DisplayName = "Sprite Size"),
	SPRITE_SKIN      UMETA(DisplayName = "Sprite Skin"),
	EFFECT_SOUND     UMETA(DisplayName = "Effect Sound"),
	EFFECT_VISUAL    UMETA(DisplayName = "Effect Visual")
};

UENUM(BlueprintType)
enum class EPotProbAlertTypes : uint8
{
	HIGH_PRIORITY    UMETA(DisplayName = "High Priority Alert"),
	LOW_PRIORITY     UMETA(DisplayName = "Low Priority Alert"),
	TUTORIAL_ALERT    UMETA(DisplayName = "Tutorial Alert"),
};

UENUM(BlueprintType)
enum class EPotProbRoomTypes : uint8
{
	CLASSROOM     UMETA(DisplayName = "Potions Classroom"),
	OBSERVATORY    UMETA(DisplayName = "Observatory"),
};