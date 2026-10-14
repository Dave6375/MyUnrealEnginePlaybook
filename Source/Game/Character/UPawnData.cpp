// Fill out your copyright notice in the Description page of Project Settings.

#include "UPawnData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UPawnData)

UUPawnData::UUPawnData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PawnClass = nullptr;
	TagRelationshipMapping = nullptr;
	InputConfig = nullptr;
	DefaultCameraMode = nullptr;
}
