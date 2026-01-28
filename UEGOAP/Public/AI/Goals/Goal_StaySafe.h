// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Goals/Goal.h"
#include "Goal_StaySafe.generated.h"

/**
 * 
 */
UCLASS()
class UEGOAP_API UGoal_StaySafe : public UGoal
{
	GENERATED_BODY()

public:
	UGoal_StaySafe() {
		Priority = 10;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal")
	int32 ThreatThreshold = 40;

	virtual bool IsSatisfied(const FWorldState& WorldState) const override;
	
};
