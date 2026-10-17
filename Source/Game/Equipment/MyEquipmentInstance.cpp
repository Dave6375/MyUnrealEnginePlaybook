// Fill out your copyright notice in the Description page of Project Settings.

#include "MyEquipmentInstance.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyEquipmentInstance)

UMyEquipmentInstance::UMyEquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Instigator = nullptr;
}

void UMyEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMyEquipmentInstance, Instigator);
	DOREPLIFETIME(UMyEquipmentInstance, SpawnedActors);
}

UWorld* UMyEquipmentInstance::GetWorld() const
{
	if (const APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}
	return nullptr;
}

APawn* UMyEquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn* UMyEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = GetPawn();
	if (Result && Result->IsA(PawnType))
	{
		return Result;
	}
	return nullptr;
}

void UMyEquipmentInstance::SpawnEquipmentActors(const TArray<FEquipmentActorActorToSpawn>& ActorsToSpawn)
{
	if (APawn* OwningPawn = GetPawn())
	{
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			AttachTarget = Char->GetMesh();
		}

		for (const FEquipmentActorActorToSpawn& Entry : ActorsToSpawn)
		{
			if (IsValid(Entry.ActorToSpawn))
			{
				AActor* NewActor = OwningPawn->GetWorld()->SpawnActorDeferred<AActor>(
					Entry.ActorToSpawn,
					FTransform::Identity,
					OwningPawn,
					OwningPawn,
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);

				if (NewActor)
				{
					NewActor->FinishSpawning(FTransform::Identity, true);
					NewActor->SetActorRelativeTransform(Entry.AttachTransform);
					if (AttachTarget)
					{
						NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, Entry.AttachSocket);
					}
					SpawnedActors.Add(NewActor);
				}
			}
		}
	}
}

void UMyEquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	SpawnedActors.Reset();
}

void UMyEquipmentInstance::OnEquipped()
{
	K2_OnEquipped();
}

void UMyEquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
}

void UMyEquipmentInstance::OnRep_Instigator()
{
}

void UMyEquipmentInstance::OnRep_SpawnedActors()
{
}
