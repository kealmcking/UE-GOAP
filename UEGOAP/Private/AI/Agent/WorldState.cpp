#include "AI/Agent/WorldState.h"

/**
 * Returns the stored value for Key, or 0 if not set.
 */
int32 FWorldState::GetValue(FName Key) const 
{
	const int32* Found = Values.Find(Key);
	return Found ? *Found : 0;
}

/**
 * Stores or overwrites the value for Key.
 */
void FWorldState::SetValue(FName Key, int32 Value)
{
	Values.Add(Key, Value);
}

/**
 * Returns true if the stored value for Key is >= Value (or Key is unset and Value <= 0).
 */
bool FWorldState::Meets(FName Key, int32 Value) const
{
	const int32* Found = Values.Find(Key);
	return Found && *Found >= Value;
}

/**
 * Returns true if this state meets every key/value condition in Conditions.
 */
bool FWorldState::Meets(const TMap<FName, int32>& Conditions) const {
    for (const auto& Condition : Conditions)
    {
        if (!Meets(Condition.Key, Condition.Value))
            return false;
    }
    return true;
}
