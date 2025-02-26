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

	/*FCollisionObjectQueryParams CollisionObjectQueryParams = FCollisionObjectQueryParams::DefaultObjectQueryParam;
	FCollisionShape CollisionShape;
	CollisionShape.MakeCapsule(FVector(CapsuleRadius, CapsuleRadius, CapsuleHalfHeight));
	
	FCollisionQueryParams CQP;
	CQP.TraceTag = TraceTag;
	if (bShouldDrawDebug)
	{
		World->DebugDrawTraceTag = TraceTag;
	}*/

	FVector TraceStart = TraceOrigin.GetLocation() - UpVector * (CapsuleHalfHeight - CapsuleRadius);
	FVector TraceEnd = TraceOrigin.GetLocation() + UpVector * (CapsuleHalfHeight);

	UKismetSystemLibrary::SphereTraceMulti(World, TraceStart, TraceEnd, CapsuleRadius, ETraceTypeQuery::TraceTypeQuery1,
		true, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResults, true);
		
	return HitResults;
}
