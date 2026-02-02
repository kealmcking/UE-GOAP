// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Actions/Action.h"
#include "Action_Wander.generated.h"

/**
 * 
 */
UCLASS()
class UEGOAP_API UAction_Wander : public UAction
{
	GENERATED_BODY()

public:
	UAction_Wander();

	virtual bool Setup(AAgent* Agent) override;
	virtual bool Execute(AAgent* Agent) override;
	virtual bool IsComplete() const override;
	virtual void ResetForPlan() override;
	virtual UAction* Clone(UObject* Outer) const override;

private:
	FVector WanderLocation;
	bool bReachedDestination = false;
	
};
