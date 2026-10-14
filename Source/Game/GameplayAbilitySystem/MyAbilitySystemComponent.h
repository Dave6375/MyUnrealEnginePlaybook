// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "NativeGameplayTags.h"
#include "MyGameplayAbility.h"
#include "MyAbilitySystemComponent.generated.h"

class AActor;
class UGameplayAbility;
class UMyAbilityTagRelationshipMapping;
class UObject;
struct FFrame;
struct FGameplayAbilityTargetDataHandle;

/**
 * UMyAbilitySystemComponent
 * Base ability system component class used by this project.
 */
UCLASS()
class GAME_API UMyAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UMyAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface
	
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	
	typedef TFunctionRef<bool(const UGameplayAbility* GameAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);
	
	void CancelInputActivatedAbilities(bool bReplicateCancelAbility);
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();
	
	// Activation Groups
	bool IsActivationGroupBlocked(EMyAbilityActivationGroup Group) const;
	void AddAbilityToActivationGroup(EMyAbilityActivationGroup Group, UMyGameplayAbility* Ability);
	void RemoveAbilityFromActivationGroup(EMyAbilityActivationGroup Group, UMyGameplayAbility* Ability);
	void CancelActivationGroupAbilities(EMyAbilityActivationGroup Group, UMyGameplayAbility* IgnoreAbility, bool bReplicateCancelAbility);

	// Dynamic Tag Effect Helpers (granting temporary runtime tags via effects)
	void AddDynamicTagGameplayEffect(const FGameplayTag& Tag);
	void RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag);
	
	// Target Data Helper
	void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle);

	// Tag Relationships (Optional data asset for managing global cancel/block rules)
	void SetTagRelationshipMapping(UMyAbilityTagRelationshipMapping* NewMapping);
	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

	void TryActivateAbilitiesOnSpawn();

protected:
	//~ Begin UAbilitySystemComponent Overrides
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	virtual void HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled) override;
	//~ End UAbilitySystemComponent Overrides

	// Client RPC to broadcast why an ability failed to activate (cooldown, missing cost, etc.)
	UFUNCTION(Client, Unreliable)
	void ClientNotifyAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

	void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

protected:
	UPROPERTY()
	TObjectPtr<UMyAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Handles buffered each frame for Enhanced Input
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// Array tracking count of running abilities in each activation group
	int32 ActivationGroupCounts[static_cast<uint8>(EMyAbilityActivationGroup::MAX)];
};
