#include "BTTask_Attack.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Stack.h"
#include "Game/PlayerCombatComponent.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Trigger Melee Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	// 1. Try CustomEvent
	UFunction* Func = Pawn->FindFunction(FName("CustomEvent"));
	if (Func)
	{
		Pawn->ProcessEvent(Func, nullptr);
		return EBTNodeResult::Succeeded;
	}

	// 2. Try Attack function
	Func = Pawn->FindFunction(FName("Attack"));
	if (Func)
	{
		Pawn->ProcessEvent(Func, nullptr);
		return EBTNodeResult::Succeeded;
	}

	// 3. Try CombatComponent HandleCombatInput if present
	if (UPlayerCombatComponent* CombatComp = Pawn->FindComponentByClass<UPlayerCombatComponent>())
	{
		if (CombatComp->MeleeAbilityTag.IsValid())
		{
			CombatComp->HandleCombatInput(CombatComp->MeleeAbilityTag);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
