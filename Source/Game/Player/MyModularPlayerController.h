// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyModularPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AMyModularPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMyModularPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
