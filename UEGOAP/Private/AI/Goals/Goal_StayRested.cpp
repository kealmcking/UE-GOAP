#include "AI/Goals/Goal_StayRested.h"

/**
 * True when Energy meets or exceeds EnergyThreshold.
 */
bool UGoal_StayRested::IsSatisfied(const FWorldState& WorldState) const
{
	return WorldState.Meets("Energy", EnergyThreshold);
}
