#include "AI/Actions/Action.h"
#include "AI/Agent/Agent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"

/**
 * Override in subclasses to pick targets and prepare the action; base returns false (no setup).
 */
bool UAction::Setup(AAgent* Agent)
{
	return false;
}

/**
 * Returns true if the world state meets all of this action's preconditions.
 */
bool UAction::CanExecute(const FWorldState& WorldState) const
{
	for (const TPair<FName, int32>& Condition : Preconditions) {
		if (!WorldState.Meets(Condition.Key, Condition.Value)) {
			return false;
		}
	}
	return true;
}

/**
 * Base implementation applies all effects additively to the agent's world state; override for custom behavior.
 */
bool UAction::Execute(AAgent* Agent)
{
	if (!Agent)
		return false;

	for (const auto& Effect : Effects)
	{
		int32 Current = Agent->WorldState.GetValue(Effect.Key);
		Agent->WorldState.SetValue(Effect.Key, Current + Effect.Value);
	}

	return true;
}

/**
 * Returns true when the action has finished; override for multi-tick actions.
 */
bool UAction::IsComplete() const {
	return true;
}

/**
 * Clears movement state and target so the action can be run again in a new plan.
 */
void UAction::ResetForPlan()
{
	bIsMoving = false;
	bUseManualMovement = false;
	TargetActor = nullptr;
	AgentController = nullptr;
}

/**
 * Requests pathfinding to the target; falls back to manual movement if MoveTo fails.
 */
bool UAction::StartMoveTo(AAgent* Agent, AActor* Target)
{
    if (!Agent || !Target)
        return false;

    if (!AgentController)
        AgentController = Cast<AAIController>(Agent->GetController());
    if (!AgentController)
        return false;

    TargetActor = Target;

    UWorld* World = Agent->GetWorld();
    UNavigationSystemV1* NavSys = World ? FNavigationSystem::GetCurrent<UNavigationSystemV1>(World) : nullptr;

    FVector GoalLocation = Target->GetActorLocation();
    if (NavSys)
    {
        FNavLocation ProjectedLocation;
        const FVector QueryExtent(100.f, 100.f, 100.f);
        if (NavSys->ProjectPointToNavigation(GoalLocation, ProjectedLocation, QueryExtent))
        {
            GoalLocation = ProjectedLocation.Location;
        }
    }

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalLocation(GoalLocation);
    MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
    MoveRequest.SetUsePathfinding(true);

    FNavPathSharedPtr NavPath;
    EPathFollowingRequestResult::Type Result = AgentController->MoveTo(MoveRequest, &NavPath);

    bIsMoving = (Result == EPathFollowingRequestResult::RequestSuccessful);
    bUseManualMovement = false;

    if (!bIsMoving)
    {
        bIsMoving = true;
        bUseManualMovement = true;
    }

    return true;
}

/**
 * Returns true if the agent is within AcceptanceRadius of TargetActor.
 */
bool UAction::CheckArrival(AAgent* Agent)
{
    if (!Agent || !TargetActor)
        return false;

    float Distance = FVector::Dist(Agent->GetActorLocation(), TargetActor->GetActorLocation());
    if (Distance <= AcceptanceRadius)
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
 * Moves the agent toward TargetActor on the XY plane when pathfinding is not used.
 */
void UAction::TickManualMovement(AAgent* Agent, float DeltaTime)
{
    if (!Agent || !TargetActor || !bUseManualMovement || !bIsMoving)
        return;

    FVector ToTarget = TargetActor->GetActorLocation() - Agent->GetActorLocation();
    ToTarget.Z = 0.f;
    float Dist = ToTarget.Size();
    if (Dist < 1.f)
        return;

    ToTarget /= Dist;
    float Speed = Agent->GetCharacterMovement()->GetMaxSpeed();
    Agent->AddMovementInput(ToTarget, 1.f);
}
