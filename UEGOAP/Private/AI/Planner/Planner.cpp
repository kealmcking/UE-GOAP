#include "AI/Planner/Planner.h"
#include "AI/Planner/Node.h"

/**
 * Finds a lowest-cost sequence of actions that takes StartState to a state satisfying Goal; writes it to OutPlan.
 */
bool UPlanner::BuildPlan(const FWorldState& StartState, const UGoal* Goal, const TArray<UAction*>& Actions, TArray<UAction*>& OutPlan)
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
            OutPlan = Current.ActionPath;
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
            for (const auto& Effect : Action->Effects)
            {
                int32 NewValue;
                if (Effect.Key == "HasFood")
                {
                    NewValue = FMath::Clamp(Effect.Value, 0, 1);
                }
                else
                {
                    int32 OldValue = NewState.GetValue(Effect.Key);
                    NewValue = OldValue + Effect.Value;
                    if (Effect.Key == "Hunger" || Effect.Key == "Energy")
                        NewValue = FMath::Clamp(NewValue, 0, 100);
                    else if (Effect.Key == "ThreatNearby")
                        NewValue = FMath::Clamp(NewValue, 0, 100);
                }
                NewState.SetValue(Effect.Key, NewValue);
            }

            if (Current.ActionPath.Num() > 0 && Current.ActionPath.Last() == Action && !Action->CanExecute(NewState))
                continue;

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
