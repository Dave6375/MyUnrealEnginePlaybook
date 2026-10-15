// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameCameraMode.h"
#include "MyHeroComponent.generated.h"

namespace EEndPlayReason { enum Type : int; }

class UGameFrameworkComponentManager;
class UInputComponent;
class UGameCameraMode;
class UUInputConfig;
class UInputMappingContext;
class UObject;
class APawn;
class APlayerController;
class UMyPawnExtensionComponent;
class UMyAbilitySystemComponent;
struct FActorInitStateChangedParams;
struct FGameplayTag;
struct FInputActionValue;

/**
 * UMyHeroComponent
 *
 * Component that sets up input and camera handling for player-controlled pawns.
 * This depends on a PawnExtensionComponent to coordinate initialization.
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GAME_API UMyHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UMyHeroComponent(const FObjectInitializer& ObjectInitializer);

	/** Returns the hero component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Hero")
	static UMyHeroComponent* FindHeroComponent(const AActor* Actor) { return Actor ? Actor->FindComponentByClass<UMyHeroComponent>() : nullptr; }

	/** Overrides the camera from an active gameplay ability */
	void SetAbilityCameraMode(TSubclassOf<UGameCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle);

	/** Clears the camera override if it is set */
	void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);

	/** Adds mode-specific input config */
	void AddAdditionalInputConfig(const UUInputConfig* InputConfig);

	/** Remove a mode-specific input config if it has been added */
	void RemoveAdditionalInputConfig(const UUInputConfig* InputConfig);

	/** True if this is controlled by a real player and has progressed far enough in initialization where additional input bindings can be added */
	bool IsReadyToBindInput() const;

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	/** The name of the extension event sent via GameFrameworkComponentManager when ability inputs are ready and bindings can be added */
	static const FName NAME_BindInputsNow;

	/** The name of this component-implemented feature */
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);
	void Input_Crouch(const FInputActionValue& InputActionValue);
	void Input_Jump(const FInputActionValue& InputActionValue);

protected:
	/** Camera mode set by an ability */
	UPROPERTY()
	TSubclassOf<UGameCameraMode> AbilityCameraMode;

	/** Spec handle of ability that set the camera mode */
	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;

	/** Input mapping context to apply when initializing input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** Input priority for the default mapping context */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	int32 DefaultInputPriority = 0;

	/** True when player input bindings have been applied */
	bool bReadyToBindInputs;
};
