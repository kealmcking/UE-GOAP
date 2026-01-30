#include "AI/Agent/Executor.h"
#include "AI/Agent/Agent.h"

const float UExecutor::StuckTimeoutSeconds = 15.f;

UExecutor::UExecutor() : CurrentActionIndex(0)
{
}

/**
 * Replaces the current plan with a new action sequence and resets execution state.
 */
void UExecutor::SetPlan(const TArray<UAction*>& NewPlan)
{
	CurrentPlan = NewPlan;
	CurrentActionIndex = 0;
	LastLoggedActionIndex = -1;
	TimeOnCurrentAction = 0.f;
	ConsecutiveExecuteFalse = 0;
}

/**
 * Returns the action currently being executed, or nullptr if no plan or index is invalid.
 */
UAction* UExecutor::GetCurrentAction() const
{
	if (!CurrentPlan.IsValidIndex(CurrentActionIndex))
		return nullptr;
	return CurrentPlan[CurrentActionIndex];
}

/**
 * Runs the current action each tick; aborts the plan if stuck, preconditions fail, or action returns false repeatedly.
 */
void UExecutor::TickExecution(AAgent* Agent, float DeltaTime)
{
    if (!HasActivePlan() || !Agent)
        return;

    UAction* CurrentAction = CurrentPlan[CurrentActionIndex];
    if (!CurrentAction)
        return;

    if (CurrentActionIndex != LastLoggedActionIndex)
    {
        LastLoggedActionIndex = CurrentActionIndex;
        TimeOnCurrentAction = 0.f;
        ConsecutiveExecuteFalse = 0;
    }

    TimeOnCurrentAction += DeltaTime;
    if (TimeOnCurrentAction >= StuckTimeoutSeconds)
    {
        CurrentPlan.Empty();
        CurrentActionIndex = 0;
        LastLoggedActionIndex = -1;
        TimeOnCurrentAction = 0.f;
        ConsecutiveExecuteFalse = 0;
        return;
    }

    if (!CurrentAction->CanExecute(Agent->WorldState))
    {
        CurrentPlan.Empty();
        CurrentActionIndex = 0;
        LastLoggedActionIndex = -1;
        TimeOnCurrentAction = 0.f;
        ConsecutiveExecuteFalse = 0;
        return;
    }

    bool bExecuted = CurrentAction->Execute(Agent);
    if (!bExecuted)
    {
        ConsecutiveExecuteFalse++;
        if (ConsecutiveExecuteFalse >= 120)
        {
            CurrentPlan.Empty();
            CurrentActionIndex = 0;
            LastLoggedActionIndex = -1;
            TimeOnCurrentAction = 0.f;
            ConsecutiveExecuteFalse = 0;
        }
        return;
    }

    ConsecutiveExecuteFalse = 0;
    if (CurrentAction->IsComplete())
    {
        CurrentActionIndex++;
        if (CurrentActionIndex >= CurrentPlan.Num())
        {
            CurrentPlan.Empty();
            CurrentActionIndex = 0;
            LastLoggedActionIndex = -1;
            TimeOnCurrentAction = 0.f;
        }
    }
}
