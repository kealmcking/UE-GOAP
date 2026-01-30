#include "AI/Agent/Agent.h"

#include "AI/Actions/Action_Eat.h"
#include "AI/Actions/Action_Sleep.h"
#include "AI/Actions/Action_FindFood.h"
#include "AI/Goals/Goal_StayFed.h"
#include "AI/Goals/Goal_StayRested.h"
#include "AI/Goals/Goal_StaySafe.h"

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

    WorldState.SetValue("Hunger", 50);
    WorldState.SetValue("Energy", 50);
    WorldState.SetValue("ThreatNearby", 0);
    WorldState.SetValue("HasFood", 0);

    AvailableActions.Add(NewObject<UAction_Eat>(this));
    AvailableActions.Add(NewObject<UAction_Sleep>(this));
    AvailableActions.Add(NewObject<UAction_FindFood>(this));

    Goals.Add(NewObject<UGoal_StayFed>(this));
    Goals.Add(NewObject<UGoal_StayRested>(this));
    Goals.Add(NewObject<UGoal_StaySafe>(this));

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

    GetWorldTimerManager().SetTimer(PlanTimerHandle, this, &AAgent::RequestPlan, 0.5f, true);
}

/**
 * Clears the plan timer and resets debug widget state so the next session can create a new widget.
 */
void AAgent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (GetWorld())
    {
        GetWorldTimerManager().ClearTimer(PlanTimerHandle);
    }
    bGOAPDebugWidgetCreated = false;
    Super::EndPlay(EndPlayReason);
}

/**
 * Applies hunger/energy degradation (unless suppressed by current action) and ticks the executor.
 */
void AAgent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    DegradationAccumulator += DeltaTime;
    if (DegradationAccumulator >= 0.5f)
    {
        UAction* CurrentAction = Executor && Executor->HasActivePlan() ? Executor->GetCurrentAction() : nullptr;

        const bool bSuppressHungerDecay = CurrentAction && CurrentAction->SuppressesHungerDegradation();
        if (!bSuppressHungerDecay)
        {
            int32 CurrentHunger = WorldState.GetValue("Hunger");
            WorldState.SetValue("Hunger", FMath::Clamp(CurrentHunger + 1, 0, 100));
        }

        const bool bSuppressEnergyDecay = CurrentAction && CurrentAction->SuppressesEnergyDegradation();
        if (!bSuppressEnergyDecay)
        {
            int32 CurrentEnergy = WorldState.GetValue("Energy");
            WorldState.SetValue("Energy", FMath::Clamp(CurrentEnergy - 1, 0, 100));
        }

        DegradationAccumulator = 0.f;
    }

    if (Executor)
    {
        Executor->TickExecution(this, DeltaTime);
    }
}

/**
 * If no plan is active, picks the top unsatisfied goal and asks the planner for a new action sequence.
 */
void AAgent::RequestPlan()
{
    if (!Planner || !Executor || AvailableActions.Num() == 0 || Goals.Num() == 0)
        return;

    UGoal* Goal = SelectTopGoal();
    if (!Goal)
        return;

    if (Executor->HasActivePlan())
        return;

    TArray<UAction*> Plan;
    bool bSuccess = Planner->BuildPlan(WorldState, Goal, AvailableActions, Plan);

    if (bSuccess && Plan.Num() > 0)
    {
        CurrentFoodSource = nullptr;
        for (UAction* Action : Plan)
        {
            if (Action)
                Action->ResetForPlan();
        }
        for (UAction* Action : Plan)
        {
            if (!Action->Setup(this))
                return;
        }
        Executor->SetPlan(Plan);
    }
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

