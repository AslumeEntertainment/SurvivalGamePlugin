// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameplayTagContainer.h"
#include "WaterVolume.generated.h"

class UBoxComponent;
class UGameplayEffect;
class UStaticMeshComponent;
class UPostProcessComponent;

UCLASS(Blueprintable)
class SURVIVALGAMEPLUGIN_API AWaterVolume : public AActor
{
	GENERATED_BODY()
	
public:

	AWaterVolume();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "OnEntered")
	FGameplayTagContainer TagsToApplyWhenEntered;

	UPROPERTY(EditDefaultsOnly, Category = "OnEntered")
	TArray<TSubclassOf<UGameplayEffect>> EffectsToApplyWhenEntered;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* WaterBox;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UPostProcessComponent* PostProcessComponent;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WaterSurfaceMesh;

public:

	UFUNCTION()
	void OnActorEnteringVolume(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnActorLeavingVolume(AActor* OverlappedActor, AActor* OtherActor);
};
