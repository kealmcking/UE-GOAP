#include "AI/Planner/Planner.h"
#include "AI/Planner/Node.h"

/**
 * Applies an action's effects to a world state for planning simulation.
 * Uses consistent rules: boolean flags are set directly, resources are clamped 0-100.
 */
void UPlanner::ApplyEffects(const UAction* Action, FWorldState& State)
{
    for (const auto& Effect : Action->Effects)
    {
        int32 NewValue;
        
        if (Effect.Key == "HasFood" || Effect.Key == "Wandered" || Effect.Key == "IsResting")
        {
            NewValue = FMath::Clamp(Effect.Value, 0, 1);
        }
        else if (Effect.Key == "CarriedWood" && Effect.Value == 0)
        {
            NewValue = 0;
        }
        else if (Effect.Key == "HasFood" && Effect.Value == 0)
        {
            NewValue = 0;
        }
        else
        {
            int32 OldValue = State.GetValue(Effect.Key);
            NewValue = FMath::Clamp(OldValue + Effect.Value, 0, 100);
        }
        
        State.SetValue(Effect.Key, NewValue);
    }
}

/**
 * Finds a lowest-cost sequence of actions that takes StartState to a state satisfying Goal; writes it to OutPlan.
 * Each action in the plan is a CLONE so they have independent state.
 */
bool UPlanner::BuildPlan(const FWorldState& StartState, const UGoal* Goal, const TArray<UAction*>& Actions, TArray<UAction*>& OutPlan, UObject* ActionOuter)
{
    OutPlan.Reset();
    TArray<FNode> OpenSet;
    TSet<FWorldState> ClosedSet;

    FNode StartNode;
    StartNode.WorldState = StartState;
    StartNode.Cost = 0.f;
    OpenSet.HeapPush(StartNode);

    static const int32 MaxIterations = 10000;
    int32 Iterations = 0;

    while (OpenSet.Num() > 0 && Iterations++ < MaxIterations)
    {
        FNode Current;
        OpenSet.HeapPop(Current);

        if (Goal->IsSatisfied(Current.WorldState))
        {
            for (UAction* TemplateAction : Current.ActionPath)
            {
                UAction* ClonedAction = TemplateAction->Clone(ActionOuter);
                OutPlan.Add(ClonedAction);
            }
            return true;
        }

        if (ClosedSet.Contains(Current.WorldState))
            continue;

        ClosedSet.Add(Current.WorldState);

        for (UAction* Action : Actions)
        {
            if (!Action)
                continue;

            if (!Action->CanExecute(Current.WorldState))
                continue;

            FWorldState NewState = Current.WorldState;
            ApplyEffects(Action, NewState);

            if (Goal->IsSatisfied(NewState))
            {
                for (UAction* TemplateAction : Current.ActionPath)
                {
                    UAction* ClonedAction = TemplateAction->Clone(ActionOuter);
                    OutPlan.Add(ClonedAction);
                }
                UAction* FinalAction = Action->Clone(ActionOuter);
                OutPlan.Add(FinalAction);
                return true;
            }

            if (ClosedSet.Contains(NewState))
                continue;

            FNode NewNode;
            NewNode.WorldState = NewState;
            NewNode.Cost = Current.Cost + Action->Cost;
            NewNode.ActionPath = Current.ActionPath;
            NewNode.ActionPath.Add(Action);

            OpenSet.HeapPush(NewNode);
        }
    }

    return false;
}
