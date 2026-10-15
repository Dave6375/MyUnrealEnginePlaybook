// Fill out your copyright notice in the Description page of Project Settings.

#include "MyModulerCharacter.h"
#include "MyPawnExtensionComponent.h"
#include "MyHeroComponent.h"
#include "UPawnData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Game/GameplayAbilitySystem/MyAbilitySystemComponent.h"
#include "Game/Player/MyModularPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyModulerCharacter)

AMyModulerCharacter::AMyModulerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if (UCharacterMovementComponent* CharMovement = GetCharacterMovement())
	{
		CharMovement->bOrientRotationToMovement = true;
		CharMovement->bUseControllerDesiredRotation = false;
		CharMovement->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	}

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

	if (PawnData && HasAuthority() && PawnExtComponent->GetPawnData<UUPawnData>() == nullptr)
	{
		PawnExtComponent->SetPawnData(PawnData);
	}

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

	if (UMyHeroComponent* HeroComp = UMyHeroComponent::FindHeroComponent(this))
	{
		HeroComp->InitializePlayerInput(PlayerInputComponent);
	}
}

