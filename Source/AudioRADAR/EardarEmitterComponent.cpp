// Fill out your copyright notice in the Description page of Project Settings.


#include "EardarEmitterComponent.h"
#include "EardarManager.h"

#include "Components/AudioComponent.h"
#include "EngineUtils.h"

UEardarEmitterComponent::UEardarEmitterComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // manager will tick, not emitters
}

void UEardarEmitterComponent::BeginPlay()
{
	Super::BeginPlay();
	/*
	* Find the EARDAR Manager that has been placed in the current level.
	*
	* TActorIterator searches through Actors of the requested type.
	* In our prototype we expect only ONE AEardarManager in the level,
	* so once we find the first one we register this emitter and stop.
	*/
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<AEardarManager> It(World); It; ++It)
		{
			AEardarManager* Manager = *It;

			if (Manager)
			{
				/*
				 * Tell the manager:
				 *
				 * "This component is an EARDAR sound source.
				 *  Please include me when calculating radar blips."
				 */
				Manager->RegisterEmitter(this);

				// We only need one manager for this prototype.
				break;
			}
		}
	}
}

void UEardarEmitterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	/*
	 * Remove this emitter from the manager when the Actor/component
	 * leaves the world.
	 *
	 * This prevents the manager retaining references to objects that
	 * have been destroyed.
	 */
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<AEardarManager> It(World); It; ++It)
		{
			AEardarManager* Manager = *It;

			if (Manager)
			{
				Manager->UnregisterEmitter(this);
				break;
			}
		}
	}

	Super::EndPlay(EndPlayReason);
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


