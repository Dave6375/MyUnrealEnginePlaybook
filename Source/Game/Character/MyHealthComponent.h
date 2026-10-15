// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "GameplayEffectTypes.h"
#include "MyHealthComponent.generated.h"

class UMyAbilitySystemComponent;
class UBasicAttributeSet;
class UObject;
struct FFrame;
struct FGameplayEffectSpec;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FMyHealth_AttributeChanged, UMyHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMyHealth_DeathEvent, AActor*, OwningActor);

UENUM(BlueprintType)
enum class EMyDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};

/**
 * UMyHealthComponent
 *
 * An actor component that handles health, damage events, and death orchestration.
 * Hooks directly into UBasicAttributeSet on the AbilitySystemComponent.
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GAME_API UMyHealthComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UMyHealthComponent(const FObjectInitializer& ObjectInitializer);

	/** Returns the health component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Health")
	static UMyHealthComponent* FindHealthComponent(const AActor* Actor) { return Actor ? Actor->FindComponentByClass<UMyHealthComponent>() : nullptr; }

	/** Initialize the component with an ability system component. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void InitializeWithAbilitySystem(UMyAbilitySystemComponent* InASC);

	/** Uninitialize the component, clearing all bindings. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void UninitializeFromAbilitySystem();

	/** Returns the current health value. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealth() const;

	/** Returns the current maximum health value. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetMaxHealth() const;

	/** Returns the current health in the range [0.0, 1.0]. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	EMyDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsDeadOrDying() const { return (DeathState > EMyDeathState::NotDead); }

	/** Begins the death sequence for the owner. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void StartDeath();

	/** Ends the death sequence for the owner. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void FinishDeath();

public:
	/** Delegate fired when the health value has changed. */
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FMyHealth_AttributeChanged OnHealthChanged;

	/** Delegate fired when the max health value has changed. */
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FMyHealth_AttributeChanged OnMaxHealthChanged;

	/** Delegate fired when the death sequence has started. */
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FMyHealth_DeathEvent OnDeathStarted;

	/** Delegate fired when the death sequence has finished. */
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FMyHealth_DeathEvent OnDeathFinished;

	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void HandleHealthChanged(const struct FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHealthChanged(const struct FOnAttributeChangeData& ChangeData);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

protected:
	/** Ability system component used by this component. */
	UPROPERTY()
	TObjectPtr<UMyAbilitySystemComponent> AbilitySystemComponent;

	/** Health attribute set used by this component. */
	UPROPERTY()
	TObjectPtr<const UBasicAttributeSet> HealthSet;

	/** Replicated death state */
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EMyDeathState DeathState;

	UFUNCTION()
	virtual void OnRep_DeathState(EMyDeathState OldDeathState);
};
