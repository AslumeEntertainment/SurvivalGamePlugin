// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/SG_CapsuleTraceEmitter.h"

#include "Kismet/KismetSystemLibrary.h"


TArray<FHitResult> USG_CapsuleTraceEmitter::CastTrace(FTransform TraceOrigin, TArray<AActor*> ActorsToIgnore, FVector UpVector)
{
	TArray<FHitResult> HitResults;

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_CapsuleTraceEmitter::CastTrace IsValid(World) == false"));
		return HitResults;
	}

	FVector TraceStart = (CapsuleHalfHeight == CapsuleRadius) ? TraceOrigin.GetLocation() : TraceOrigin.GetLocation() - UpVector * (CapsuleHalfHeight - CapsuleRadius);
	FVector TraceEnd = (CapsuleHalfHeight == CapsuleRadius) ? TraceOrigin.GetLocation() : TraceOrigin.GetLocation() + UpVector * (CapsuleHalfHeight - CapsuleRadius);

	UKismetSystemLibrary::SphereTraceMulti(World, TraceStart, TraceEnd, CapsuleRadius, ETraceTypeQuery::TraceTypeQuery2,
		true, ActorsToIgnore, bShouldDrawDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, HitResults, true);
		
	return HitResults;
}
