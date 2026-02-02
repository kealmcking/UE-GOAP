#include "AI/Agent/Agent.h"

#include "AI/Actor/Interfaces/Edible.h"
#include "AI/Actor/Interfaces/Choppable.h"

#include "AI/Actions/Action_Eat.h"
#include "AI/Actions/Action_Sleep.h"
#include "AI/Actions/Action_FindFood.h"
#include "AI/Actions/Action_Wander.h"
#include "AI/Actions/Action_GatherWood.h"
#include "AI/Actions/Action_StockWood.h"

#include "AI/Goals/Goal_StayFed.h"
#include "AI/Goals/Goal_StockpileWood.h"
#include "AI/Goals/Goal_GatherWood.h"
#include "AI/Goals/Goal_StayRested.h"
#include "AI/Goals/Goal_StaySafe.h"
#include "AI/Goals/Goal_Idle.h"

#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

namespace
{
	bool bGOAPDebugWidgetCreated = false;
}

/**
 * Constructs the agent with planner and executor subobjects.
 */
AAgent::AAgent()
{
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AAIController::StaticClass();

	Planner = CreateDefaultSubobject<UPlanner>(TEXT("Planner"));
	Executor = CreateDefaultSubobject<UExecutor>(TEXT("Executor"));
}

/**
 * Initializes world state, actions, goals, optional debug widget, and starts the plan-request timer.
 */
void AAgent::BeginPlay()
{
    Super::BeginPlay();

    if (!GetWorld())
        return;

    if (!Planner)
        Planner = NewObject<UPlanner>(this);
    if (!Executor)
        Executor = NewObject<UExecutor>(this);

    WorldState.SetValue("Hunger", 50);
    WorldState.SetValue("Energy", 50);
    WorldState.SetValue("HasFood", 0);
    WorldState.SetValue("Wandered", 0);
    WorldState.SetValue("CarriedWood", 0);
    WorldState.SetValue("WoodStock", 0);
    WorldState.SetValue("IsResting", 0);

    AvailableActions.Add(NewObject<UAction_Eat>(this));
    AvailableActions.Add(NewObject<UAction_Sleep>(this));
    AvailableActions.Add(NewObject<UAction_FindFood>(this));
    AvailableActions.Add(NewObject<UAction_Wander>(this));
    AvailableActions.Add(NewObject<UAction_GatherWood>(this));
    AvailableActions.Add(NewObject<UAction_StockWood>(this));

    Goals.Add(NewObject<UGoal_StayFed>(this));
    Goals.Add(NewObject<UGoal_StayRested>(this));
    Goals.Add(NewObject<UGoal_Idle>(this));
    Goals.Add(NewObject<UGoal_GatherWood>(this));
    Goals.Add(NewObject<UGoal_StockpileWood>(this));

    if (IsLocallyControlled() && DebugWidgetClass && !bGOAPDebugWidgetCreated)
    {
        UGOAPDebugWidget* Widget = CreateWidget<UGOAPDebugWidget>(GetWorld(), DebugWidgetClass);
        if (Widget)
        {
            Widget->AddToViewport();
            Widget->InitializeWidget(this);
            bGOAPDebugWidgetCreated = true;
        }
    }

    RequestPlan();
}

/**
 * Resets debug widget state so the next session can create a new widget.
 */
void AAgent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    bGOAPDebugWidgetCreated = false;
    Super::EndPlay(EndPlayReason);
}

/**
 * Applies hunger/energy degradation (unless suppressed by current action) and ticks the executor.
 */
void AAgent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC && AIC->GetPathFollowingComponent() && AIC->GetPathFollowingComponent()->HasValidPath())
    {
        FVector Dir = AIC->GetPathFollowingComponent()->GetCurrentDirection();
        Dir.Z = 0.f;
        if (Dir.SizeSquared() > 0.01f)
        {
            Dir.Normalize();
            AddMovementInput(Dir, 1.f);
        }
    }

    UAction* CurrentActionBeforeTick = Executor && Executor->HasActivePlan() ? Executor->GetCurrentAction() : nullptr;

    if (Executor)
    {
        Executor->TickExecution(this, DeltaTime);
        if (!Executor->HasActivePlan())
            RequestPlan();
    }

    HungerAcummulationAccumulator += DeltaTime;
    if (HungerAcummulationAccumulator >= HungerAccumulationIntervalSeconds)
    {
        const bool bSuppressHungerDecay = CurrentActionBeforeTick && CurrentActionBeforeTick->SuppressesHungerDegradation();
        if (!bSuppressHungerDecay)
        {
            int32 CurrentHunger = WorldState.GetValue("Hunger");
            WorldState.SetValue("Hunger", FMath::Clamp(CurrentHunger + 1, 0, 100));
        }
        HungerAcummulationAccumulator = 0.f;
    }

    EnergyDegradationAccumulator += DeltaTime;
    if (EnergyDegradationAccumulator >= EnergyDegradationIntervalSeconds)
    {
        const bool bSuppressEnergyDecay = CurrentActionBeforeTick && CurrentActionBeforeTick->SuppressesEnergyDegradation();
        if (!bSuppressEnergyDecay)
        {
            int32 CurrentEnergy = WorldState.GetValue("Energy");
            WorldState.SetValue("Energy", FMath::Clamp(CurrentEnergy - 1, 0, 100));
        }
        EnergyDegradationAccumulator = 0.f;
    }
    
    SourceCleanupAccumulator += DeltaTime;
    if (SourceCleanupAccumulator >= 5.f)
    {
        ClearRegeneratedSources();
        SourceCleanupAccumulator = 0.f;
    }
}

/**
 * If no plan is active, picks the top unsatisfied goal and asks the planner for a new action sequence.
 */
void AAgent::RequestPlan()
{
    if (!Planner || !Executor || AvailableActions.Num() == 0 || Goals.Num() == 0)
        return;

    if (Executor->HasActivePlan())
        return;

    TArray<UGoal*> UnsatisfiedGoals;
    for (UGoal* Goal : Goals)
    {
        if (!Goal)
            continue;
        if (!Goal->IsSatisfied(WorldState))
            UnsatisfiedGoals.Add(Goal);
    }

    if (UnsatisfiedGoals.Num() == 0)
    {
        WorldState.SetValue("Wandered", 0);
        for (UGoal* Goal : Goals)
        {
            if (!Goal)
                continue;
            if (!Goal->IsSatisfied(WorldState))
                UnsatisfiedGoals.Add(Goal);
        }
    }

    if (UnsatisfiedGoals.Num() == 0)
        return;

    UnsatisfiedGoals.Sort([](const UGoal& A, const UGoal& B) { return A.Priority > B.Priority; });

    for (UGoal* Goal : UnsatisfiedGoals)
    {
        TArray<UAction*> Plan;
        bool bSuccess = Planner->BuildPlan(WorldState, Goal, AvailableActions, Plan, Executor);

        if (!bSuccess || Plan.Num() == 0)
            continue;

        CurrentFoodSource = nullptr;
        
        if (!Plan.IsValidIndex(0) || !Plan[0] || !Plan[0]->Setup(this))
        {
            for (UAction* Action : Plan)
            {
                if (Action)
                    Action->MarkAsGarbage();
            }
            continue;
        }

        CurrentGoal = Goal;
        Executor->SetPlan(Plan);
        return;
    }

    CurrentGoal = nullptr;
    WorldState.SetValue("Wandered", 0);
}

/**
 * Returns the highest-priority goal that is not yet satisfied by the current world state.
 */
UGoal* AAgent::SelectTopGoal() const
{
	UGoal* BestGoal = nullptr;

	for (UGoal* Goal : Goals)
	{
		if (!Goal)
			continue;
		if (Goal->IsSatisfied(WorldState))
			continue;
		if (!BestGoal || Goal->Priority > BestGoal->Priority)
			BestGoal = Goal;
	}
	return BestGoal;
}

/**
 * Clears depleted source tracking for sources that have regenerated or been destroyed.
 */
void AAgent::ClearRegeneratedSources()
{
	if (LastDepletedFoodSource)
	{
		if (!IsValid(LastDepletedFoodSource))
		{
			LastDepletedFoodSource = nullptr;
		}
		else if (IEdible* Edible = Cast<IEdible>(LastDepletedFoodSource))
		{
			if (Edible->GetAvailableAmount() > 0)
			{
				LastDepletedFoodSource = nullptr;
			}
		}
	}
	
	if (LastDepletedWoodSource)
	{
		if (!IsValid(LastDepletedWoodSource))
		{
			LastDepletedWoodSource = nullptr;
		}
		else if (IChoppable* Choppable = Cast<IChoppable>(LastDepletedWoodSource))
		{
			if (Choppable->GetAvailableAmount() > 0)
			{
				LastDepletedWoodSource = nullptr;
			}
		}
	}
}

