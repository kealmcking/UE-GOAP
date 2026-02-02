// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Actions/Action.h"
#include "Action_GatherWood.generated.h"

/**
 * 
 */
UCLASS()
class UEGOAP_API UAction_GatherWood : public UAction
{
	GENERATED_BODY()

public:
	UAction_GatherWood();

	virtual bool CanExecute(const FWorldState& WorldState) const override;

	virtual bool Execute(class AAgent* Agent) override;

	virtual bool IsComplete() const override;

	virtual bool Setup(class AAgent* Agent) override;

	virtual bool CheckArrival(class AAgent* Agent) override;

	virtual void ResetForPlan() override;
	
	virtual UAction* Clone(UObject* Outer) const override;

	UPROPERTY(EditDefaultsOnly, Category = "GatherWood")
	float GatherWoodDuration = 2.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "GatherWood")
	int32 TargetWoodAmount = 5;

private:
	bool bHasFoundWood = false;
	bool bHasArrived = false;
	float TimeSpentGatheringWood = 0.f;
};
