#include "AI/Actions/Action_Wander.h"
#include "AI/Agent/Agent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

UAction_Wander::UAction_Wander()
{
	Cost = 2.0f;
	Effects.Add("Wandered", 1);
}

bool UAction_Wander::Setup(AAgent* Agent)
{
	if (!Agent || !Agent->GetWorld())
		return false;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Agent->GetWorld());
	if (!NavSys)
		return false;

	const FVector Origin = Agent->GetActorLocation();
	FNavLocation RandomLocation;

	float Radius = 500.f;
	for (int32 Try = 0; Try < 5; Try++)
	{
		if (NavSys->GetRandomReachablePointInRadius(Origin, Radius, RandomLocation))
		{
			WanderLocation = RandomLocation.Location;
			return true;
		}
		Radius += 500.f;
	}

	FNavLocation Projected;
	FVector Dir(FMath::RandRange(-1.f, 1.f), FMath::RandRange(-1.f, 1.f), 0.f);
	if (Dir.SizeSquared() < 0.01f)
		Dir = FVector(1.f, 0.f, 0.f);
	else
		Dir.Normalize();
	const FVector FallbackTarget = Origin + Dir * 400.f;
	if (NavSys->ProjectPointToNavigation(FallbackTarget, Projected))
	{
		WanderLocation = Projected.Location;
		return true;
	}

	WanderLocation = Origin;
	return true;
}

bool UAction_Wander::Execute(AAgent* Agent)
{
	if (!Agent)
		return false;

	AAIController* Controller = Cast<AAIController>(Agent->GetController());
	if (!Controller)
		return false;

	if (!bIsMoving)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalLocation(WanderLocation);
		MoveRequest.SetAcceptanceRadius(100.f);
		Controller->MoveTo(MoveRequest);
		bIsMoving = true;
		return true;
	}

	UPathFollowingComponent* PFC = Controller->GetPathFollowingComponent();
	if (PFC && PFC->GetStatus() == EPathFollowingStatus::Idle)
	{
		bReachedDestination = true;
		bIsMoving = false;
		if (Agent)
			Agent->WorldState.SetValue("Wandered", 1);
		return true;
	}
	return true;
}

bool UAction_Wander::IsComplete() const
{
	return bReachedDestination;
}

void UAction_Wander::ResetForPlan()
{
	UAction::ResetForPlan();
	bReachedDestination = false;
}

UAction* UAction_Wander::Clone(UObject* Outer) const
{
	UAction_Wander* NewAction = NewObject<UAction_Wander>(Outer, GetClass());
	NewAction->AcceptanceRadius = AcceptanceRadius;
	NewAction->ConsumeAmount = ConsumeAmount;
	NewAction->Cost = Cost;
	NewAction->Preconditions = Preconditions;
	NewAction->Effects = Effects;
	return NewAction;
}
