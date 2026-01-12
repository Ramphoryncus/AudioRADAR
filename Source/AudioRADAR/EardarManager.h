// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
 * AEardarManager
 *
 * The central system that listens for all sound emitters in the world
 * and converts them into "blips" on the accessibility radar (E.A.R.D.A.R.).
 *
 * Each frame:
 *   - Collects nearby emitters
 *   - Computes an "audibility score" for each sound
 *   - Simulates occlusion (blocked by walls, etc.)
 *   - Sends the visible blips to the Radar Widget
 */

UCLASS()
class AUDIORADAR_API AEardarManager : public AActor
{
    GENERATED_BODY()

public:
    AEardarManager();

    // Data table that maps SoundTag/GameplayTags -> FEardarProfileRow definitions
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="EARDAR|Config")
    TObjectPtr<UDataTable> ProfileTable;

    // Global threshold: if computed audibility falls below this, no blip.
    // Minimum audibility value needed for a sound to appear on the radar
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR|Config", meta=(ClampMin="0.0", ClampMax="1.0"))
    float PerceptibilityThreshold = 0.2f;

    // After a loud event (e.g., flashbang), this drops and recovers to 1.0
    // Simulates "temporary hearing damage" (0.1 = deafened, 1.0 = full hearing)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR|Runtime", meta=(ClampMin="0.1", ClampMax="1.0"))
    float HearingSensitivity = 1.0f;

    // How quickly HearingSensitivity returns to 1.0 (seconds to recover ~95%)
    // Time (seconds) for hearing sensitivity to recover after a loud event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR|Config", meta=(ClampMin="0.1"))
    float SensitivityRecoverySeconds = 6.f;

    // Maximum icons shown (to avoid clutter); highest priority wins.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR|Config", meta=(ClampMin="1"))
    int32 MaxVisibleBlips = 24;

    // Reference to your UMG widget (create in BP and set here or expose a setter)
    // Reference to the UMG widget that renders the radar on-screen
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR|UI")
    TSubclassOf<UEardarRadarWidget> RadarWidgetClass;

    // Called by emitter components to register/unregister with this manager
    UFUNCTION(BlueprintCallable, Category="EARDAR|Runtime")
    void RegisterEmitter(UEardarEmitterComponent* Emitter);

    UFUNCTION(BlueprintCallable, Category="EARDAR|Runtime")
    void UnregisterEmitter(UEardarEmitterComponent* Emitter);

    // Call when a flashbang/explosion occurs to simulate temporary hearing loss
    UFUNCTION(BlueprintCallable, Category="EARDAR|Runtime")
    void ApplyLoudEvent(float EventIntensity /*0..1*/);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    // All active sound emitters in the world
    UPROPERTY()
    TArray<TWeakObjectPtr<UEardarEmitterComponent>> Emitters;

    // The currently active radar widget instance
    UPROPERTY()
    TObjectPtr<UEardarRadarWidget> RadarWidgetInstance;

    // Helpers
    // Looks up or merges the emitter’s profile data from the DataTable
    bool ResolveProfile(UEardarEmitterComponent* Emitter, FEardarProfileRow& Out) const;

    // Converts a sound emitter into a visual blip (returns false if inaudible)
    bool ComputeBlip(UEardarEmitterComponent* Emitter, const FEardarProfileRow& Profile, FEardarBlip& OutBlip) const;

    // Performs a line trace to simulate sound occlusion
    // If a wall blocks the path, the sound’s audibility will be reduced
    bool CheckOcclusion(const FVector& From, const FVector& To, float& OcclusionMultiplier) const;
};
