// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActionBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UEGOAP_API UActionBase : public UObject
{
	GENERATED_BODY()

	// Objects (Items involved in the action)
	TArray<UObject> Objects;
	
	// Preconditions (Facts that must be true for the action to work)
	virtual TMap<FName, bool> Preconditions();

	// Effects (How the facts in the state change when the action is complete)
	
};
