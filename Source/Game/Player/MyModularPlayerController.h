// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemInterface.h"
#include "MyModularPlayerController.generated.h"

class UMyAbilitySystemComponent;
class AMyModularPlayerState;

/**
 * AMyModularPlayerController
 *
 * Base player controller class following Lyra's architecture.
 */
UCLASS()
class GAME_API AMyModularPlayerController : public APlayerController, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AMyModularPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UMyAbilitySystemComponent* GetMyAbilitySystemComponent() const;

	AMyModularPlayerState* GetMyPlayerState() const;

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
};
