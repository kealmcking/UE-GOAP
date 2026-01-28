// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Agent/Executor.h"
#include "AI/Agent/Agent.h"

UExecutor::UExecutor() : CurrentActionIndex(0)
{
}

void UExecutor::SetPlan(const TArray<UAction*>& NewPlan)
{
	CurrentPlan = NewPlan;
	CurrentActionIndex = 0;
}

void UExecutor::TickExecution(AAgent* Agent, float DeltaTime)
{
	if (!HasActivePlan())
		return;

	UAction* CurrentAction = CurrentPlan[CurrentActionIndex];
	if (!CurrentAction)
		return;

	if (!CurrentAction->CanExecute(Agent->WorldState)) {
		CurrentPlan.Empty();
		CurrentActionIndex = 0;
		return;
	}

	if (CurrentAction->Execute(Agent)) {
		if (CurrentAction->IsComplete()) {
			CurrentActionIndex++;
			if (CurrentActionIndex >= CurrentPlan.Num()) {
				CurrentPlan.Empty();
				CurrentActionIndex = 0;
			}
		}
	}
}
