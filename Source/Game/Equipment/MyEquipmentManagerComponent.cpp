// Fill out your copyright notice in the Description page of Project Settings.

#include "MyEquipmentManagerComponent.h"
#include "MyEquipmentDefinition.h"
#include "MyEquipmentInstance.h"
#include "Game/GameplayAbilitySystem/MyAbilitySystemComponent.h"
#include "Game/GameplayAbilitySystem/AbilitySet.h"
#include "AbilitySystemGlobals.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyEquipmentManagerComponent)

//////////////////////////////////////////////////////////////////////
// FEquipmentList

void FEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FAppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->DestroyEquipmentActors();
			Entry.Instance->OnUnequipped();
		}
	}
}

void FEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FAppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr && Entry.EquipmentDefinition != nullptr)
		{
			const UMyEquipmentDefinition* EquipmentCDO = GetDefault<UMyEquipmentDefinition>(Entry.EquipmentDefinition);
			Entry.Instance->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);
			Entry.Instance->OnEquipped();
		}
	}
}

void FEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}

UMyAbilitySystemComponent* FEquipmentList::GetAbilitySystemComponent() const
{
	if (OwnerComponent)
	{
		if (AActor* OwnerActor = OwnerComponent->GetOwner())
		{
			return Cast<UMyAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor));
		}
	}
	return nullptr;
}

UMyEquipmentInstance* FEquipmentList::AddEntry(TSubclassOf<UMyEquipmentDefinition> EquipmentDefinition)
{
	check(EquipmentDefinition != nullptr);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	const UMyEquipmentDefinition* EquipmentCDO = GetDefault<UMyEquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<UMyEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UMyEquipmentInstance::StaticClass();
	}

	FAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UMyEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);
	NewEntry.Instance->SetInstigator(const_cast<UMyEquipmentDefinition*>(EquipmentCDO));

	// Spawn physical attached actors
	NewEntry.Instance->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);

	// Grant Ability Sets to AbilitySystemComponent
	if (UMyAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (const TObjectPtr<const UAbilitySet>& AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			if (AbilitySet)
			{
				AbilitySet->GiveToAbilitySystem(ASC, &NewEntry.GrantedHandles, NewEntry.Instance);
			}
		}
	}

	NewEntry.Instance->OnEquipped();
	MarkItemDirty(NewEntry);

	return NewEntry.Instance;
}

void FEquipmentList::RemoveEntry(UMyEquipmentInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			if (UMyAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			Entry.Instance->DestroyEquipmentActors();
			Entry.Instance->OnUnequipped();

			EntryIt.RemoveCurrent();
			MarkArrayDirty();
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// UMyEquipmentManagerComponent

UMyEquipmentManagerComponent::UMyEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UMyEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMyEquipmentManagerComponent, EquipmentList);
}

void UMyEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UMyEquipmentManagerComponent::UninitializeComponent()
{
	TArray<UMyEquipmentInstance*> AllInstances;
	for (const FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (Entry.Instance)
		{
			AllInstances.Add(Entry.Instance);
		}
	}

	for (UMyEquipmentInstance* Instance : AllInstances)
	{
		UnequipItem(Instance);
	}

	Super::UninitializeComponent();
}

void UMyEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
		{
			if (IsValid(Entry.Instance))
			{
				AddReplicatedSubObject(Entry.Instance);
			}
		}
	}
}

UMyEquipmentInstance* UMyEquipmentManagerComponent::EquipItem(TSubclassOf<UMyEquipmentDefinition> EquipmentDefinition)
{
	UMyEquipmentInstance* Result = nullptr;
	if (EquipmentDefinition != nullptr)
	{
		Result = EquipmentList.AddEntry(EquipmentDefinition);
		if (Result && IsUsingRegisteredSubObjectList())
		{
			AddReplicatedSubObject(Result);
		}
	}
	return Result;
}

void UMyEquipmentManagerComponent::UnequipItem(UMyEquipmentInstance* ItemInstance)
{
	if (ItemInstance != nullptr)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(ItemInstance);
		}
		EquipmentList.RemoveEntry(ItemInstance);
	}
}

UMyEquipmentInstance* UMyEquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UMyEquipmentInstance> InstanceType)
{
	for (const FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (Entry.Instance && Entry.Instance->IsA(InstanceType))
		{
			return Entry.Instance;
		}
	}
	return nullptr;
}

TArray<UMyEquipmentInstance*> UMyEquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<UMyEquipmentInstance> InstanceType) const
{
	TArray<UMyEquipmentInstance*> Results;
	for (const FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (Entry.Instance && Entry.Instance->IsA(InstanceType))
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}
