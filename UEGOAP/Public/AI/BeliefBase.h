#pragma once

#include "CoreMinimal.h"
#include "AI/Agent/Sensor.h"
#include "AI/Agent/BaseAgent.h"



class UEGOAP_API FBeliefBase
{
public:
	FName Name;

	TFunction<bool()> Condition = []() {return false; };
	TFunction<FVector()> ObservedLocation = []() { return FVector::ZeroVector; };

public:
	FVector GetLocation() const;
	FBeliefBase();

public:
	void Initialize(FName InName);

public:
	bool Evaluate();
};


class UEGOAP_API FBeliefBuilder {
public:

	FBeliefBase* Belief;

	FBeliefBuilder(FName Name) {
		Belief = new FBeliefBase();
		Belief->Initialize(Name);
	};

	FBeliefBuilder WithCondition(TFunction<bool()> Condition) {
		Belief->Condition = Condition;
		return *this;
	};

	FBeliefBuilder WithLocation(TFunction<FVector()> ObservedLocation) {
		Belief->ObservedLocation = ObservedLocation;
		return *this;
	};

	FBeliefBase* Build() {
		return Belief;
	};
};

class UEGOAP_API FBeliefFactory {
	AAgentBase* Agent;
	TMap<FName, FBeliefBase*> Beliefs;

public:
	FBeliefFactory(AAgentBase* Agent, TMap<FName, FBeliefBase*> InBeliefs) {
		this->Agent = Agent;
		this->Beliefs = InBeliefs;
	};

	void AddBelief(FName Key, TFunction<bool()> Condition) {
		FBeliefBuilder Builder(Key);

		Builder.WithCondition(Condition);

		FBeliefBase* NewBelief = Builder.Build();
		Beliefs.Add(Key, NewBelief);
	};

	void AddSensorBelief(FName Key, USensorComponent* Sensor) {
		FBeliefBuilder Builder(Key);
		Builder.WithCondition([Sensor]() -> bool {return Sensor && Sensor->bIsTargetInRange(); });
		Builder.WithLocation([Sensor]() {return Sensor ? Sensor->GetTargetPosition() : FVector::ZeroVector; });

		FBeliefBase* NewBelief = Builder.Build();
		Beliefs.Add(Key, NewBelief);
	};

	void AddLocationBelief(FName Key, float Dist, FTransform3d LocationCondition) {
		AddLocationBelief(Key, Dist, LocationCondition.GetLocation());
	};

	void AddLocationBelief(FName Key, float Dist, FVector LocationCondition) {
		FBeliefBuilder Builder(Key);

		Builder.WithCondition([this, LocationCondition, Dist]() { return InRangeOf(LocationCondition, Dist); });
		Builder.WithLocation([this, LocationCondition, Dist]() {return LocationCondition; });

		FBeliefBase* NewBelief = Builder.Build();
		Beliefs.Add(Key, NewBelief);
	};

	bool InRangeOf(FVector Pos, float Range) {
		return FVector::Dist(Agent->GetActorLocation(), Pos) < Range;
	};

};
