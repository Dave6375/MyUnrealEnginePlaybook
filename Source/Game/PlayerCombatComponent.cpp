#include "PlayerCombatComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"

UPlayerCombatComponent::UPlayerCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// Cache the owner's Ability System Component
	if (AActor* Owner = GetOwner())
	{
		if (IAbilitySystemInterface* ASInterface = Cast<IAbilitySystemInterface>(Owner))
		{
			ASC = ASInterface->GetAbilitySystemComponent();
		}
		
		if (!ASC)
		{
			ASC = Owner->FindComponentByClass<UAbilitySystemComponent>();
		}
	}
}

void UPlayerCombatComponent::HandleCombatInput(FGameplayTag AttackTag)
{
	if (ASC && AttackTag.IsValid())
	{
		// Check if we have the "CanAttack" tag (mirrors the video's branch logic)
		if (ASC->HasMatchingGameplayTag(CanAttackTag))
		{
			ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
		}
	}
}

void UPlayerCombatComponent::ApplyDamageToTarget(AActor* HitActor)
{
	// Check if hit actor has an Ability System Component
	if (IAbilitySystemInterface* HitASCInterface = Cast<IAbilitySystemInterface>(HitActor))
	{
		UAbilitySystemComponent* TargetASC = HitASCInterface->GetAbilitySystemComponent();
		if (ASC && TargetASC && DamageGameplayEffectClass)
		{
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageGameplayEffectClass, 1.0f, ASC->MakeEffectContext());
			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			}
		}
	}
}

