// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/SG_TraceEmitterBase.h"
#include "SG_CapsuleTraceEmitter.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USG_CapsuleTraceEmitter : public USG_TraceEmitterBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "TraceProperties")
	float CapsuleHalfHeight = 40.f;

	UPROPERTY(EditAnywhere, Category = "TraceProperties")
	float CapsuleRadius = 20.f;

	virtual TArray<FHitResult> CastTrace(FTransform TraceOrigin, TArray<AActor*> ActorsToIgnore, FVector UpVector) override;
	
};
