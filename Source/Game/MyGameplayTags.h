#pragma once

#include "NativeGameplayTags.h"

namespace MyGameplayTags
{
	GAME_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	// Feature Init States (used by PawnExtensionComponent & GameFrameworkInitStateInterface)
	GAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	GAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	GAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	GAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

	// Ability Behavior
	GAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);

	// Input Tags
	GAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	GAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	GAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Stick);
	GAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
	GAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Jump);
}