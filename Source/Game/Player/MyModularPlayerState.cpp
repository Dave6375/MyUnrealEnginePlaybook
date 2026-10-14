// Fill out your copyright notice in the Description page of Project Settings.

#include "MyModularPlayerState.h"
#include "Game/GameplayAbilitySystem/MyAbilitySystemComponent.h"
#include "Game/Character/UPawnData.h"
#include "Game/Character/MyPawnExtensionComponent.h"
#include "Game/GameplayAbilitySystem/AbilitySet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyModularPlayerState)

AMyModularPlayerState::AMyModularPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UMyAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	NetUpdateFrequency = 100.0f;
}

void AMyModularPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyModularPlayerState, PawnData);
}

UAbilitySystemComponent* AMyModularPlayerState::GetAbilitySystemComponent() const
{
	return GetMyAbilitySystemComponent();
}

void AMyModularPlayerState::SetPawnData(const UUPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogTemp, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;

	for (const UAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	if (APawn* Pawn = GetPawn())
	{
		if (UMyPawnExtensionComponent* PawnExtComp = UMyPawnExtensionComponent::GetPawnExtensionComponent(Pawn))
		{
			PawnExtComp->CheckDefaultInitialization();
		}
	}
}

void AMyModularPlayerState::OnRep_PawnData()
{
}