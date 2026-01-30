#include "AI/Goals/Goal_StayFed.h"

/**
 * True when Hunger is at or below HungerThreshold.
 */
bool UGoal_StayFed::IsSatisfied(const FWorldState& WorldState) const
{
	return WorldState.GetValue("Hunger") <= HungerThreshold;
}

