#include "EnvQueryContext_Player.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEnvQueryContext_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	UObject* WorldContextObject = QueryInstance.Owner.Get();
	if (!WorldContextObject)
	{
		return;
	}

	AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(WorldContextObject, 0);
	if (PlayerActor)
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, PlayerActor);
	}
}
