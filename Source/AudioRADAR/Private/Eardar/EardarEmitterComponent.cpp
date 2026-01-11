// Fill out your copyright notice in the Description page of Project Settings.


#include "Eardar/EardarEmitterComponent.h"
#include "Components/AudioComponent.h"

UEardarEmitterComponent::UEardarEmitterComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // manager will tick, not emitters
}

void UEardarEmitterComponent::BeginPlay()
{
	Super::BeginPlay();
	// Nothing special; the manager will resolve the profile at runtime.
}

FVector UEardarEmitterComponent::GetEmitterLocation() const
{
	// Prefer the AudioComponent’s location if supplied (spatialized sounds),
	// otherwise use the owning actor location.
	if (AudioComponent)
	{
		return AudioComponent->GetComponentLocation();
	}
	return GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
}


