#pragma once

#include "CoreMinimal.h"
#include "WorldState.generated.h"

USTRUCT(BlueprintType)
struct FWorldState
{
    GENERATED_BODY()

public:
    TMap<FName, int32> Values;

    /** Returns the stored value for Key, or 0 if not set. */
    int32 GetValue(FName Key) const;

    /** Stores or overwrites the value for Key. */
    void SetValue(FName Key, int32 Value);

    /** True if the stored value for Key is >= Value. */
    bool Meets(FName Key, int32 Value) const;

    /** True if this state meets every key/value condition in Conditions. */
    bool Meets(const TMap<FName, int32>& Conditions) const;

    bool operator==(const FWorldState& Other) const
    {
        return Values.OrderIndependentCompareEqual(Other.Values);
    }

    friend uint32 GetTypeHash(const FWorldState& State)
    {
        uint32 Hash = 0;
        TArray<FName> Keys;
        State.Values.GetKeys(Keys);
        Keys.Sort([](const FName& A, const FName& B) { return A.Compare(B) < 0; });
        for (const FName& Key : Keys)
        {
            if (const int32* Val = State.Values.Find(Key))
            {
                Hash = HashCombine(Hash, GetTypeHash(Key));
                Hash = HashCombine(Hash, GetTypeHash(*Val));
            }
        }
        return Hash;
    }
};
