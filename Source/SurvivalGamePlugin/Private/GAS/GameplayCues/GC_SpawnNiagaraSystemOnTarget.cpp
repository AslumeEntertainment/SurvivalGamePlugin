// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayCues/GC_SpawnNiagaraSystemOnTarget.h"

#include "NiagaraFunctionLibrary.h"

bool UGC_SpawnNiagaraSystemOnTarget::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if (IsValid(MyTarget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_SpawnNiagaraSystemOnTarget::OnExecute_Implementation IsValid(MyTarget) == false"));
		return false;
	}
	if (IsValid(NiagaraSystem) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_SpawnNiagaraSystemOnTarget::OnExecute_Implementation IsValid(OnDestroyedEffect) == false"));
		return false;
	}
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_SpawnNiagaraSystemOnTarget::OnExecute_Implementation IsValid(World) == false"));
		return false;
	}

	const FVector NiagaraSystemLocation = bUseLocationParameter ? FVector(Parameters.Location) : MyTarget->GetActorLocation();

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, NiagaraSystem, NiagaraSystemLocation,
		MyTarget->GetActorRotation(), MyTarget->GetActorScale3D());

	return true;
}
