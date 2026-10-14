// Fill out your copyright notice in the Description page of Project Settings.

#include "MyModulerCharacter.h"
#include "MyPawnExtensionComponent.h"
#include "Game/GameplayAbilitySystem/MyAbilitySystemComponent.h"
#include "Game/Player/MyModularPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyModulerCharacter)

AMyModulerCharacter::AMyModulerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	PawnExtComponent = CreateDefaultSubobject<UMyPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
}

UAbilitySystemComponent* AMyModulerCharacter::GetAbilitySystemComponent() const
{
	return PawnExtComponent ? PawnExtComponent->GetMyAbilitySystemComponent() : nullptr;
}

UMyAbilitySystemComponent* AMyModulerCharacter::GetMyAbilitySystemComponent() const
{
	return PawnExtComponent ? PawnExtComponent->GetMyAbilitySystemComponent() : nullptr;
}

void AMyModulerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PawnExtComponent->HandleControllerChanged();

	if (AMyModularPlayerState* PS = GetPlayerState<AMyModularPlayerState>())
	{
		PawnExtComponent->InitializeAbilitySystem(PS->GetMyAbilitySystemComponent(), PS);
	}
}

void AMyModulerCharacter::UnPossessed()
{
	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();
}

void AMyModulerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtComponent->HandleControllerChanged();
}

void AMyModulerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtComponent->HandlePlayerStateReplicated();

	if (AMyModularPlayerState* PS = GetPlayerState<AMyModularPlayerState>())
	{
		PawnExtComponent->InitializeAbilitySystem(PS->GetMyAbilitySystemComponent(), PS);
	}
}

void AMyModulerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

