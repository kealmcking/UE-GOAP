#include "AI/Actor/BedActor.h"

ABedActor::ABedActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABedActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/**
 * Called each rest tick by Action_Sleep; return value can drive visuals (e.g. sound).
 */
bool ABedActor::Rest(int32 Amount)
{
	return true;
}

/**
 * Reserves this bed for the agent if no one is occupying it.
 */
bool ABedActor::TryReserve(AAgent* Agent)
{
	if (!OccupyingAgent) {
		OccupyingAgent = Agent;
		return true;
	}
	return false;
}

/**
 * Clears occupancy when Agent matches or when Agent is nullptr (e.g. plan reset).
 */
void ABedActor::Release(AAgent* Agent)
{
	if (Agent == nullptr || OccupyingAgent == Agent)
		OccupyingAgent = nullptr;
}

/**
 * True when no agent is currently occupying this bed.
 */
bool ABedActor::IsAvailable() const {
	return OccupyingAgent == nullptr;
}

