// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Kismet/KismetSystemLibrary.h"

#include "SG_TraceEmitterBase.generated.h"


UCLASS(Abstract, EditInlineNew, DefaultToInstanced, CollapseCategories)
class SURVIVALGAMEPLUGIN_API USG_TraceEmitterBase : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "TraceProperties")
	bool bOnlyCheckFirstHit = false;

	UPROPERTY(EditAnywhere, Category = "TraceProperties")
	bool bShouldDrawDebug = true;

	UFUNCTION(BlueprintCallable)
	virtual TArray<FHitResult> CastTrace(FTransform TraceOrigin, TArray<AActor*> ActorsToIgnore, FVector UpVector = FVector(0,0,1)) 
		PURE_VIRTUAL(USG_TraceEmitterBase::CastTrace, return TArray<FHitResult>(););

};
