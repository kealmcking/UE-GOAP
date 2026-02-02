#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Choppable.generated.h"

UINTERFACE(MinimalAPI)
class UChoppable : public UInterface
{
	GENERATED_BODY()
};

class UEGOAP_API IChoppable
{
	GENERATED_BODY()

public:

	virtual int32 Gather(int32 Amount) = 0;

	virtual int32 GetAvailableAmount() const { return 0; }
};
