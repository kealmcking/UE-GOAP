#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Edible.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UEdible : public UInterface
{
	GENERATED_BODY()
};

class UEGOAP_API IEdible
{
	GENERATED_BODY()

public:
	/** Consumes up to Amount; returns the amount actually consumed. */
	virtual int32 Consume(int32 Amount) = 0;
	/** Returns how much can still be consumed (non-destructive). */
	virtual int32 GetAvailableAmount() const { return 0; }
};
