// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseNoiseEmitter.generated.h"

class UAIPerceptionStimuliSourceComponent;
class UStateTreeComponent;
class UStaticMeshComponent;
class UChildActorComponent;

UCLASS(Blueprintable)
class GAME_API ABaseNoiseEmitter : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* BaseMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UChildActorComponent* DummyActorComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UAIPerceptionStimuliSourceComponent* PerceptionStimuliSourceComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStateTreeComponent* StateTreeComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise Generation")
	float MaxNoiseRange = 3000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise Generation")
	float NoiseRangeRandomDeviation = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise Generation")
	FName NoiseTag = "EmitterNoise";
	
	UFUNCTION(BlueprintCallable)
	void EmitNoise();

public:
	// Sets default values for this actor's properties
	ABaseNoiseEmitter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
