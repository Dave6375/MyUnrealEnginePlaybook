// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MyEquipmentDefinition.h"
#include "MyEquipmentInstance.generated.h"

class AActor;
class APawn;
struct FFrame;

/**
 * UMyEquipmentInstance
 * 
 * A piece of equipment spawned and applied to a pawn.
 * Tracks spawned physical actors and provides lifecycle hooks.
 */
UCLASS(BlueprintType, Blueprintable)
class GAME_API UMyEquipmentInstance : public UObject
{
	GENERATED_BODY()
	
public:
	UMyEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Begin UObject Interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override;
	//~ End UObject Interface

	/** Returns the object that instigated this equipment */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	UObject* GetInstigator() const { return Instigator; }

	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }

	/** Returns the owning Pawn */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	APawn* GetPawn() const;

	/** Returns the typed owning Pawn */
	UFUNCTION(BlueprintPure, Category = "Equipment", meta = (DeterminesOutputType = "PawnType"))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	/** Returns all physical actors spawned by this equipment */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	virtual void SpawnEquipmentActors(const TArray<FEquipmentActorActorToSpawn>& ActorsToSpawn);
	virtual void DestroyEquipmentActors();

	virtual void OnEquipped();
	virtual void OnUnequipped();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "OnUnequipped"))
	void K2_OnUnequipped();

	UFUNCTION()
	virtual void OnRep_Instigator();

	UFUNCTION()
	virtual void OnRep_SpawnedActors();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Instigator)
	TObjectPtr<UObject> Instigator;

	UPROPERTY(ReplicatedUsing = OnRep_SpawnedActors)
	TArray<TObjectPtr<AActor>> SpawnedActors;
};
