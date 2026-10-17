// Fill out your copyright notice in the Description page of Project Settings.

#include "MyHealthComponent.h"
#include "MyPawnExtensionComponent.h"
#include "Game/MyGameplayTags.h"
#include "Game/GameplayAbilitySystem/MyAbilitySystemComponent.h"
#include "Game/GameplayAbilitySystem/Attributes/BasicAttributeSet.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyHealthComponent)

const FName UMyHealthComponent::NAME_ActorFeatureName("Health");

UMyHealthComponent::UMyHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
	DeathState = EMyDeathState::NotDead;
}

void UMyHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMyHealthComponent, DeathState);
}

void UMyHealthComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf(Pawn != nullptr, TEXT("MyHealthComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> HealthComponents;
	Pawn->GetComponents(UMyHealthComponent::StaticClass(), HealthComponents);
	ensureAlwaysMsgf(HealthComponents.Num() == 1, TEXT("Only one MyHealthComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	RegisterInitStateFeature();
}

void UMyHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(UMyPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	ensure(TryToChangeInitState(MyGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UMyHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	UninitializeFromAbilitySystem();

	Super::EndPlay(EndPlayReason);
}

bool UMyHealthComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == MyGameplayTags::InitState_Spawned)
	{
		return (Pawn != nullptr);
	}
	else if (CurrentState == MyGameplayTags::InitState_Spawned && DesiredState == MyGameplayTags::InitState_DataAvailable)
	{
		return true;
	}
	else if (CurrentState == MyGameplayTags::InitState_DataAvailable && DesiredState == MyGameplayTags::InitState_DataInitialized)
	{
		const UMyPawnExtensionComponent* PawnExtComp = UMyPawnExtensionComponent::GetPawnExtensionComponent(Pawn);
		return PawnExtComp && PawnExtComp->GetMyAbilitySystemComponent() != nullptr;
	}
	else if (CurrentState == MyGameplayTags::InitState_DataInitialized && DesiredState == MyGameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UMyHealthComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == MyGameplayTags::InitState_DataAvailable && DesiredState == MyGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		const UMyPawnExtensionComponent* PawnExtComp = UMyPawnExtensionComponent::GetPawnExtensionComponent(Pawn);
		if (PawnExtComp)
		{
			if (UMyAbilitySystemComponent* ASC = PawnExtComp->GetMyAbilitySystemComponent())
			{
				InitializeWithAbilitySystem(ASC);
			}
		}
	}
}

void UMyHealthComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UMyPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == MyGameplayTags::InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

void UMyHealthComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = {
		MyGameplayTags::InitState_Spawned,
		MyGameplayTags::InitState_DataAvailable,
		MyGameplayTags::InitState_DataInitialized,
		MyGameplayTags::InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
}

void UMyHealthComponent::InitializeWithAbilitySystem(UMyAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(InASC);

	if (AbilitySystemComponent)
	{
		UninitializeFromAbilitySystem();
	}

	AbilitySystemComponent = InASC;
	HealthSet = AbilitySystemComponent->GetSet<UBasicAttributeSet>();

	if (!HealthSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("MyHealthComponent: BasicAttributeSet not found on [%s]."), *GetNameSafe(Owner));
		return;
	}

	// Register change delegates
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBasicAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBasicAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBasicAttributeSet::GetShieldAttribute()).AddUObject(this, &ThisClass::HandleShieldChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBasicAttributeSet::GetMaxShieldAttribute()).AddUObject(this, &ThisClass::HandleMaxShieldChanged);

	// Broadcast initial values
	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetMaxHealth(), HealthSet->GetMaxHealth(), nullptr);
	OnShieldChanged.Broadcast(this, HealthSet->GetShield(), HealthSet->GetShield(), nullptr);
	OnMaxShieldChanged.Broadcast(this, HealthSet->GetMaxShield(), HealthSet->GetMaxShield(), nullptr);
}

void UMyHealthComponent::UninitializeFromAbilitySystem()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBasicAttributeSet::GetHealthAttribute()).RemoveAll(this);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBasicAttributeSet::GetMaxHealthAttribute()).RemoveAll(this);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBasicAttributeSet::GetShieldAttribute()).RemoveAll(this);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBasicAttributeSet::GetMaxShieldAttribute()).RemoveAll(this);
	}

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
}

float UMyHealthComponent::GetHealth() const
{
	return HealthSet ? HealthSet->GetHealth() : 0.0f;
}

float UMyHealthComponent::GetMaxHealth() const
{
	return HealthSet ? HealthSet->GetMaxHealth() : 0.0f;
}

float UMyHealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();
		return (MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f;
	}
	return 0.0f;
}

float UMyHealthComponent::GetShield() const
{
	return HealthSet ? HealthSet->GetShield() : 0.0f;
}

float UMyHealthComponent::GetMaxShield() const
{
	return HealthSet ? HealthSet->GetMaxShield() : 0.0f;
}

float UMyHealthComponent::GetShieldNormalized() const
{
	if (HealthSet)
	{
		const float Shield = HealthSet->GetShield();
		const float MaxShield = HealthSet->GetMaxShield();
		return (MaxShield > 0.0f) ? (Shield / MaxShield) : 0.0f;
	}
	return 0.0f;
}

void UMyHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);

	if (ChangeData.NewValue <= 0.0f && DeathState == EMyDeathState::NotDead)
	{
		StartDeath();
	}
}

void UMyHealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnMaxHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);
}

void UMyHealthComponent::HandleShieldChanged(const FOnAttributeChangeData& ChangeData)
{
	OnShieldChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);
}

void UMyHealthComponent::HandleMaxShieldChanged(const FOnAttributeChangeData& ChangeData)
{
	OnMaxShieldChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);
}

void UMyHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	if (DeathState == EMyDeathState::NotDead)
	{
		StartDeath();
	}
}

void UMyHealthComponent::StartDeath()
{
	if (DeathState != EMyDeathState::NotDead)
	{
		return;
	}

	DeathState = EMyDeathState::DeathStarted;

	if (GetOwner()->HasAuthority())
	{
		// Detach from controller on death if desired
	}

	OnDeathStarted.Broadcast(GetOwner());
}

void UMyHealthComponent::FinishDeath()
{
	if (DeathState != EMyDeathState::DeathStarted)
	{
		return;
	}

	DeathState = EMyDeathState::DeathFinished;
	OnDeathFinished.Broadcast(GetOwner());
}

void UMyHealthComponent::OnRep_DeathState(EMyDeathState OldDeathState)
{
	if (OldDeathState == EMyDeathState::NotDead && DeathState == EMyDeathState::DeathStarted)
	{
		OnDeathStarted.Broadcast(GetOwner());
	}
	else if (OldDeathState == EMyDeathState::DeathStarted && DeathState == EMyDeathState::DeathFinished)
	{
		OnDeathFinished.Broadcast(GetOwner());
	}
}
