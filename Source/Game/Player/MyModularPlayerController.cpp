// Fill out your copyright notice in the Description page of Project Settings.

#include "MyModularPlayerController.h"
#include "MyModularPlayerState.h"
#include "Game/GameplayAbilitySystem/MyAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyModularPlayerController)

AMyModularPlayerController::AMyModularPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

AMyModularPlayerState* AMyModularPlayerController::GetMyPlayerState() const
{
	return CastChecked<AMyModularPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UMyAbilitySystemComponent* AMyModularPlayerController::GetMyAbilitySystemComponent() const
{
	const AMyModularPlayerState* PS = GetMyPlayerState();
	return PS ? PS->GetMyAbilitySystemComponent() : nullptr;
}

UAbilitySystemComponent* AMyModularPlayerController::GetAbilitySystemComponent() const
{
	return GetMyAbilitySystemComponent();
}

void AMyModularPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UMyAbilitySystemComponent* MyASC = GetMyAbilitySystemComponent())
	{
		MyASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

