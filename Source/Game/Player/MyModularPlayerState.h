// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "MyModularPlayerState.generated.h"

class UMyAbilitySystemComponent;
class UUPawnData;

/**
 * AMyModularPlayerState
 *
 * Base player state class following Lyra's architecture.
 */
UCLASS()
class GAME_API AMyModularPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AMyModularPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UMyAbilitySystemComponent* GetMyAbilitySystemComponent() const { return AbilitySystemComponent; }

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UUPawnData* InPawnData);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_PawnData();

private:
	UPROPERTY(VisibleAnywhere, Category = "PlayerState")
	TObjectPtr<UMyAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UUPawnData> PawnData;
};
