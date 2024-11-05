// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayCues/GC_OnFire.h"

#include "AbilitySystemInterface.h"

#include "GAS/Abilities/SGGA_OnStartedBurning.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/SG_Character.h"


bool UGC_OnFire::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	UMeshComponent* TargetMesh = MyTarget->GetComponentByClass<UMeshComponent>();
	if (IsValid(TargetMesh) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_OnFire::OnActive_Implementation IsValid(Mesh) == false"));
		return false;
	}

	TargetMesh->SetOverlayMaterial(FireOverlay);

	return true;
}

bool UGC_OnFire::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(MyTarget);
	if (ASI == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_OnFire::OnRemove_Implementation ASI == nullptr"));
		return false;
	}
	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(ASI->GetAbilitySystemComponent());
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_OnFire::OnRemove_Implementation IsValid(ASC) == false"));
		return false;
	}

	if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasImmunity.Fire")))
	{
		return true;
	}

	ASG_Character* Character = Cast<ASG_Character>(MyTarget);
	if (IsValid(Character))
	{
		UAnimInstance* AnimInst = Character->GetMesh()->GetAnimInstance();
		if (IsValid(AnimInst) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UGC_OnFire::OnExecute_Implementation IsValid(AnimInst) == false"));
			return false;
		}

		if (AnimInst->IsAnyMontagePlaying() == false)
		{
			Character->PlayAnimMontage(HurtAnimation);
		}
	}
	
	return true;
}

bool UGC_OnFire::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	UMeshComponent* TargetMesh = MyTarget->GetComponentByClass<UMeshComponent>();
	if (IsValid(TargetMesh) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_OnFire::OnRemove_Implementation IsValid(Mesh) == false"));
		return false;
	}

	TargetMesh->SetOverlayMaterial(nullptr);

	return true;
}
