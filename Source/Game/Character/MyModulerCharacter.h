// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "MyModulerCharacter.generated.h"

class UMyPawnExtensionComponent;
class UMyAbilitySystemComponent;
class UAbilitySystemComponent;

UCLASS()
class GAME_API AMyModulerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyModulerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UMyAbilitySystemComponent* GetMyAbilitySystemComponent() const;

	UMyPawnExtensionComponent* GetPawnExtensionComponent() const { return PawnExtComponent; }

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMyPawnExtensionComponent> PawnExtComponent;
};
