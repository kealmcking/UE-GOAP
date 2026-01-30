#include "AI/Actions/Action_Eat.h"
#include "EngineUtils.h"
#include "AI/Actor/Interfaces/Edible.h"
#include "AI/Agent/Agent.h"

/**
 * Sets preconditions (HasFood) and effects (reduce Hunger, clear HasFood).
 */
UAction_Eat::UAction_Eat() : bHasEaten(false)
{
	Cost = 1.0f;

	Preconditions.Add("HasFood", 1);
	Effects.Add("Hunger", -40);
	Effects.Add("HasFood", 0);
}

/**
 * True when the agent has food (set by FindFood).
 */
bool UAction_Eat::CanExecute(const FWorldState& WorldState) const
{
	return UAction::CanExecute(WorldState);
}

/**
 * Consumes from the agent's CurrentFoodSource (set by FindFood); reduces hunger by amount consumed and clears HasFood.
 */
bool UAction_Eat::Execute(AAgent* Agent)
{
	if (!Agent)
		return false;

	AActor* FoodTarget = Agent->CurrentFoodSource ? Agent->CurrentFoodSource : TargetActor;
	if (!FoodTarget)
		return false;

	IEdible* Edible = Cast<IEdible>(FoodTarget);
	if (!Edible)
		return false;

	int32 Consumed = Edible->Consume(40);
	if (Consumed > 0)
	{
		Agent->WorldState.SetValue("Hunger", FMath::Clamp(Agent->WorldState.GetValue("Hunger") - Consumed, 0, 100));
		Agent->WorldState.SetValue("HasFood", 0);
		Agent->LastDepletedFoodSource = nullptr;
		Agent->CurrentFoodSource = nullptr;
		bHasEaten = true;
		return true;
	}

	Agent->LastDepletedFoodSource = FoodTarget;
	Agent->CurrentFoodSource = nullptr;
	Agent->WorldState.SetValue("HasFood", 0);
	return false;
}

/**
 * True once food was successfully consumed this plan.
 */
bool UAction_Eat::IsComplete() const
{
	return bHasEaten;
}

/**
 * No target needed here; Eat uses Agent->CurrentFoodSource set when FindFood completes.
 */
bool UAction_Eat::Setup(AAgent* Agent)
{
	if (!Agent || !Agent->GetWorld())
		return false;
	TargetActor = nullptr;
	return true;
}

/**
 * Clears eaten state so the action can run again in a new plan.
 */
void UAction_Eat::ResetForPlan()
{
	UAction::ResetForPlan();
	bHasEaten = false;
}
