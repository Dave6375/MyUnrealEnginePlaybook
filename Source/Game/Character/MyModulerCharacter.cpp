// Fill out your copyright notice in the Description page of Project Settings.


#include "MyModulerCharacter.h"


// Sets default values
AMyModulerCharacter::AMyModulerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyModulerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyModulerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyModulerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

