// Fill out your copyright notice in the Description page of Project Settings.

#include "MyHeroComponent.h"
#include "MyPawnExtensionComponent.h"
#include "UPawnData.h"
#include "Game/Input/UInputConfig.h"
#include "Game/GameplayAbilitySystem/MyAbilitySystemComponent.h"
#include "Game/MyGameplayTags.h"

#include "Components/GameFrameworkComponentManager.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyHeroComponent)

const FName UMyHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName UMyHeroComponent::NAME_ActorFeatureName("Hero");

UMyHeroComponent::UMyHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	AbilityCameraMode = nullptr;
	bReadyToBindInputs = false;
}

void UMyHeroComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf(Pawn != nullptr, TEXT("MyHeroComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> HeroComponents;
	Pawn->GetComponents(UMyHeroComponent::StaticClass(), HeroComponents);
	ensureAlwaysMsgf(HeroComponents.Num() == 1, TEXT("Only one MyHeroComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	RegisterInitStateFeature();
}

void UMyHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(UMyPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	ensure(TryToChangeInitState(MyGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UMyHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

bool UMyHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == MyGameplayTags::InitState_Spawned)
	{
		return (Pawn != nullptr);
	}

	if (CurrentState == MyGameplayTags::InitState_Spawned && DesiredState == MyGameplayTags::InitState_DataAvailable)
	{
		const UMyPawnExtensionComponent* PawnExtComp = UMyPawnExtensionComponent::GetPawnExtensionComponent(Pawn);
		if (!PawnExtComp || !PawnExtComp->GetPawnData<UUPawnData>())
		{
			return false;
		}

		return true;
	}
	else if (CurrentState == MyGameplayTags::InitState_DataAvailable && DesiredState == MyGameplayTags::InitState_DataInitialized)
	{
		const UMyPawnExtensionComponent* PawnExtComp = UMyPawnExtensionComponent::GetPawnExtensionComponent(Pawn);
		if (!PawnExtComp || !PawnExtComp->GetPawnData<UUPawnData>())
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

		return Manager->HasFeatureReachedInitState(Pawn, UMyPawnExtensionComponent::NAME_ActorFeatureName, MyGameplayTags::InitState_DataAvailable);
	}
	else if (CurrentState == MyGameplayTags::InitState_DataInitialized && DesiredState == MyGameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UMyHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == MyGameplayTags::InitState_DataAvailable && DesiredState == MyGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		if (!Pawn)
		{
			return;
		}

		const UMyPawnExtensionComponent* PawnExtComp = UMyPawnExtensionComponent::GetPawnExtensionComponent(Pawn);
		const UUPawnData* PawnData = PawnExtComp ? PawnExtComp->GetPawnData<UUPawnData>() : nullptr;

		if (Pawn->InputComponent != nullptr)
		{
			InitializePlayerInput(Pawn->InputComponent);
		}

		if (PawnData && PawnData->DefaultCameraMode)
		{
			AbilityCameraMode = PawnData->DefaultCameraMode;
		}
	}
}

void UMyHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UMyPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == MyGameplayTags::InitState_DataAvailable || Params.FeatureState == MyGameplayTags::InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

void UMyHeroComponent::CheckDefaultInitialization()
{
	const TArray<FGameplayTag> StateChain = {
		MyGameplayTags::InitState_Spawned,
		MyGameplayTags::InitState_DataAvailable,
		MyGameplayTags::InitState_DataInitialized,
		MyGameplayTags::InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
}

void UMyHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = Pawn->GetController<APlayerController>();
	if (!PC)
	{
		return;
	}

	if (const ULocalPlayer* LP = PC->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->ClearAllMappings();
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, DefaultInputPriority);
			}
		}
	}

	const UMyPawnExtensionComponent* PawnExtComp = UMyPawnExtensionComponent::GetPawnExtensionComponent(Pawn);
	const UUPawnData* PawnData = PawnExtComp ? PawnExtComp->GetPawnData<UUPawnData>() : nullptr;

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent && PawnData && PawnData->InputConfig)
	{
		const UUInputConfig* InputConfig = PawnData->InputConfig;

		// Bind Native Actions
		if (const UInputAction* Action = InputConfig->FindNativeInputActionForTag(MyGameplayTags::InputTag_Move, false))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, this, &UMyHeroComponent::Input_Move);
		}

		if (const UInputAction* Action = InputConfig->FindNativeInputActionForTag(MyGameplayTags::InputTag_Look_Mouse, false))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, this, &UMyHeroComponent::Input_LookMouse);
		}

		if (const UInputAction* Action = InputConfig->FindNativeInputActionForTag(MyGameplayTags::InputTag_Look_Stick, false))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, this, &UMyHeroComponent::Input_LookStick);
		}

		if (const UInputAction* Action = InputConfig->FindNativeInputActionForTag(MyGameplayTags::InputTag_Crouch, false))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, this, &UMyHeroComponent::Input_Crouch);
		}

		if (const UInputAction* Action = InputConfig->FindNativeInputActionForTag(MyGameplayTags::InputTag_Jump, false))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, this, &UMyHeroComponent::Input_Jump);
		}

		// Bind Ability Actions
		for (const FGameInputAction& Action : InputConfig->AbilityInputActions)
		{
			if (Action.InputAction && Action.InputTag.IsValid())
			{
				EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Triggered, this, &UMyHeroComponent::Input_AbilityInputTagPressed, Action.InputTag);
				EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Completed, this, &UMyHeroComponent::Input_AbilityInputTagReleased, Action.InputTag);
			}
		}
	}

	bReadyToBindInputs = true;

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UMyHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UMyPawnExtensionComponent* PawnExtComp = UMyPawnExtensionComponent::GetPawnExtensionComponent(Pawn))
		{
			if (UMyAbilitySystemComponent* MyASC = PawnExtComp->GetMyAbilitySystemComponent())
			{
				MyASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UMyHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UMyPawnExtensionComponent* PawnExtComp = UMyPawnExtensionComponent::GetPawnExtensionComponent(Pawn))
		{
			if (UMyAbilitySystemComponent* MyASC = PawnExtComp->GetMyAbilitySystemComponent())
			{
				MyASC->AbilityInputTagReleased(InputTag);
			}
		}
	}
}

void UMyHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UMyHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (Pawn)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();

		if (Value.X != 0.0f)
		{
			Pawn->AddControllerYawInput(Value.X);
		}

		if (Value.Y != 0.0f)
		{
			Pawn->AddControllerPitchInput(Value.Y);
		}
	}
}

void UMyHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (Pawn)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();

		const UWorld* World = GetWorld();
		check(World);

		if (Value.X != 0.0f)
		{
			Pawn->AddControllerYawInput(Value.X * World->GetDeltaSeconds());
		}

		if (Value.Y != 0.0f)
		{
			Pawn->AddControllerPitchInput(Value.Y * World->GetDeltaSeconds());
		}
	}
}

void UMyHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	if (ACharacter* Character = GetPawn<ACharacter>())
	{
		if (Character->bIsCrouched)
		{
			Character->UnCrouch();
		}
		else
		{
			Character->Crouch();
		}
	}
}

void UMyHeroComponent::Input_Jump(const FInputActionValue& InputActionValue)
{
	if (ACharacter* Character = GetPawn<ACharacter>())
	{
		if (InputActionValue.Get<bool>())
		{
			Character->Jump();
		}
		else
		{
			Character->StopJumping();
		}
	}
}

void UMyHeroComponent::SetAbilityCameraMode(TSubclassOf<UGameCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void UMyHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}

void UMyHeroComponent::AddAdditionalInputConfig(const UUInputConfig* InputConfig)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	UEnhancedInputComponent* EnhancedInputComponent = Pawn->FindComponentByClass<UEnhancedInputComponent>();
	if (EnhancedInputComponent && InputConfig)
	{
		for (const FGameInputAction& Action : InputConfig->AbilityInputActions)
		{
			if (Action.InputAction && Action.InputTag.IsValid())
			{
				EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Triggered, this, &UMyHeroComponent::Input_AbilityInputTagPressed, Action.InputTag);
				EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Completed, this, &UMyHeroComponent::Input_AbilityInputTagReleased, Action.InputTag);
			}
		}
	}
}

void UMyHeroComponent::RemoveAdditionalInputConfig(const UUInputConfig* InputConfig)
{
}

bool UMyHeroComponent::IsReadyToBindInput() const
{
	return bReadyToBindInputs;
}

