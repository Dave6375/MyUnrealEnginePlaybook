// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseNoiseEmitter.h"
#include "Components/ChildActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/StateTreeComponent.h"
#include "Engine/StaticMesh.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values
ABaseNoiseEmitter::ABaseNoiseEmitter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMeshComponent"));
	RootComponent = BaseMeshComponent;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BaseStaticMeshAsset(TEXT("/Game/Fab/Flat_Portal_-_Teleportation_Pad/flat_portal_teleportation_pad/StaticMeshes/flat_portal_teleportation_pad.flat_portal_teleportation_pad"));
	if (BaseStaticMeshAsset.Succeeded())
	{
		BaseMeshComponent->SetStaticMesh(BaseStaticMeshAsset.Object);
		
	}

	DummyActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("DummyActorComponent"));
	DummyActorComponent->SetupAttachment(RootComponent);
	DummyActorComponent->SetRelativeLocation(FVector(0.f, -90.f, 0.f));
	DummyActorComponent->SetRelativeRotation(FRotator(0.f, 0.f, 80.f));
		

	PerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSourceComponent"));
	PerceptionStimuliSourceComponent->RegisterForSense(UAISense_Hearing::StaticClass());

	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));

	static ConstructorHelpers::FClassFinder<AActor> DummyActorClass(TEXT("/Game/Dummy/Dummy"));

	if (DummyActorClass.Succeeded())
	{
		DummyActorComponent->SetChildActorClass(DummyActorClass.Class);
	}
}

// Called when the game starts or when spawned
void ABaseNoiseEmitter::BeginPlay()
{
	Super::BeginPlay();
	
	PerceptionStimuliSourceComponent->RegisterWithPerceptionSystem();
	StateTreeComponent->StartLogic();
	
}

void ABaseNoiseEmitter::EmitNoise()
{
	const auto NoiseRange = MaxNoiseRange + FMath::RandRange(-1.f * NoiseRangeRandomDeviation, NoiseRangeRandomDeviation);
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.f, this, NoiseRange, NoiseTag);
}

// Called every frame
void ABaseNoiseEmitter::Tick(float DeltaTime)
{
	PrimaryActorTick.bCanEverTick = false;
}

