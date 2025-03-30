// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayCues/GC_OnDestroyed.h"

#include "NiagaraFunctionLibrary.h"

bool UGC_OnDestroyed::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if (IsValid(MyTarget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_OnDestroyed::OnExecute_Implementation IsValid(MyTarget) == false"));
		return false;
	}
	if (IsValid(OnDestroyedEffect) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_OnDestroyed::OnExecute_Implementation IsValid(OnDestroyedEffect) == false"));
		return false;
	}
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_OnDestroyed::OnExecute_Implementation IsValid(World) == false"));
		return false;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, OnDestroyedEffect, MyTarget->GetActorLocation(),
		MyTarget->GetActorRotation(), MyTarget->GetActorScale3D());

	return true;
}
