// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MeleeAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API UMeleeAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UMeleeAttributeSet();

	// Attack Power: Increases damage dealt by melee attacks
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS_BASIC(UMeleeAttributeSet, AttackPower)

	// Defense: Reduces damage taken from attacks
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Defense)
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS_BASIC(UMeleeAttributeSet, Defense)

	// Attack Speed: Multiplier for attack animations
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_AttackSpeed)
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS_BASIC(UMeleeAttributeSet, AttackSpeed)

public:
	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UMeleeAttributeSet, AttackPower, OldValue);
	}

	UFUNCTION()
	void OnRep_Defense(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UMeleeAttributeSet, Defense, OldValue);
	}

	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UMeleeAttributeSet, AttackSpeed, OldValue);
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
