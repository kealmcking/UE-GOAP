#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Edible.h"
#include "GameFramework/Actor.h"
#include "AppleTreeActor.generated.h"

UCLASS()
class UEGOAP_API AAppleTreeActor : public AActor, public IEdible
{
	GENERATED_BODY()
	
public:
	AAppleTreeActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FruitAmount = 10;

	virtual int32 Consume(int32 Amount) override;
	virtual int32 GetAvailableAmount() const override;
};
