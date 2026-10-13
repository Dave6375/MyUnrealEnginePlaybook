// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UPawnData.generated.h"

/**
 * GamePawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintableType, Const, meta = (DisplayName = "Game Pawn Data", ShortTooltip = "Data asset used to define a Pawn. "))
class GAME_API UUPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// Class to instantiate for this pawn (should usually derive from APawn or ACharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Pawn")
	TSubclassOf<APawn> PawnClass;
	
	//Ability sets to grant to this Pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abilities")
	TArray<TObjectPtr<AbilitySet> AbilitySets;
};

