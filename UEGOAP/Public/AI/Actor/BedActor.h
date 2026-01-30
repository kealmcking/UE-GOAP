#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Restable.h"
#include "GameFramework/Actor.h"
#include "AI/Agent/Agent.h"
#include "BedActor.generated.h"

UCLASS()
class UEGOAP_API ABedActor : public AActor, public IRestable
{
	GENERATED_BODY()
	
public:
	ABedActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rest")
	int32 RestRestore = 2;

	virtual bool Rest(int32 Amount) override;

	UPROPERTY()
	AAgent* OccupyingAgent = nullptr;

	virtual bool TryReserve(AAgent* Agent) override;
	virtual void Release(AAgent* Agent) override;


	virtual bool IsAvailable() const override;
};
