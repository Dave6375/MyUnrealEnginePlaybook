// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyModularPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AMyModularPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AMyModularPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
