// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/SG_BoxTraceEmitter.h"

#include "Kismet/KismetSystemLibrary.h"


TArray<FHitResult> USG_BoxTraceEmitter::CastTrace(FTransform TraceOrigin, TArray<AActor*> ActorsToIgnore, FVector UpVector)
{
	TArray<FHitResult> HitResults;

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_BoxTraceEmitter::CastTrace IsValid(World) == false"));
		return HitResults;
	}

	UKismetSystemLibrary::BoxTraceMulti(World, TraceOrigin.GetLocation(), TraceOrigin.GetLocation(), BoxExtend, TraceOrigin.Rotator(), ETraceTypeQuery::TraceTypeQuery1,
		true, ActorsToIgnore, bShouldDrawDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, HitResults, true);

	return HitResults;
}
