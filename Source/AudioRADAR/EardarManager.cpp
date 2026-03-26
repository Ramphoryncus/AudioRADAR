// Fill out your copyright notice in the Description page of Project Settings.

#include "EardarManager.h"
#include "EardarEmitterComponent.h"
#include "EardarRadarWidget.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

/**
 * @brief Constructs the E.A.R.D.A.R. manager.
 *
 * Enables ticking so the manager can evaluate emitters and update
 * the radar every frame.
 */
AEardarManager::AEardarManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

/**
 * @brief Called when play begins.
 *
 * Creates the radar widget instance and adds it to the viewport,
 * provided a valid widget class has been assigned.
 */
void AEardarManager::BeginPlay()
{
    Super::BeginPlay();

    // Spawn and add the radar widget to viewport if provided.
    if (RadarWidgetClass)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC)
        {
            RadarWidgetInstance = CreateWidget<UEardarRadarWidget>(PC, RadarWidgetClass);
            if (RadarWidgetInstance)
            {
                RadarWidgetInstance->AddToViewport();
            }
        }
    }
}

/**
 * @brief Updates the radar system each frame.
 *
 * This function:
 * - Gradually restores hearing sensitivity after loud events
 * - Collects valid blips from registered emitters
 * - Applies optional on-screen suppression
 * - Limits the number of visible blips by priority
 * - Sends the final blip list to the radar widget
 *
 * @param DeltaSeconds Time elapsed since the previous frame.
 */
void AEardarManager::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Step 1: Gradually restore hearing sensitivity after loud events
    // Recover hearing sensitivity exponentially toward 1.0
    if (HearingSensitivity < 1.0f)
    {
        const float Lambda = DeltaSeconds / FMath::Max(0.01f, SensitivityRecoverySeconds);
        HearingSensitivity = FMath::Min(1.0f, FMath::Lerp(HearingSensitivity, 1.0f, Lambda * 5.f));
    }

    // Step 2: Prepare for blip collection
    // Gather valid blips
    TArray<FEardarBlip> Blips;
    Blips.Reserve(Emitters.Num());

    // Player/camera transform
    AActor* ViewTarget = nullptr;
    FRotator CamRot;
    FVector CamLoc;
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->GetPlayerViewPoint(CamLoc, CamRot);
        ViewTarget = PC->GetPawn();
    }

    // Step 3: Iterate through all emitters
    for (int32 i = Emitters.Num() - 1; i >= 0; --i)
    {
        UEardarEmitterComponent* E = Emitters[i].Get();
        if (!E || !E->GetOwner())
        {
            Emitters.RemoveAtSwap(i);
            continue;
        }

        FEardarProfileRow Profile;
        if (!ResolveProfile(E, Profile))
        {
            continue;
        }

        FEardarBlip Blip;
        if (ComputeBlip(E, Profile, Blip))
        {
            // Optional: on-screen suppression
            if (Profile.bOnScreenSuppression && ViewTarget)
            {
                // Rough heuristic: if within 8m and roughly in front, fade a lot.
                const FVector To = E->GetEmitterLocation() - CamLoc;
                const float Dist = To.Length();
                const float Dot = FVector::DotProduct(To.GetSafeNormal(), CamRot.Vector());
                if (Dist < 800.f && Dot > 0.7f)
                {
                    Blip.Alpha *= 0.2f; // heavy fade
                }
            }

            Blips.Add(Blip);
        }
    }

    // Step 4: Limit number of visible blips (based on priority)
    // Cull by priority if needed
    if (Blips.Num() > MaxVisibleBlips)
    {
        Blips.Sort([](const FEardarBlip& A, const FEardarBlip& B)
        {
            if (A.Priority != B.Priority)
            {
                return A.Priority > B.Priority;
            }
            return A.Alpha > B.Alpha;
        });

        Blips.SetNum(MaxVisibleBlips);
    }

    // Step 5: Send data to UI
    // Push to widget
    if (RadarWidgetInstance)
    {
        RadarWidgetInstance->UpdateBlips(Blips);
    }
}

/**
 * @brief Registers an emitter with the manager.
 *
 * Adds the emitter to the internal list if it is valid and not already present.
 *
 * @param Emitter The emitter component to register.
 */
void AEardarManager::RegisterEmitter(UEardarEmitterComponent* Emitter)
{
    if (!Emitter)
    {
        return;
    }

    Emitters.AddUnique(Emitter);
}

/**
 * @brief Unregisters an emitter from the manager.
 *
 * Removes the emitter from the internal tracking list.
 *
 * @param Emitter The emitter component to unregister.
 */
void AEardarManager::UnregisterEmitter(UEardarEmitterComponent* Emitter)
{
    Emitters.Remove(Emitter);
}

/**
 * @brief Applies the effect of a loud event to hearing sensitivity.
 *
 * Simulates temporary hearing loss by reducing {@link HearingSensitivity}
 * based on the supplied event intensity.
 *
 * @param EventIntensity Strength of the loud event in the range 0..1.
 */
void AEardarManager::ApplyLoudEvent(float EventIntensity)
{
    // Reduce sensitivity (simulate temporary hearing loss), clamp to [0.1,1]
    HearingSensitivity = FMath::Clamp(
        HearingSensitivity - FMath::Clamp(EventIntensity, 0.f, 1.f) * 0.6f,
        0.1f,
        1.0f
    );
}

/**
 * @brief Resolves the profile data for a sound emitter.
 *
 * This function first attempts to load profile data from the data table using
 * the emitter's sound tag. It then applies any emitter-level overrides.
 *
 * @param Emitter The emitter whose profile data should be resolved.
 * @param Out Output profile row populated with resolved values.
 * @return True if a valid profile was produced, otherwise false.
 */
bool AEardarManager::ResolveProfile(UEardarEmitterComponent* Emitter, FEardarProfileRow& Out) const
{
    // 1) Resolve via DataTable + SoundTag if present
    if (ProfileTable && Emitter->SoundTag.IsValid())
    {
        // DataTable rows are named; we’ll assume the row name == tag name for simplicity.
        const FName RowName(*Emitter->SoundTag.ToString());
        if (const FEardarProfileRow* Row = ProfileTable->FindRow<FEardarProfileRow>(RowName, TEXT("EARDAR")))
        {
            Out = *Row;
        }
    }

    // 2) Apply overrides if set
    if (Emitter->CategoryOverride != EEardarCategory::Custom) Out.Category = Emitter->CategoryOverride;
    if (Emitter->BaseLoudnessOverride >= 0.f)               Out.BaseLoudness = Emitter->BaseLoudnessOverride;
    if (Emitter->MaxEffectiveRangeOverride >= 0.f)          Out.MaxEffectiveRangeM = Emitter->MaxEffectiveRangeOverride;
    if (Emitter->OcclusionPenaltyOverride >= 0.f)           Out.OcclusionPenalty = Emitter->OcclusionPenaltyOverride;
    if (!Emitter->IconIdOverride.IsNone())                  Out.IconId = Emitter->IconIdOverride;
    if (Emitter->bOnScreenSuppressionOverride)              Out.bOnScreenSuppression = true;

    // If still default (never set), bail.
    return Out.MaxEffectiveRangeM > 0.f && Out.BaseLoudness > 0.f;
}

/**
 * @brief Checks whether the path between the listener and the sound source is blocked.
 *
 * Performs a line trace between two points to determine whether geometry blocks
 * the sound path. If blocked, the sound can later be treated as muffled or reduced.
 *
 * @param From Start position of the trace, usually the listener location.
 * @param To End position of the trace, usually the sound source location.
 * @param OcclusionMultiplier Output multiplier indicating whether the sound path is occluded.
 * @return True if the path is blocked, otherwise false.
 */
bool AEardarManager::CheckOcclusion(const FVector& From, const FVector& To, float& OcclusionMultiplier) const
{
    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(EardarOcclusion), /*bTraceComplex*/false);

    const bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, From, To, ECC_Visibility, Params);

    // If blocked, return a multiplier < 1. If clear, return 1.
    // If blocked → reduce audibility (e.g. wall between player and sound)
    OcclusionMultiplier = bBlocked ? 0.0f : 1.0f; // we just flag; penalty is applied in ComputeBlip

    // Return TRUE when blocked (i.e. path is blocked / occluded)
    return bBlocked;
}

/**
 * @brief Converts a registered emitter into a radar blip.
 *
 * This function evaluates the emitter using:
 * - Distance-based attenuation
 * - Occlusion
 * - Hearing sensitivity
 * - Source loudness
 *
 * If the resulting audibility score is high enough, the output blip is populated
 * for display on the radar.
 *
 * @param E The emitter being evaluated.
 * @param Profile The resolved profile associated with the emitter.
 * @param OutBlip Output blip populated if the emitter is audible enough.
 * @return True if a blip was generated, otherwise false.
 */
bool AEardarManager::ComputeBlip(UEardarEmitterComponent* E, const FEardarProfileRow& Profile, FEardarBlip& OutBlip) const
{
    const FVector CamLoc = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation();
    const FRotator CamRot = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraRotation();

    const FVector Source = E->GetEmitterLocation();
    const FVector To = Source - CamLoc;
    const float DistM = To.Length() / 100.f; // UE units = cm, convert to meters

    // Skip if too far away
    if (DistM > Profile.MaxEffectiveRangeM)
    {
        return false;
    }

    // Base attenuation: simple linear falloff 1 - d/range (cheap and stable)
    const float Attenuation = FMath::Clamp(1.f - (DistM / Profile.MaxEffectiveRangeM), 0.f, 1.f);

    // Occlusion: line trace; if blocked, we'll apply penalty later.
    // Occlusion check (muffling from walls)
    float OcclusionMultiplier = 1.f;
    if (CheckOcclusion(CamLoc, Source, OcclusionMultiplier))
    {
        // If blocked, reduce audibility by the profile’s penalty
        OcclusionMultiplier = 1.f - Profile.OcclusionPenalty;
    }

    // Optional live RMS from AudioComponent (if you wire it later)
    const float SourcePlaybackRMS = 1.f; // placeholder (1 == use base, wire real meter later)

    // Combine all factors into an audibility score
    float Audibility =
        Profile.BaseLoudness
      * Attenuation
      * OcclusionMultiplier
      * HearingSensitivity
      * SourcePlaybackRMS;

    // Skip if below perceptibility threshold
    if (Audibility < PerceptibilityThreshold)
    {
        return false;
    }

    // Fill out the blip for UI (metadata for radar widget)
    OutBlip.WorldLocation = Source;
    OutBlip.Category = Profile.Category;
    OutBlip.SoundTag = E->SoundTag;
    OutBlip.IconId = Profile.IconId;
    OutBlip.Priority = Profile.Priority;

    // Bearing relative to camera forward. Convert to UI-friendly polar coordinates
    const FVector Fwd = CamRot.Vector();
    const FVector Right = FRotationMatrix(CamRot).GetUnitAxis(EAxis::Y);
    const FVector ToNorm = To.GetSafeNormal();

    const float X = FVector::DotProduct(ToNorm, Right);
    const float Y = FVector::DotProduct(ToNorm, Fwd);
    OutBlip.BearingDeg = FMath::RadiansToDegrees(FMath::Atan2(X, Y)); // -180..180

    // Normalized radius (0..1) based on distance (closer = nearer center)
    OutBlip.NormalizedRadius = FMath::Clamp(DistM / Profile.MaxEffectiveRangeM, 0.f, 1.f);

    // Visual alpha from audibility (remap so values just over threshold aren't too faint)
    const float Low = PerceptibilityThreshold;
    OutBlip.Alpha = FMath::Clamp((Audibility - Low) / (1.f - Low), 0.2f, 1.f);

    return true;
}