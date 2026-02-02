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

	if (FruitAmount == 0) {
		SetLifeSpan(1.0f);
	}
}

/**
 * Removes up to Amount from FruitAmount and returns the amount actually taken.
 */
int32 AAppleTreeActor::Consume(int32 Amount)
{
	if (FruitAmount <= 0) return 0;
	FruitAmount -= Amount;
	return Amount;
}

/**
 * Returns how much fruit can still be consumed (non-destructive).
 */
int32 AAppleTreeActor::GetAvailableAmount() const
{
	return FMath::Max(0, FruitAmount);
}
