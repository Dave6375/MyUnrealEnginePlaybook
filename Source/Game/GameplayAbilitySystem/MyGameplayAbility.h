// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MyGameplayAbility.generated.h"

/**
 * Defines how an ability interacts with other active abilities.
 */
UENUM(BlueprintType)
enum class EMyAbilityActivationGroup : uint8
{
	// Ability runs independently without affecting or being affected by other abilities.
	Independent,

	// Ability is cancelled and replaced if another ability in this group triggers.
	Exclusive_Replaceable,

	// Ability blocks any other ability in this group from activating until this one finishes.
	Exclusive_Blocking,

	MAX UMETA(Hidden)
};

/**
 * Base gameplay ability class used by this project.
 */
UCLASS(Abstract)
class GAME_API UMyGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	EMyAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
	EMyAbilityActivationGroup ActivationGroup = EMyAbilityActivationGroup::Independent;
};
