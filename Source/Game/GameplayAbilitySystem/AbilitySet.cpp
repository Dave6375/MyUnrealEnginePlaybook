#include "AbilitySet.h"
#include "MyAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilitySet)

//////////////////////////////////////////////////////////////////////
// FGameAbilitySet_GrantedHandles

void FGameAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FGameAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FGameAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	if (Set)
	{
		GrantedAttributeSets.Add(Set);
	}
}

void FGameAbilitySet_GrantedHandles::TakeFromAbilitySystem(UMyAbilitySystemComponent* GameASC)
{
	check(GameASC);

	if (!GameASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative (Server) to remove granted abilities and effects
		return;
	}

	// 1. Remove Granted Abilities
	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			GameASC->ClearAbility(Handle);
		}
	}

	// 2. Remove Granted Gameplay Effects
	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			GameASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	// 3. Remove Granted Attribute Sets
	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		if (Set)
		{
			GameASC->RemoveSpawnedAttribute(Set);
		}
	}

	// Clear out tracking arrays
	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

//////////////////////////////////////////////////////////////////////
// UAbilitySet

UAbilitySet::UAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAbilitySet::GiveToAbilitySystem(UMyAbilitySystemComponent* InASC, FGameAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(InASC);

	if (!InASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative (Server) to grant abilities and effects
		return;
	}

	// 1. Grant Gameplay Abilities
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FGameAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			continue;
		}

		UGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;

		// Bind the input tag to the ability dynamic tags so MyAbilitySystemComponent can trigger it by tag
		if (AbilityToGrant.InputTag.IsValid())
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);
		}

		const FGameplayAbilitySpecHandle AbilitySpecHandle = InASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// 2. Grant Gameplay Effects
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FGameAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = InASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, InASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	// 3. Grant Attribute Sets
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FGameAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(InASC->GetOwner(), SetToGrant.AttributeSet);
		InASC->AddAttributeSetSubobject(NewSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}