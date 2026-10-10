#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "PlayerCombatComponent.generated.h"

class UAbilitySystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAME_API UPlayerCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:    
	UPlayerCombatComponent();

protected:
	virtual void BeginPlay() override;

	// Cache the owner's ASC so we don't have to look it up every frame
	UPROPERTY()
	UAbilitySystemComponent* ASC;

public:    
	// An interface function your Player Character can call on input
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void HandleCombatInput(FGameplayTag AttackTag);

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	FGameplayTag MeleeAbilityTag; // e.g. Ability.Combat.Melee
	
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	FGameplayTag CanAttackTag; // e.g. Combat.State.CanAttack
	
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<class UGameplayEffect> DamageGameplayEffectClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	int32 CurrentComboIndex = 0;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyDamageToTarget(AActor* HitActor);
};

