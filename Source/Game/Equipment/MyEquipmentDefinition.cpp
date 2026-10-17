// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEquipmentDefinition.h"
#include "MyEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyEquipmentDefinition)

UMyEquipmentDefinition::UMyEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = UMyEquipmentInstance::StaticClass();
}
