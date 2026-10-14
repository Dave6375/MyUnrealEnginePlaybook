// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "MyAbilityTagRelationshipMapping.generated.h"

/**
 * Struct that defines the relationship between different gameplay tags for abilities
 */
USTRUCT(BlueprintType)
struct FMyAbilityTagRelationship
{
	GENERATED_BODY()

	/** The tag that this container relationship is notated with */
	UPROPERTY(EditAnywhere, Category = Ability, meta = (Categories = "Gameplay.Action"))
	FGameplayTag AbilityTag;

	/** The other ability tags that will be blocked by any ability using AbilityTag */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer AbilityTagsToBlock;

	/** The other ability tags that will be canceled by any ability using AbilityTag */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer AbilityTagsToCancel;

	/** If an ability has AbilityTag, it will require all of these tags on the owner to activate */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationRequiredTags;

	/** If an ability has AbilityTag, it will be blocked from activating if the owner has any of these tags */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationBlockedTags;
};

/**
 * UMyAbilityTagRelationshipMapping
 * 
 * Mapping of how ability tags relate to other tags for blocking and canceling.
 */
UCLASS(BlueprintType, Const)
class GAME_API UMyAbilityTagRelationshipMapping : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Given a set of ability tags, get the tags to block and cancel */
	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const;

	/** Given a set of ability tags, add additional required and blocking tags */
	void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const;

	/** Returns true if the specified ability tag is canceled by any tag in the ActionTags */
	bool IsAbilityCancelledByTag(const FGameplayTagContainer& ActionTags, const FGameplayTag& AbilityTag) const;

private:
	/** The list of relationships between different gameplay tags */
	UPROPERTY(EditAnywhere, Category = Ability, meta = (TitleProperty = "AbilityTag"))
	TArray<FMyAbilityTagRelationship> AbilityTagRelationships;
};
