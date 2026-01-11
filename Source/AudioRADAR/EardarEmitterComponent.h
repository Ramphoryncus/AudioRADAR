// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "EardarTypes.h"
#include "EardarEmitterComponent.generated.h"

/**
 * Attach this to any actor that emits a sound you care about.
 * You can set its profile via DataTable lookup (by SoundTag) or override fields here.
 */
UCLASS(ClassGroup=(Audio), meta=(BlueprintSpawnableComponent))
class AUDIORADAR_API UEardarEmitterComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEardarEmitterComponent();

    // Optional gameplay tag to look up a profile row.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR")
    FGameplayTag SoundTag;

    // If set, the manager can find the matching row here.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR")
    EEardarCategory CategoryOverride = EEardarCategory::Custom;

    // Manual overrides (if you don't want to use a DataTable for this emitter)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR", meta=(ClampMin="0.0", ClampMax="1.0"))
    float BaseLoudnessOverride = -1.f; // <0 means "use table"

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR", meta=(ClampMin="1.0"))
    float MaxEffectiveRangeOverride = -1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR", meta=(ClampMin="0.0", ClampMax="1.0"))
    float OcclusionPenaltyOverride = -1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR")
    bool bOnScreenSuppressionOverride = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR")
    FName IconIdOverride;

    // Optional: hook to your AudioComponent if you want live RMS (can be null)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EARDAR")
    TObjectPtr<class UAudioComponent> AudioComponent = nullptr;

    // Runtime cache (manager fills these from table/overrides)
    FEardarProfileRow ResolvedProfile;

    // Utility: where is this sound?
    UFUNCTION(BlueprintCallable, Category="EARDAR")
    FVector GetEmitterLocation() const;

protected:
    virtual void BeginPlay() override;
};
