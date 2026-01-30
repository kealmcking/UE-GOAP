#include "AI/Actions/Action_FindFood.h"
#include "AI/Actor/Interfaces/Edible.h"
#include "AI/Agent/Agent.h"
#include "EngineUtils.h"

/**
 * Requires Hunger >= 30 and HasFood == 0; effect sets HasFood to 1 on arrival at food.
 */
UAction_FindFood::UAction_FindFood() : bHasFoundFood(false)
{
	Cost = 1.0f;

	AcceptanceRadius = 250.f;
	Preconditions.Add("Hunger", 30);
	Preconditions.Add("HasFood", 0);
	Effects.Add("HasFood", 1);
}

/**
 * True when the agent is hungry enough and does not already have food.
 */
bool UAction_FindFood::CanExecute(const FWorldState& WorldState) const
{
	return UAction::CanExecute(WorldState);
}

/**
 * Moves to the chosen food source; on arrival sets CurrentFoodSource and HasFood so Eat can consume.
 */
bool UAction_FindFood::Execute(AAgent* Agent)
{
	if (!Agent || !TargetActor)
		return false;

	if (!AgentController)
	{
		AgentController = Cast<AAIController>(Agent->GetController());
		if (!AgentController)
			return false;
	}

	if (!bIsMoving)
	{
		if (!StartMoveTo(Agent, TargetActor))
			return false;
		return true;
	}

	if (bUseManualMovement)
	{
		float DeltaTime = Agent->GetWorld() ? Agent->GetWorld()->GetDeltaSeconds() : 0.016f;
		TickManualMovement(Agent, DeltaTime);
	}

	if (CheckArrival(Agent))
	{
		if (IEdible* Edible = Cast<IEdible>(TargetActor))
		{
			if (Edible->GetAvailableAmount() < 1)
			{
				Agent->LastDepletedFoodSource = TargetActor;
				return false;
			}
		}
		Agent->CurrentFoodSource = TargetActor;
		Agent->WorldState.SetValue("HasFood", 1);
		bHasFoundFood = true;
		return true;
	}
	return true;
}

/**
 * True once the agent has arrived at food and HasFood was set.
 */
bool UAction_FindFood::IsComplete() const
{
	return bHasFoundFood;
}

/**
 * Uses horizontal distance only so tall actors (e.g. trees) do not block arrival.
 */
bool UAction_FindFood::CheckArrival(AAgent* Agent)
{
	if (!Agent || !TargetActor)
		return false;

	FVector AgentXY = Agent->GetActorLocation();
	FVector TargetXY = TargetActor->GetActorLocation();
	AgentXY.Z = 0.f;
	TargetXY.Z = 0.f;
	float Dist2D = FVector::Dist(AgentXY, TargetXY);
	if (Dist2D <= AcceptanceRadius)
	{
		if (AgentController)
			AgentController->StopMovement();
		bIsMoving = false;
		bUseManualMovement = false;
		return true;
	}
	return false;
}

/**
 * Picks the closest edible actor with food; skips LastDepletedFoodSource so the agent tries another source.
 */
bool UAction_FindFood::Setup(AAgent* Agent)
{
	if (!Agent) return false;

	AgentController = Cast<AAIController>(Agent->GetController());
	if (!AgentController) return false;

	AActor* Closest = nullptr;
	float MinDistance = TNumericLimits<float>::Max();

	UWorld* World = Agent->GetWorld();
	if (!World) return false;

	for (TActorIterator<AActor> It(Agent->GetWorld()); It; ++It) {
		if (!It->GetClass()->ImplementsInterface(UEdible::StaticClass()))
			continue;
		if (Agent->LastDepletedFoodSource && *It == Agent->LastDepletedFoodSource)
			continue;
		if (IEdible* Edible = Cast<IEdible>(*It))
		{
			if (Edible->GetAvailableAmount() < 1)
				continue;
		}
		float Dist = FVector::Dist(Agent->GetActorLocation(), It->GetActorLocation());
		if (Dist < MinDistance) {
			MinDistance = Dist;
			Closest = *It;
		}
	}

	if (Closest) {
		TargetActor = Closest;
		return true;
	}

	if (Agent->LastDepletedFoodSource)
		Agent->LastDepletedFoodSource = nullptr;
	return false;
}

/**
 * Clears found-food state so the action can run again in a new plan.
 */
void UAction_FindFood::ResetForPlan()
{
	UAction::ResetForPlan();
	bHasFoundFood = false;
}
