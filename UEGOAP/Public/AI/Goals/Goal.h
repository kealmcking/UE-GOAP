// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Agent/WorldState.h"
#include "UObject/Object.h"
#include "Goal.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class UEGOAP_API UGoal : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	int32 Priority;

	virtual bool IsSatisfied(const FWorldState& WorldState) const PURE_VIRTUAL(UGoal::IsSatisfied, return false;);
};
