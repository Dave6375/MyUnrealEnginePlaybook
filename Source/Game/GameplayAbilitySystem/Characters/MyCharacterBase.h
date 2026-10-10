// Fill out your copyright notice in the Description page of Project Settings.
// Definitions of the functions
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "MyCharacterBase.generated.h"

// Forward declarations
class UAbilitySystemComponent;
class UBasicAttributeSet;
class UMeleeAttributeSet;
class UStateTreeComponent;
class UPlayerCombatComponent;
class UGameplayAbility;

UCLASS()
class GAME_API AMyCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacterBase();
	
	// Ability System Interface
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem")
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem")
	class UBasicAttributeSet* BasicAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem")
	class UMeleeAttributeSet* MeleeAttributeSet;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	class UStateTreeComponent* StateTreeComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	class UPlayerCombatComponent* CombatComponent;
	
	// Ability System Replication Mode
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AbilitySystem")
	EGameplayEffectReplicationMode ASCReplicationMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void OnRep_PlayerState() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

};
