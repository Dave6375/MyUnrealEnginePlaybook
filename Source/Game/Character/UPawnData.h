// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UPawnData.generated.h"

class APawn;
class UAbilitySet;
class UMyAbilityTagRelationshipMapping;
class UUInputConfig;
class UGameCameraMode;

/**
 * UUPawnData
 *
 * Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, meta = (DisplayName = "Game Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class GAME_API UUPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UUPawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Class to instantiate for this pawn (should usually derive from APawn or ACharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	TSubclassOf<APawn> PawnClass;
	
	// Ability sets to grant to this Pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TObjectPtr<UAbilitySet>> AbilitySets;
	
	// What mapping of ability tags to use for actions taken by this pawn.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UMyAbilityTagRelationshipMapping> TagRelationshipMapping;
	
	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UUInputConfig> InputConfig;
	
	// Default camera mode used by player controlled pawns (Leave as nullptr if using standard SpringArm/CameraComponent).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UGameCameraMode> DefaultCameraMode;
};
