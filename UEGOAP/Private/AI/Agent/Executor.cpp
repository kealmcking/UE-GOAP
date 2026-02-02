#include "AI/Agent/Executor.h"
#include "AI/Agent/Agent.h"
#include "AIController.h"

const float UExecutor::StuckTimeoutSeconds = 15.f;

void UExecutor::AbortPlan(AAgent* Agent)
{
    if (CurrentPlan.IsValidIndex(CurrentActionIndex))
    {
        UAction* CurrentAction = CurrentPlan[CurrentActionIndex];
        if (CurrentAction)
        {
            CurrentAction->Cleanup(Agent);
        }
    }
    
    for (UAction* Action : CurrentPlan)
    {
        if (Action && Action->GetOuter() == this)
        {
            Action->MarkAsGarbage();
        }
    }
    
    CurrentPlan.Empty();
    CurrentActionIndex = 0;
    LastLoggedActionIndex = -1;
    TimeOnCurrentAction = 0.f;
    ConsecutiveExecuteFalse = 0;

    if (Agent)
    {
        if (AAIController* AIC = Cast<AAIController>(Agent->GetController()))
        {
            AIC->StopMovement();
        }
    }
}

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
	LastLoggedActionIndex = 0;
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

        CurrentAction->ResetForPlan();
        if (!CurrentAction->Setup(Agent)) {
            AbortPlan(Agent);
            return;
        }
    }

    TimeOnCurrentAction += DeltaTime;
    if (TimeOnCurrentAction >= StuckTimeoutSeconds)
    {
        AbortPlan(Agent);
        return;
    }

    if (!CurrentAction->CanExecute(Agent->WorldState))
    {
        AbortPlan(Agent);
        return;
    }

    bool bExecuted = CurrentAction->Execute(Agent);
    if (!bExecuted)
    {
        ConsecutiveExecuteFalse++;
        if (ConsecutiveExecuteFalse >= 120)
        {
            AbortPlan(Agent);
        }
        return;
    }

    ConsecutiveExecuteFalse = 0;
    if (CurrentAction->IsComplete())
    {
        CurrentActionIndex++;
        if (CurrentActionIndex >= CurrentPlan.Num())
        {
            for (UAction* Action : CurrentPlan)
            {
                if (Action && Action->GetOuter() == this)
                {
                    Action->MarkAsGarbage();
                }
            }
            CurrentPlan.Empty();
            CurrentActionIndex = 0;
            LastLoggedActionIndex = -1;
            TimeOnCurrentAction = 0.f;
            return;
        }
        
        LastLoggedActionIndex = CurrentActionIndex;
        TimeOnCurrentAction = 0.f;
        ConsecutiveExecuteFalse = 0;
        
        UAction* NextAction = CurrentPlan[CurrentActionIndex];
        NextAction->ResetForPlan();
        if (!NextAction->Setup(Agent))
        {
            AbortPlan(Agent);
            return;
        }
    }
}
