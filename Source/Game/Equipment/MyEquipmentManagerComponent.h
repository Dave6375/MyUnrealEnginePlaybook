// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Game/GameplayAbilitySystem/AbilitySet.h"
#include "MyEquipmentDefinition.h"
#include "MyEquipmentInstance.h"
#include "MyEquipmentManagerComponent.generated.h"

class UMyEquipmentDefinition;
class UMyEquipmentInstance;
class UMyEquipmentManagerComponent;
class UMyAbilitySystemComponent;
class UObject;
struct FEquipmentList;

/** A single applied piece of equipment entry */
USTRUCT(BlueprintType)
struct FAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FAppliedEquipmentEntry()
		: EquipmentDefinition(nullptr)
		, Instance(nullptr)
	{}

private:
	friend FEquipmentList;
	friend UMyEquipmentManagerComponent;

	UPROPERTY()
	TSubclassOf<UMyEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UMyEquipmentInstance> Instance;

	UPROPERTY(NotReplicated)
	FGameAbilitySet_GrantedHandles GrantedHandles;
};

/** Replicated list of applied equipment entries */
USTRUCT(BlueprintType)
struct FEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FEquipmentList()
		: OwnerComponent(nullptr)
	{}

	FEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{}

public:
	//~ Begin FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~ End FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FAppliedEquipmentEntry, FEquipmentList>(Entries, DeltaParms, *this);
	}

	UMyEquipmentInstance* AddEntry(TSubclassOf<UMyEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UMyEquipmentInstance* Instance);

private:
	UMyAbilitySystemComponent* GetAbilitySystemComponent() const;

private:
	friend UMyEquipmentManagerComponent;

	UPROPERTY()
	TArray<FAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FEquipmentList> : public TStructOpsTypeTraitsBase2<FEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};

/**
 * UMyEquipmentManagerComponent
 *
 * Manages equipment instances applied to a pawn.
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class GAME_API UMyEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UMyEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment")
	UMyEquipmentInstance* EquipItem(TSubclassOf<UMyEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment")
	void UnequipItem(UMyEquipmentInstance* ItemInstance);

	//~ Begin UActorComponent Interface
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~ End UActorComponent Interface

	/** Returns the first equipment instance of the specified class */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	UMyEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UMyEquipmentInstance> InstanceType);

	/** Returns all equipment instances of the specified class */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	TArray<UMyEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UMyEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return Cast<T>(GetFirstInstanceOfType(T::StaticClass()));
	}

private:
	UPROPERTY(Replicated)
	FEquipmentList EquipmentList;
};
