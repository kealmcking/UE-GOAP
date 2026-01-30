# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

_Nothing yet._

## [0.1.0] - 2025-01-29

### Added

- **GOAP core**
  - `FWorldState`: key/value int state with `GetValue`, `SetValue`, `Meets` (single and map).
  - `UGoal` (abstract) with `Priority` and `IsSatisfied`; `UGoal_StayFed`, `UGoal_StayRested`, `UGoal_StaySafe`.
  - `UAction` (abstract): `Preconditions`, `Effects`, `Setup`, `CanExecute`, `Execute`, `IsComplete`, `ResetForPlan`, movement helpers (`StartMoveTo`, `CheckArrival`, `TickManualMovement`), optional manual movement when pathfinding fails.
  - `UPlanner::BuildPlan`: A*-style search from start state to a state satisfying the goal; `HasFood` uses replacement semantics so plans like Eat→FindFood→Eat are valid.
  - `UExecutor`: `SetPlan`, `TickExecution`, `GetCurrentAction`; aborts plan on stuck timeout, precondition failure, or repeated `Execute` false.
- **Agent**
  - `AAgent`: `WorldState`, `AvailableActions`, `Goals`, `Planner`, `Executor`; periodic `RequestPlan`, `SelectTopGoal`; hunger/energy degradation every 0.5s with optional suppression per action.
  - `CurrentFoodSource` (set by FindFood, used by Eat) and `LastDepletedFoodSource` (skipped when finding food).
- **Actions**
  - `UAction_FindFood`: move to closest edible with food; skip last depleted source; horizontal arrival check; set `HasFood` and `CurrentFoodSource` on arrival.
  - `UAction_Eat`: consume from `CurrentFoodSource`; dynamic consume amount; reduce hunger by amount taken; clear `HasFood`; `SuppressesHungerDegradation`.
  - `UAction_Sleep`: optional move to bed with reserve/release; timed energy restore (at bed or in place if movement gives up); `GiveUpOnBedAfterSeconds`; `SuppressesEnergyDegradation`.
- **Interfaces and actors**
  - `IEdible`: `Consume(Amount)` returns amount taken, `GetAvailableAmount`.
  - `IRestable`: `Rest`, `TryReserve`, `Release`, `IsAvailable`.
  - `IChoppable` (stub).
  - `AAppleTreeActor`: `FruitAmount`, `Consume`, `GetAvailableAmount`.
  - `ABedActor`: `Rest`, `TryReserve`, `Release`, `IsAvailable`, `OccupyingAgent`.
  - `APineTreeActor`, `ACampfireActor` (base actors).
- **Debug**
  - `UGOAPDebugWidget`: optional `ConsolidatedDebugText` (single block) or separate WorldState/Goal/Plan/Action text; one widget per session; `InitializeWidget(AAgent*)`.

[0.1.0]: https://github.com/kealmcking/UE-GOAP
