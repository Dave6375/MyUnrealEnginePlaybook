// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGuardAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"


// Sets default values
ABaseGuardAIController::ABaseGuardAIController()
{
	const auto SenseConfig_Hearing =  CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("SenseConfig_Hearing"));
		SenseConfig_Hearing->DetectionByAffiliation.bDetectEnemies = true;
		SenseConfig_Hearing->DetectionByAffiliation.bDetectFriendlies = true;
		SenseConfig_Hearing->DetectionByAffiliation.bDetectNeutrals = true;
		SenseConfig_Hearing->HearingRange = 2500.f;
		SenseConfig_Hearing->SetStartsEnabled(true);
	
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	PerceptionComponent->ConfigureSense(*SenseConfig_Hearing);
	PerceptionComponent->SetDominantSense(SenseConfig_Hearing->GetSenseImplementation());
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseGuardAIController::OnTargetPerceptionUpdate);
		
		
	
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseGuardAIController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseGuardAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

