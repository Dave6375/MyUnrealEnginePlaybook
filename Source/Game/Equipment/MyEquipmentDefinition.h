// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "Templates/SubclassOf.h"
#include "MyEquipmentDefinition.generated.h"

class AActor;
class UAbilitySet;
class UMyEquipmentInstance;

USTRUCT()
struct FEquipmentActorActorToSpawn
{
	GENERATED_BODY()
	
	FEquipmentActorActorToSpawn(){}
	
	UPROPERTY(EditAnywhere, Category=Equipment)
	TSubclassOf<AActor> ActorToSpawn;
	
	UPROPERTY(EditAnywhere, Category=Equipment)
	FName AttachSocket;
	
	UPROPERTY(EditAnywhere, Category=Equipment)
	FTransform AttachTransform;
};

/**
 * UEquipmentDefinition
 * Definition of a piece of equipment that can be applied to a pawn
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class GAME_API UMyEquipmentDefinition : public UObject
{
	GENERATED_BODY()
	
public:
	UMyEquipmentDefinition(const FObjectInitializer&  ObjectInitializer = FObjectInitializer::Get());
	
	// Class to spawn
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TSubclassOf<UMyEquipmentInstance> InstanceType;
	
	// Gameplay Ability sets to grant it when this equipped
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<TObjectPtr<const UAbilitySet>> AbilitySetsToGrant;
	
	// Actors to spawn on the pawn when this equipped
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<FEquipmentActorActorToSpawn> ActorsToSpawn;
};
