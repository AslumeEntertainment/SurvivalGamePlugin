// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/TraceEmitters/SG_TraceEmitterBase.h"
#include "SG_BoxTraceEmitter.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USG_BoxTraceEmitter : public USG_TraceEmitterBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "TraceProperties")
	FVector BoxExtend = FVector(20);
	
	virtual TArray<FHitResult> CastTrace(FTransform TraceOrigin, TArray<AActor*> ActorsToIgnore, FVector UpVector) override;

};
