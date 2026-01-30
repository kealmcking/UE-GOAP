#include "AI/Goals/Goal_StaySafe.h"

/**
 * True when ThreatNearby is at or below ThreatThreshold.
 */
bool UGoal_StaySafe::IsSatisfied(const FWorldState& WorldState) const
{
	return WorldState.GetValue("ThreatNearby") <= ThreatThreshold;
}
