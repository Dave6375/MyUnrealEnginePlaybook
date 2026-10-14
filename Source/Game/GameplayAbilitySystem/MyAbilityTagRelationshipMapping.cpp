// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAbilityTagRelationshipMapping.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyAbilityTagRelationshipMapping)

void UMyAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
{
	for (const FMyAbilityTagRelationship& Relationship : AbilityTagRelationships)
	{
		if (AbilityTags.HasTag(Relationship.AbilityTag))
		{
			if (OutTagsToBlock)
			{
				OutTagsToBlock->AppendTags(Relationship.AbilityTagsToBlock);
			}
			if (OutTagsToCancel)
			{
				OutTagsToCancel->AppendTags(Relationship.AbilityTagsToCancel);
			}
		}
	}
}

void UMyAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{
	for (const FMyAbilityTagRelationship& Relationship : AbilityTagRelationships)
	{
		if (AbilityTags.HasTag(Relationship.AbilityTag))
		{
			if (OutActivationRequired)
			{
				OutActivationRequired->AppendTags(Relationship.ActivationRequiredTags);
			}
			if (OutActivationBlocked)
			{
				OutActivationBlocked->AppendTags(Relationship.ActivationBlockedTags);
			}
		}
	}
}

bool UMyAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& ActionTags, const FGameplayTag& AbilityTag) const
{
	for (const FMyAbilityTagRelationship& Relationship : AbilityTagRelationships)
	{
		if (Relationship.AbilityTag == AbilityTag && Relationship.AbilityTagsToCancel.HasAny(ActionTags))
		{
			return true;
		}
	}

	return false;
}
