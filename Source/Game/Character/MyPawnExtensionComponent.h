// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "MyPawnExtensionComponent.generated.h"

namespace EEndPlayReason { enum Type : int; }

class AActor;
class APawn;
class AController;
class UGameFrameworkComponentManager;
class UMyAbilitySystemComponent;
class UUPawnData; // Adjust to your PawnData class name (e.g. UUPawnData or UMyPawnData)
class UObject;
struct FActorInitStateChangedParams;
struct FGameplayTag;

/**
 * UMyPawnExtensionComponent
 *
 * Component that coordinates modular initialization and ability system setup across Pawn components.
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GAME_API UMyPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UMyPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	/** Feature name used by GameFrameworkComponentManager */
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface Interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface Interface

	/** Returns the pawn extension component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Pawn")
	static UMyPawnExtensionComponent* GetPawnExtensionComponent(const AActor* Actor)
	{
		return Actor ? Actor->FindComponentByClass<UMyPawnExtensionComponent>() : nullptr;
	}

	/** Gets the pawn data, cast to the desired type */
	template <class T>
	const T* GetPawnData() const
	{
		return Cast<T>(PawnData);
	}

	/** Sets the current pawn data (Server only) */
	void SetPawnData(const UUPawnData* NewData);

	/** Gets the current ability system component */
	UFUNCTION(BlueprintPure, Category = "Pawn")
	UMyAbilitySystemComponent* GetMyAbilitySystemComponent() const { return AbilitySystemComponent; }

	/** Sets this pawn as the avatar actor for the ability system */
	void InitializeAbilitySystem(UMyAbilitySystemComponent* InASC, AActor* InOwnerActor);

	/** Removes this pawn as the avatar actor from the ability system */
	void UninitializeAbilitySystem();

	/** Called when controller changes / possession occurs */
	void HandleControllerChanged();

	/** Called when PlayerState replicates to client */
	void HandlePlayerStateReplicated();

	/** Called when input component is set up */
	void SetupPlayerInputComponent();

	/** Register callback when ASC is initialized */
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	/** Register callback when ASC is uninitialized */
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_PawnData();

	/** Delegate fired when our pawn becomes the ability system's avatar actor. */
	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	/** Delegate fired when our pawn is removed as the ability system's avatar actor. */
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

	/** Pawn data used to configure the pawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_PawnData, Category = "Pawn")
	TObjectPtr<const UUPawnData> PawnData;

	/** Cached pointer to ability system component */
	UPROPERTY(Transient)
	TObjectPtr<UMyAbilitySystemComponent> AbilitySystemComponent;
};