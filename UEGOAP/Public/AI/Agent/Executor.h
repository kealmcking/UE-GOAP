// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AI/Actions/Action.h"
#include "Executor.generated.h"

/**
 * 
 */
UCLASS()
class UEGOAP_API UExecutor : public UObject
{
	GENERATED_BODY()

	UExecutor();

public:
	void TickExecution(AAgent* Agent, float DeltaTime);
	void SetPlan(const TArray<UAction*>& NewPlan);

	bool HasActivePlan() const { return CurrentPlan.Num() > 0; }

private:

	UPROPERTY()
	TArray<UAction*> CurrentPlan;

	int32 CurrentActionIndex;

	
};
