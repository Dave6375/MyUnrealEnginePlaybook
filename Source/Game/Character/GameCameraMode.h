// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameCameraMode.generated.h"

/**
 * UGameCameraMode
 * 
 * Base class for camera modes used by pawns / characters.
 */
UCLASS(Abstract, Blueprintable)
class GAME_API UGameCameraMode : public UObject
{
	GENERATED_BODY()
};
