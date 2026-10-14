// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySet.generated.h"

class UGameplayEffect;
class UAttributeSet;
class UMyAbilitySystemComponent;
class UGameplayAbility;
class UObject;


/**
 * FGameAbilitySet_GameplayAbility
 * Data used by the ability set to grant gameplay abilities
 */
USTRUCT(BlueprintType)
struct FGameAbilitySet_GameplayAbility
{
	GENERATED_BODY()
	
public:
	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> Ability;
	
	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;
	
	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


/**
 * FGameAbilitySet_GameplayEffect
 * 
 * Data used by ability set to grant gameplay effects
 */
USTRUCT(BlueprintType)
struct FGameAbilitySet_GameplayEffect
{
	GENERATED_BODY()
	
public:
	// Gameplay effect to grant
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect;
	
	// Level of gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};


/**
 * FGameAbilitySet_AttributeSet
 * Data used by the ability set to grant attribute sets
 */
USTRUCT(BlueprintType)
struct FGameAbilitySet_AttributeSet
{
	GENERATED_BODY()
	
public:
	// Attribute set to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
};


/**
 * FGameAbilitySet_GrantedHandles
 * Data used to store handles to what has been granted by the ability set.
 */
USTRUCT(BlueprintType)
struct FGameAbilitySet_GrantedHandles
{
	GENERATED_BODY()
	
public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);
	void TakeFromAbilitySystem(UMyAbilitySystemComponent* GameASC);

protected:
	// Handles to granted abilities.
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
	
	// Handles to the granted gameplay effects.
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;
	
	// Pointers to the granted attribute sets
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};


/**
 * UAbilitySet
 * 
 * Non-mutable data asset used to grant gameplay abilities and gameplay effect
 */
UCLASS(BlueprintType, Const)
class GAME_API UAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/**
	 * Grants the ability set to the specified ability system component.
	 * The returned handles can be used later to take away anything that was granted.
	 */
	void GiveToAbilitySystem(UMyAbilitySystemComponent* GameASC, FGameAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;
	
protected:
	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities", meta=(TitleProperty=Ability)) 
	TArray<FGameAbilitySet_GameplayAbility> GrantedGameplayAbilities;
	
	// Gameplay effects to grant when this ability set is granted
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FGameAbilitySet_GameplayEffect> GrantedGameplayEffects;
	
	UPROPERTY(EditDefaultsOnly, Category="Attribute Sets", meta=(TitleProperty=AttributeSet)) 
	TArray<FGameAbilitySet_AttributeSet> GrantedAttributes;
};
