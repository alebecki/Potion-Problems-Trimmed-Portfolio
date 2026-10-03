// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ChatFilterLibrary.generated.h"

/**
 * 
 */
UCLASS()
class POTIONPROBLEMS_DEV_API UChatFilterLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	FString static CensorMessage(const FString Message);

protected:
	static TSet<FString> IllegalWords;

	static void GeneratePrefixTree();
};