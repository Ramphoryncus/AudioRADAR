// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EardarTypes.h"
#include "EardarRadarWidget.generated.h"

/**
 * Base class for the radar UI. Implement drawing in Blueprint.
 * We only expose a single method to receive blips each tick.
 */
UCLASS()
class AUDIORADAR_API UEardarRadarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category="EARDAR")
	void UpdateBlips(const TArray<FEardarBlip>& Blips);
};