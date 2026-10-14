// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPawnExtensionComponent.h"
#include "Game/GameplayAbilitySystem/MyAbilitySystemComponent.h"
#include "UPawnData.h"
#include "Game/MyGameplayTags.h"

#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagsManager.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyPawnExtensionComponent)

const FName UMyPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UMyPawnExtensionComponent::UMyPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	PawnData = nullptr;
	AbilitySystemComponent = nullptr;
}

void UMyPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMyPawnExtensionComponent, PawnData);
}

void UMyPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf(Pawn != nullptr, TEXT("MyPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> ExtensionComponents;
	Pawn->GetComponents(UMyPawnExtensionComponent::StaticClass(), ExtensionComponents);
	ensureAlwaysMsgf(ExtensionComponents.Num() == 1, TEXT("Only one MyPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	// Register with GameFrameworkComponentManager init state feature system
	RegisterInitStateFeature();
}

void UMyPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for state changes across all modular features on this actor
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// Request transition to Spawned state
	ensure(TryToChangeInitState(MyGameplayTags::InitState_Spawned));

	CheckDefaultInitialization();
}

void UMyPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UMyPawnExtensionComponent::SetPawnData(const UUPawnData* NewData)
{
	check(NewData);

	APawn* Pawn = GetPawnChecked<APawn>();

	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogTemp, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."), *GetNameSafe(NewData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
		return;
	}

	PawnData = NewData;
	Pawn->ForceNetUpdate();

	CheckDefaultInitialization();
}

void UMyPawnExtensionComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}

void UMyPawnExtensionComponent::InitializeAbilitySystem(UMyAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	if (ExistingAvatar != nullptr && ExistingAvatar != Pawn)
	{
		// Detach previous avatar if lag caused a new pawn to possess before cleanup
		ensure(!ExistingAvatar->HasAuthority());

		if (UMyPawnExtensionComponent* OtherExtension = GetPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtension->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	if (PawnData && PawnData->TagRelationshipMapping)
	{
		AbilitySystemComponent->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
	}

	OnAbilitySystemInitialized.Broadcast();
}

void UMyPawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		AbilitySystemComponent->CancelAbilities(nullptr, nullptr);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void UMyPawnExtensionComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UninitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}

	CheckDefaultInitialization();
}

void UMyPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UMyPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void UMyPawnExtensionComponent::CheckDefaultInitialization()
{
	// Progress any other feature components that might be waiting
	CheckDefaultInitializationForImplementers();

	const TArray<FGameplayTag> StateChain = {
		MyGameplayTags::InitState_Spawned,
		MyGameplayTags::InitState_DataAvailable,
		MyGameplayTags::InitState_DataInitialized,
		MyGameplayTags::InitState_GameplayReady
	};

	// Attempt to step through the initialization chain
	ContinueInitStateChain(StateChain);
}

bool UMyPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == MyGameplayTags::InitState_Spawned)
	{
		return (Pawn != nullptr);
	}

	if (CurrentState == MyGameplayTags::InitState_Spawned && DesiredState == MyGameplayTags::InitState_DataAvailable)
	{
		if (!PawnData)
		{
			return false;
		}

		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (bHasAuthority || bIsLocallyControlled)
		{
			if (!Pawn->GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == MyGameplayTags::InitState_DataAvailable && DesiredState == MyGameplayTags::InitState_DataInitialized)
	{
		return Manager->HaveAllFeaturesReachedInitState(Pawn, MyGameplayTags::InitState_DataAvailable);
	}
	else if (CurrentState == MyGameplayTags::InitState_DataInitialized && DesiredState == MyGameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UMyPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	// Other feature components (like HeroComponent) bind to this transition to perform their setup
}

void UMyPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == MyGameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void UMyPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UMyPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}
