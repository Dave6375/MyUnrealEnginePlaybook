// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAttributeSet.h"
#include "Net/UnrealNetwork.h"

UMeleeAttributeSet::UMeleeAttributeSet()
{
	AttackPower = 10.f;
	Defense = 5.f;
	AttackSpeed = 1.f;
}

void UMeleeAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMeleeAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMeleeAttributeSet, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMeleeAttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
}
