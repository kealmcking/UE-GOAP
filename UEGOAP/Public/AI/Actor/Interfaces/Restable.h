#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Restable.generated.h"

class AAgent;

UINTERFACE(MinimalAPI, Blueprintable)
class URestable : public UInterface
{
	GENERATED_BODY()
};

class UEGOAP_API IRestable
{
	GENERATED_BODY()

public:
	virtual bool Rest(int32 Amount) = 0;

	/** Reserves this rest spot for the agent; returns false if already occupied. */
	virtual bool TryReserve(AAgent* Agent) = 0;
	/** Clears occupancy for the given agent (or any if Agent is nullptr). */
	virtual void Release(AAgent* Agent) = 0;
	/** True when no agent is currently using this rest spot. */
	virtual bool IsAvailable() const = 0;
};
