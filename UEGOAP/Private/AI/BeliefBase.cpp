// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BeliefBase.h"

FBeliefBase::FBeliefBase()
{
}

bool FBeliefBase::Evaluate()
{
	return Condition ? Condition() : false;
}

void FBeliefBase::Initialize(FName InName) {
	Name = InName;
}
