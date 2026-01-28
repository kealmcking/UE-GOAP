// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Actions/Action.h"
#include "Action_Eat.generated.h"

/**
 * 
 */
UCLASS()
class UEGOAP_API UAction_Eat : public UAction
{
	GENERATED_BODY()

public:
	UAction_Eat();

	virtual bool CanExecute(const FWorldState& WorldState) const override;

	virtual bool Execute(class AAgent* Agent) override;

	virtual bool IsComplete() const override;

private:
	bool bHasEaten;
	
};
