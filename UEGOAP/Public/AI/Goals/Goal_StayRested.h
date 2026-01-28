// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Goals/Goal.h"
#include "Goal_StayRested.generated.h"

/**
 * 
 */
UCLASS()
class UEGOAP_API UGoal_StayRested : public UGoal
{
	GENERATED_BODY()

public:
	UGoal_StayRested() {
		Priority = 10;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal")
	int32 EnergyThreshold = 40;

	virtual bool IsSatisfied(const FWorldState& WorldState) const override;
	
};
