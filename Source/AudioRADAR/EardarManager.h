// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "EardarTypes.h"
#include "EardarManager.generated.h"

// Forward declare
class UEardarEmitterComponent;
class UEardarRadarWidget;

/**
 * @class AEardarManager
 * @brief Central manager for the E.A.R.D.A.R. accessibility radar system.
 *
 * This actor listens for registered sound emitters in the world and converts
 * them into visual radar blips for the player.
 *
 * Each frame, the manager:
 * - Collects nearby emitters
 * - Computes an audibility score for each sound
 * - Simulates occlusion caused by level geometry
 * - Sends the visible blips to the radar widget
 */
UCLASS()
class AUDIORADAR_API AEardarManager : public AActor
{
    GENERATED_BODY()

public:
    /**
     * @brief Constructs the E.A.R.D.A.R. manager.
     */
    AEardarManager();

    /**
     * @brief Data table that maps sound or gameplay tags to radar profile definitions.
     *
     * Used to look up {@link FEardarProfileRow} data for each emitter.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="EARDAR|Config")
    TObjectPtr<UDataTable> ProfileTable;

    /**
     * @brief Minimum audibility value required for a sound to appear on the radar.
     *
     * If a computed audibility score falls below this threshold, no blip is generated.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR|Config", meta=(ClampMin="0.0", ClampMax="1.0"))
    float PerceptibilityThreshold = 0.2f;

    /**
     * @brief Current hearing sensitivity multiplier.
     *
     * This value temporarily drops after a loud event such as a flashbang or explosion,
     * then gradually recovers back to 1.0.
     *
     * A value of 0.1 simulates extreme hearing loss, while 1.0 represents full hearing.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR|Runtime", meta=(ClampMin="0.1", ClampMax="1.0"))
    float HearingSensitivity = 1.0f;

    /**
     * @brief Time in seconds for hearing sensitivity to recover after a loud event.
     *
     * Controls how quickly {@link HearingSensitivity} returns toward 1.0.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR|Config", meta=(ClampMin="0.1"))
    float SensitivityRecoverySeconds = 6.f;

    /**
     * @brief Maximum number of radar blips visible at once.
     *
     * This helps prevent UI clutter by limiting the number of displayed icons.
     * Higher-priority blips are shown first.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR|Config", meta=(ClampMin="1"))
    int32 MaxVisibleBlips = 24;

    /**
     * @brief Widget class used to render the radar on screen.
     *
     * This is typically assigned in Blueprint.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR|UI")
    TSubclassOf<UEardarRadarWidget> RadarWidgetClass;

    /**
     * @brief Registers an emitter with the manager.
     *
     * Registered emitters are tracked and evaluated each frame for radar display.
     *
     * @param Emitter The emitter component to register.
     */
    UFUNCTION(BlueprintCallable, Category="EARDAR|Runtime")
    void RegisterEmitter(UEardarEmitterComponent* Emitter);

    /**
     * @brief Unregisters an emitter from the manager.
     *
     * Removes the emitter from the active tracking list.
     *
     * @param Emitter The emitter component to unregister.
     */
    UFUNCTION(BlueprintCallable, Category="EARDAR|Runtime")
    void UnregisterEmitter(UEardarEmitterComponent* Emitter);

    /**
     * @brief Applies the effect of a loud event to the player's hearing sensitivity.
     *
     * This simulates temporary hearing loss caused by events such as explosions
     * or flashbangs.
     *
     * @param EventIntensity Strength of the loud event in the range 0..1.
     */
    UFUNCTION(BlueprintCallable, Category="EARDAR|Runtime")
    void ApplyLoudEvent(float EventIntensity /*0..1*/);

protected:
    /**
     * @brief Called when the actor begins play.
     */
    virtual void BeginPlay() override;

    /**
     * @brief Called every frame.
     *
     * @param DeltaSeconds Time elapsed since the previous frame.
     */
    virtual void Tick(float DeltaSeconds) override;

private:
    /**
     * @brief All active sound emitters currently registered in the world.
     */
    UPROPERTY()
    TArray<TWeakObjectPtr<UEardarEmitterComponent>> Emitters;

    /**
     * @brief The currently active radar widget instance.
     */
    UPROPERTY()
    TObjectPtr<UEardarRadarWidget> RadarWidgetInstance;

    /**
     * @brief Resolves profile data for a given emitter.
     *
     * Looks up or merges the emitter's profile data from the data table.
     *
     * @param Emitter The emitter whose profile data should be resolved.
     * @param Out Output profile row populated with the resolved data.
     * @return True if the profile was resolved successfully, otherwise false.
     */
    bool ResolveProfile(UEardarEmitterComponent* Emitter, FEardarProfileRow& Out) const;

    /**
     * @brief Converts a sound emitter into a radar blip.
     *
     * Calculates whether the sound is audible enough to appear on the radar and,
     * if so, fills in the output blip data.
     *
     * @param Emitter The emitter being evaluated.
     * @param Profile The resolved profile data for the emitter.
     * @param OutBlip Output blip populated if the computation succeeds.
     * @return True if a valid blip was generated, otherwise false.
     */
    bool ComputeBlip(UEardarEmitterComponent* Emitter, const FEardarProfileRow& Profile, FEardarBlip& OutBlip) const;

    /**
     * @brief Performs a line trace to simulate sound occlusion.
     *
     * If geometry blocks the path between the source and the listener,
     * the sound's audibility is reduced through the occlusion multiplier.
     *
     * @param From Start position of the trace.
     * @param To End position of the trace.
     * @param OcclusionMultiplier Output multiplier applied to audibility.
     * @return True if the path is blocked, otherwise false.
     */
    bool CheckOcclusion(const FVector& From, const FVector& To, float& OcclusionMultiplier) const;
};
