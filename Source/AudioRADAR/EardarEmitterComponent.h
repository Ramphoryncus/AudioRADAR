// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "EardarTypes.h"
#include "EardarEmitterComponent.generated.h"

/**
 * @class UEardarEmitterComponent
 * @brief Marks an actor as a sound source that can be processed by E.A.R.D.A.R.
 *
 * Attach this component to any actor that represents a sound source relevant
 * to the accessibility radar.
 *
 * The emitter can use profile data resolved through its Gameplay Tag, or it can
 * provide local override values for properties such as loudness, range,
 * occlusion behaviour and icon selection.
 */
UCLASS(ClassGroup=(Audio), meta=(BlueprintSpawnableComponent))
class AUDIORADAR_API UEardarEmitterComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /**
     * @brief Constructs the E.A.R.D.A.R. emitter component.
     */
    UEardarEmitterComponent();

    /**
     * @brief Gameplay Tag used to identify the sound emitted by this component.
     *
     * The manager can use this tag to look up a matching profile row from
     * the configured DataTable.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR")
    FGameplayTag SoundTag;

    /**
     * @brief Optional category override for this emitter.
     *
     * When set to a value other than Custom, this value overrides the
     * category supplied by the resolved profile.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR")
    EEardarCategory CategoryOverride = EEardarCategory::Custom;

    /**
     * @brief Optional loudness override for this emitter.
     *
     * A value below 0 indicates that the loudness from the resolved
     * profile should be used instead.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR", meta=(ClampMin="0.0", ClampMax="1.0"))
    float BaseLoudnessOverride = -1.f;

    /**
     * @brief Optional maximum effective range override.
     *
     * A value below 0 indicates that the range from the resolved
     * profile should be used instead.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR", meta=(ClampMin="1.0"))
    float MaxEffectiveRangeOverride = -1.f;

    /**
     * @brief Optional occlusion penalty override.
     *
     * A value below 0 indicates that the occlusion penalty from the resolved
     * profile should be used instead.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR", meta=(ClampMin="0.0", ClampMax="1.0"))
    float OcclusionPenaltyOverride = -1.f;

    /**
     * @brief Determines whether on-screen suppression should be enabled for this emitter.
     *
     * When true, this can override the resolved profile and enable suppression
     * for nearby sound sources that are already visible to the player.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR")
    bool bOnScreenSuppressionOverride = false;

    /**
     * @brief Optional icon identifier override for this emitter.
     *
     * When set, this replaces the icon identifier supplied by the resolved profile.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR")
    FName IconIdOverride;

    /**
     * @brief Optional reference to an AudioComponent associated with this emitter.
     *
     * This can be used for future live audio measurements such as RMS amplitude.
     * The reference may be null.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR")
    TObjectPtr<class UAudioComponent> AudioComponent = nullptr;

    /**
     * @brief Cached profile data associated with this emitter.
     *
     * Intended to store resolved profile values produced from DataTable data
     * and emitter-specific overrides.
     */
    FEardarProfileRow ResolvedProfile;

    /**
     * @brief Returns the world-space location of the sound emitter.
     *
     * @return World-space position used by the manager for distance and bearing calculations.
     */
    UFUNCTION(BlueprintCallable, Category="EARDAR")
    FVector GetEmitterLocation() const;

protected:
    /**
     * @brief Called when the component begins play.
     *
     * Typically used to register the emitter with the E.A.R.D.A.R. manager.
     */
    virtual void BeginPlay() override;

    /**
     * @brief Called when the component is removed from play.
     *
     * Typically used to unregister the emitter from the E.A.R.D.A.R. manager.
     *
     * @param EndPlayReason Reason the component or owning actor is ending play.
     */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};