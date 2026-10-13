// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySet.generated.h"

{
public:
	
};
/**
 * AbilitySet_GameplayAbility
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
	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
};


/**
* FGameplayAbilitySet_GrantedHandles
* Data used to store handles to what has been granted by the ability set.
*/
USTRUCT(BlueprintType)
struct FGameAbilitySet_GrantedHandles
{
	GENERATED_BODY()
	
public:
	void AddAbilitySpectHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set)
	void TakeFromAbilitySystem(UGameAbilitySystemComponent* GameASC);
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAME_API UAbilitySet : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAbilitySet();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
