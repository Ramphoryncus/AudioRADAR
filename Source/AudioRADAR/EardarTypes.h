// Minimal, engine-agnostic types and data definitions for E.A.R.D.A.R.
// Put in Public/ so other modules/classes can include it.

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "EardarTypes.generated.h"

// High-level sound categories you care about. You can also rely purely on GameplayTags.
UENUM(BlueprintType)
enum class EEardarCategory : uint8
{
    Footstep     UMETA(DisplayName="Footstep"),
    Gunshot      UMETA(DisplayName="Gunshot"),
    Explosion    UMETA(DisplayName="Explosion"),
    Vehicle      UMETA(DisplayName="Vehicle"),
    Voice        UMETA(DisplayName="Voice"),
    Ambient      UMETA(DisplayName="Ambient"),
    Custom       UMETA(DisplayName="Custom")
};

// Row for a DataTable that configures how each category/tag behaves on the radar.
// You can author this in a CSV and create a DataTable asset from it.
USTRUCT(BlueprintType)
struct FEardarProfileRow : public FTableRowBase
{
    GENERATED_BODY()

    // Optional tag lookup (e.g., "Sound.Explosion", "Sound.Footstep")
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag SoundTag;

    // Category (redundant but handy for quick switches)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EEardarCategory Category = EEardarCategory::Custom;

    // Proxy for "how loud in general" (0..1)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0", ClampMax="1.0"))
    float BaseLoudness = 0.7f;

    // Hard cut-off range for icon eligibility (meters)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1.0"))
    float MaxEffectiveRangeM = 150.f;

    // How much occlusion reduces the score (0 = no effect, 1 = fully muted)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0", ClampMax="1.0"))
    float OcclusionPenalty = 0.35f;

    // Time the blip lingers after falling below threshold
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0"))
    float PersistenceSeconds = 0.6f;

    // Used when too many icons compete
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Priority = 50;

    // If true, hide/fade when on-screen & close (because player can already see it)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bOnScreenSuppression = true;

    // Name of icon in your widget (you can map this to a texture/sprite in BP)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName IconId = TEXT("Default");
};

// Data passed to the UI each frame for a single blip
USTRUCT(BlueprintType)
struct FEardarBlip
{
    GENERATED_BODY()

    // World-space info (if your widget wants it)
    UPROPERTY(BlueprintReadOnly) FVector WorldLocation = FVector::ZeroVector;

    // Polar UI info (computed relative to the player/camera)
    UPROPERTY(BlueprintReadOnly) float BearingDeg = 0.f;  // -180..180 (0 = forward)
    UPROPERTY(BlueprintReadOnly) float NormalizedRadius = 0.f; // 0 center .. 1 outer ring

    // Visuals
    UPROPERTY(BlueprintReadOnly) FName IconId = TEXT("Default");
    UPROPERTY(BlueprintReadOnly) float Alpha = 1.f;       // opacity based on audibility
    UPROPERTY(BlueprintReadOnly) int32 Priority = 0;      // for sorting

    // Bookkeeping
    UPROPERTY(BlueprintReadOnly) FGameplayTag SoundTag;
    UPROPERTY(BlueprintReadOnly) EEardarCategory Category = EEardarCategory::Custom;
};
