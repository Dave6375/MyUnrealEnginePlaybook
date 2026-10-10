#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreeTask_EmitNoise.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "STT Emit Noise Native"))
struct GAME_API FStateTreeTask_EmitNoise : public FStateTreeTaskBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
