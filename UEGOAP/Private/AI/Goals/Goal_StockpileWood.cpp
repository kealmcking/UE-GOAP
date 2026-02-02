// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Goals/Goal_StockpileWood.h"

bool UGoal_StockpileWood::IsSatisfied(const FWorldState& WorldState) const
{
    return WorldState.GetValue("CarriedWood") < 5;
}
