#include "StateTreeTask_EmitNoise.h"
#include "BaseNoiseEmitter.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FStateTreeTask_EmitNoise::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	AActor* Owner = Cast<AActor>(Context.GetOwner());
	if (ABaseNoiseEmitter* Emitter = Cast<ABaseNoiseEmitter>(Owner))
	{
		Emitter->EmitNoise();
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Failed;
}
