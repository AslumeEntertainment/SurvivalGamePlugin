// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayCues/GC_OnFire.h"

#include "AbilitySystemInterface.h"
#include "NiagaraFunctionLibrary.h"

#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/SG_Character.h"


bool UGC_OnFire::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	/*UMeshComponent* TargetMesh = MyTarget->GetComponentByClass<UMeshComponent>();
	if (IsValid(TargetMesh) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_OnFire::OnActive_Implementation IsValid(Mesh) == false"));
		return false;
	}*/

	//TargetMesh->SetOverlayMaterial(FireOverlay);

	if (IsValid(FireEffect))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(FireEffect, MyTarget->GetRootComponent(), NAME_None, FVector::Zero(),
			FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);
	}

	return true;
}

bool UGC_OnFire::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	/*UMeshComponent* TargetMesh = MyTarget->GetComponentByClass<UMeshComponent>();
	if (IsValid(TargetMesh) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_OnFire::OnRemove_Implementation IsValid(Mesh) == false"));
		return false;
	}*/

	//TargetMesh->SetOverlayMaterial(nullptr);

	return true;
}
