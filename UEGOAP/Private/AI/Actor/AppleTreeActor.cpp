#include "AI/Actor/AppleTreeActor.h"

AAppleTreeActor::AAppleTreeActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAppleTreeActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAppleTreeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/**
 * Removes up to Amount from FruitAmount and returns the amount actually taken.
 */
int32 AAppleTreeActor::Consume(int32 Amount)
{
	if (FruitAmount <= 0) return 0;
	int32 Taken = FMath::Min(Amount, FruitAmount);
	FruitAmount -= Taken;
	return Taken;
}

/**
 * Returns how much fruit can still be consumed (non-destructive).
 */
int32 AAppleTreeActor::GetAvailableAmount() const
{
	return FMath::Max(0, FruitAmount);
}
