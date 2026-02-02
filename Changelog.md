# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.5] - 2025-02-02

### Added

- **Action cloning system**
  - `UAction::Clone(UObject* Outer)`: virtual method to create independent action copies for plans.
  - `UAction::Cleanup(AAgent*)`: virtual method to release resources (beds, reservations) on plan abort.
  - Each action in a plan is now a clone with independent state, preventing shared state corruption.
- **World state variable**
  - `IsResting`: tracks whether agent is at bed, used by `Action_Sleep::CanExecute` instead of instance state.
- **Agent improvements**
  - `ClearRegeneratedSources()`: periodically clears `LastDepletedFoodSource`/`LastDepletedWoodSource` when sources regenerate or are destroyed.
- **Planner improvements**
  - `UPlanner::ApplyEffects()`: helper method for consistent effect application (boolean flags set directly, resources clamped 0-100).
  - `BuildPlan` now accepts `ActionOuter` parameter and clones actions for the output plan.

### Changed

- **Executor**
  - `AbortPlan` now calls `Cleanup()` on current action before clearing plan.
  - `LastLoggedActionIndex` updated when advancing actions, preventing double `ResetForPlan`/`Setup` calls.
  - Cloned actions are garbage collected when plan completes or aborts.
- **Action_Sleep**
  - `CanExecute` now uses `WorldState.GetValue("IsResting")` instead of instance `bIsAtBed` for deterministic planning.
  - Sets `IsResting=1` when arriving at bed, clears on sleep complete or cleanup.
- **Action_GatherWood**
  - Now continuously gathers wood (1 per `GatherWoodDuration`) until `CarriedWood >= TargetWoodAmount`.
  - Added `TargetWoodAmount` property (default 5).
  - Added `bHasArrived` flag to prevent movement loop when already at tree.
  - Effect changed from +1 to +5 to match actual behavior.
- **Action_StockWood**
  - `ResetForPlan` now resets `AmountToStock`.
- **WoodStockpileActor**
  - `Stock()` now returns amount actually added (clamped to available space) instead of new total.
- **Goal_StockpileWood**
  - `IsSatisfied` now returns true when `CarriedWood < 5` (was `== 0`), preventing premature stocking.

### Removed

- Timer-based `RequestPlan` calls; Tick-based calling is sufficient and more responsive.
- `PlanTimerHandle` from Agent.

### Fixed

- Actions no longer share state when same action appears multiple times in a plan.
- Bed reservations properly released when sleep plan is aborted mid-execution.
- Agent no longer gets stuck trying to start movement when already at destination.
- Wood gathering no longer stops after first cycle due to movement state issues.
- WoodStock no longer inflates incorrectly when stocking wood.

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
  - `IChoppable` (No behaviour implemented yet).
  - `AAppleTreeActor`: `FruitAmount`, `Consume`, `GetAvailableAmount`.
  - `ABedActor`: `Rest`, `TryReserve`, `Release`, `IsAvailable`, `OccupyingAgent`.
  - `APineTreeActor`, `ACampfireActor` (base actors).
- **Debug**
  - `UGOAPDebugWidget`: optional `ConsolidatedDebugText` (single block) or separate WorldState/Goal/Plan/Action text; one widget per session; `InitializeWidget(AAgent*)`.

[0.1.5]: https://github.com/kealmcking/UE-GOAP
