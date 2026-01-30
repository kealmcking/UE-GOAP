#pragma once

#include "CoreMinimal.h"
#include "AI/Actor/Interfaces/Choppable.h"
#include "GameFramework/Actor.h"
#include "PineTreeActor.generated.h"

UCLASS()
class UEGOAP_API APineTreeActor : public AActor, public IChoppable
{
	GENERATED_BODY()
	
public:
	APineTreeActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

};
