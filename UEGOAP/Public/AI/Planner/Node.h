// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Agent/WorldState.h"
#include "AI/Actions/Action.h"

struct FNode
{
	FWorldState WorldState;
	TArray<UAction*> ActionPath;
	float Cost;
};
