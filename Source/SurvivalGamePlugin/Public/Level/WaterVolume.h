// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"

#include "GameplayTagContainer.h"
#include "ActiveGameplayEffectHandle.h"

#include "WaterVolume.generated.h"

class UBoxComponent;
class UNiagaraSystem;
class UGameplayEffect;
class UFluidDataComponent;
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
	TSubclassOf<UGameplayEffect> EffectToApplyWhileInWater = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "OnEntered")
	UNiagaraSystem* OnWaterEnteredEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* WaterBox;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UFluidDataComponent* FluidDataComponent;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UPostProcessComponent* PostProcessComponent;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WaterSurfaceMesh;

public:

	UFUNCTION()
	float GetSurfaceZ();

	UFUNCTION()
	void OnActorEnteringVolume(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnActorLeavingVolume(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void MakeSplash(FTransform SplashTransform);
};
