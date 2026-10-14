// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAbilitySystemComponent.h"
#include "MyGameplayAbility.h"
#include "MyAbilityTagRelationshipMapping.h"
#include "Game/MyGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyAbilitySystemComponent)

UMyAbilitySystemComponent::UMyAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FMemory::Memzero(ActivationGroupCounts, sizeof(ActivationGroupCounts));
}

void UMyAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UMyAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	TryActivateAbilitiesOnSpawn();
}

void UMyAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
{
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (!Spec.IsActive())
		{
			continue;
		}

		UMyGameplayAbility* AbilityCDO = Cast<UMyGameplayAbility>(Spec.Ability);
		if (!AbilityCDO)
		{
			continue;
		}

		if (ShouldCancelFunc(AbilityCDO, Spec.Handle))
		{
			if (Spec.Ability->IsInstantiated())
			{
				TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
				for (UGameplayAbility* Instance : Instances)
				{
					if (Instance && Instance->IsActive())
					{
						CancelAbilitySpec(Spec, Instance);
					}
				}
			}
			else
			{
				CancelAbilitySpec(Spec, nullptr);
			}
		}
	}
}

void UMyAbilitySystemComponent::CancelInputActivatedAbilities(bool bReplicateCancelAbility)
{
	auto ShouldCancel = [](const UGameplayAbility* Ability, FGameplayAbilitySpecHandle Handle)
	{
		const UMyGameplayAbility* MyAbility = Cast<UMyGameplayAbility>(Ability);
		return MyAbility != nullptr;
	};

	CancelAbilitiesByFunc(ShouldCancel, bReplicateCancelAbility);
}

void UMyAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability && Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			InputPressedSpecHandles.AddUnique(Spec.Handle);
			InputHeldSpecHandles.AddUnique(Spec.Handle);
		}
	}
}

void UMyAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability && Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			InputReleasedSpecHandles.AddUnique(Spec.Handle);
			InputHeldSpecHandles.Remove(Spec.Handle);
		}
	}
}

void UMyAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (Spec->Ability)
			{
				Spec->InputPressed = true;
				if (Spec->IsActive())
				{
					AbilitySpecInputPressed(*Spec);
				}
				else
				{
					AbilitiesToActivate.AddUnique(SpecHandle);
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(SpecHandle);
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (Spec->Ability)
			{
				Spec->InputPressed = false;
				if (Spec->IsActive())
				{
					AbilitySpecInputReleased(*Spec);
				}
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UMyAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

//////////////////////////////////////////////////////////////////////
// Activation Groups

bool UMyAbilitySystemComponent::IsActivationGroupBlocked(EMyAbilityActivationGroup Group) const
{
	bool bBlocked = false;

	switch (Group)
	{
	case EMyAbilityActivationGroup::Independent:
		bBlocked = false;
		break;
	case EMyAbilityActivationGroup::Exclusive_Replaceable:
	case EMyAbilityActivationGroup::Exclusive_Blocking:
		bBlocked = (ActivationGroupCounts[static_cast<uint8>(EMyAbilityActivationGroup::Exclusive_Blocking)] > 0);
		break;
	default:
		checkf(false, TEXT("IsActivationGroupBlocked: Invalid Group [%d]\n"), static_cast<uint8>(Group));
		break;
	}

	return bBlocked;
}

void UMyAbilitySystemComponent::AddAbilityToActivationGroup(EMyAbilityActivationGroup Group, UMyGameplayAbility* Ability)
{
	check(Ability);
	check(ActivationGroupCounts[static_cast<uint8>(Group)] < INT32_MAX);

	ActivationGroupCounts[static_cast<uint8>(Group)]++;

	const bool bIsExclusive = (Group == EMyAbilityActivationGroup::Exclusive_Replaceable || Group == EMyAbilityActivationGroup::Exclusive_Blocking);
	if (bIsExclusive)
	{
		// Cancel replaceable abilities when a new exclusive ability starts
		CancelActivationGroupAbilities(EMyAbilityActivationGroup::Exclusive_Replaceable, Ability, true);
	}
}

void UMyAbilitySystemComponent::RemoveAbilityFromActivationGroup(EMyAbilityActivationGroup Group, UMyGameplayAbility* Ability)
{
	check(Ability);
	check(ActivationGroupCounts[static_cast<uint8>(Group)] > 0);

	ActivationGroupCounts[static_cast<uint8>(Group)]--;
}

void UMyAbilitySystemComponent::CancelActivationGroupAbilities(EMyAbilityActivationGroup Group, UMyGameplayAbility* IgnoreAbility, bool bReplicateCancelAbility)
{
	auto ShouldCancel = [Group, IgnoreAbility](const UGameplayAbility* Ability, FGameplayAbilitySpecHandle Handle)
	{
		const UMyGameplayAbility* MyAbility = Cast<UMyGameplayAbility>(Ability);
		return (MyAbility && MyAbility->GetActivationGroup() == Group && MyAbility != IgnoreAbility);
	};

	CancelAbilitiesByFunc(ShouldCancel, bReplicateCancelAbility);
}

//////////////////////////////////////////////////////////////////////
// Dynamic Tags & Relationships

void UMyAbilitySystemComponent::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	if (!Tag.IsValid())
	{
		return;
	}

	const TSubclassOf<UGameplayEffect> DynamicGEClass = UGameplayEffect::StaticClass();
	FGameplayEffectSpec DynamicGESpec(DynamicGEClass.GetDefaultObject(), MakeEffectContext(), 1.0f);
	DynamicGESpec.DynamicGrantedTags.AddTag(Tag);

	ApplyGameplayEffectSpecToSelf(DynamicGESpec);
}

void UMyAbilitySystemComponent::RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	if (!Tag.IsValid())
	{
		return;
	}

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(Tag);
	RemoveActiveEffectsWithGrantedTags(TagContainer);
}

void UMyAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
	TSharedPtr<FAbilityReplicatedDataCache> TargetDataEntry = AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, ActivationInfo.GetActivationPredictionKey()));
	if (TargetDataEntry.IsValid())
	{
		OutTargetDataHandle = TargetDataEntry->TargetData;
	}
}

void UMyAbilitySystemComponent::SetTagRelationshipMapping(UMyAbilityTagRelationshipMapping* NewMapping)
{
	TagRelationshipMapping = NewMapping;
}

void UMyAbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, &OutActivationRequired, &OutActivationBlocked);
	}
}

void UMyAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (const UMyGameplayAbility* AbilityCDO = Cast<UMyGameplayAbility>(Spec.Ability))
		{
			// Activate abilities configured to activate on spawn (passive/startup abilities)
			TryActivateAbility(Spec.Handle);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Overrides & Lifecycle Hooks

void UMyAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);
}

void UMyAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);
}

void UMyAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);

	if (UMyGameplayAbility* MyAbility = Cast<UMyGameplayAbility>(Ability))
	{
		AddAbilityToActivationGroup(MyAbility->GetActivationGroup(), MyAbility);
	}
}

void UMyAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);

	if (APawn* Avatar = Cast<APawn>(GetAvatarActor()))
	{
		if (!Avatar->IsLocallyControlled() && Ability->IsSupportedForNetworking())
		{
			ClientNotifyAbilityFailed(Ability, FailureReason);
			return;
		}
	}

	HandleAbilityFailed(Ability, FailureReason);
}

void UMyAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

	if (UMyGameplayAbility* MyAbility = Cast<UMyGameplayAbility>(Ability))
	{
		RemoveAbilityFromActivationGroup(MyAbility->GetActivationGroup(), MyAbility);
	}
}

void UMyAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	FGameplayTagContainer ModifiedBlockTags = BlockTags;
	FGameplayTagContainer ModifiedCancelTags = CancelTags;

	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &ModifiedBlockTags, &ModifiedCancelTags);
	}

	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, ModifiedBlockTags, bExecuteCancelTags, ModifiedCancelTags);
}

void UMyAbilitySystemComponent::HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled)
{
	Super::HandleChangeAbilityCanBeCanceled(AbilityTags, RequestingAbility, bCanBeCanceled);
}

void UMyAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	HandleAbilityFailed(Ability, FailureReason);
}

void UMyAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	// Broadcast to UI or audio listeners here if needed (e.g. cooldown sounds, "Out of Mana" text)
}