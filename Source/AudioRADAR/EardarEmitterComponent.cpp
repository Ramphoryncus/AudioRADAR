// Fill out your copyright notice in the Description page of Project Settings.

#include "EardarEmitterComponent.h"
#include "EardarManager.h"

#include "Components/AudioComponent.h"
#include "EngineUtils.h"

/**
 * @brief Constructs the E.A.R.D.A.R. emitter component.
 *
 * Disables ticking on the emitter because runtime processing is handled
 * centrally by the E.A.R.D.A.R. manager.
 */
UEardarEmitterComponent::UEardarEmitterComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

/**
 * @brief Called when the component begins play.
 *
 * Searches the current world for an AEardarManager instance and registers
 * this emitter with the first manager found.
 *
 * The current prototype assumes that only one E.A.R.D.A.R. manager is
 * present in the level.
 */
void UEardarEmitterComponent::BeginPlay()
{
    Super::BeginPlay();

    /*
     * Find the E.A.R.D.A.R. manager placed in the current level.
     *
     * TActorIterator searches through actors of the requested type.
     * The prototype expects a single AEardarManager instance, so the
     * search stops after the first valid manager is found.
     */
    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<AEardarManager> It(World); It; ++It)
        {
            AEardarManager* Manager = *It;

            if (Manager)
            {
                // Register this component as an active E.A.R.D.A.R. sound source.
                Manager->RegisterEmitter(this);

                // Only one manager is required for the current prototype.
                break;
            }
        }
    }
}

/**
 * @brief Called when the component or owning actor leaves play.
 *
 * Searches for the E.A.R.D.A.R. manager and unregisters this emitter so
 * that the manager does not continue tracking an object that is being removed
 * from the world.
 *
 * @param EndPlayReason Reason the component or owning actor is ending play.
 */
void UEardarEmitterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    /*
     * Remove this emitter from the manager when the actor or component
     * leaves the world.
     *
     * This prevents the manager from retaining references to objects that
     * are no longer active.
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

/**
 * @brief Returns the world-space position used for this sound emitter.
 *
 * If an AudioComponent has been assigned, its component location is used so
 * that the radar can follow the actual spatialised sound source. Otherwise,
 * the owning actor's location is returned.
 *
 * @return World-space location of the emitter, or FVector::ZeroVector if no
 * valid AudioComponent or owning actor exists.
 */
FVector UEardarEmitterComponent::GetEmitterLocation() const
{
    // Prefer the AudioComponent's location if supplied for spatialised sounds.
    if (AudioComponent)
    {
        return AudioComponent->GetComponentLocation();
    }

    // Fall back to the owning actor's location.
    return GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
};

