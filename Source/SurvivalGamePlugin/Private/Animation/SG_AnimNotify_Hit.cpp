// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SG_AnimNotify_Hit.h"
#include "Animation/SG_AnimInstance.h"
#include "Characters/HumanCharacter.h"
#include "Interactables/Items/ItemBase.h"


bool USG_AnimNotify_Hit::Receive_Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) const
{
	AHumanCharacter* Player = Cast<AHumanCharacter>(MeshComp->GetOwner());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_Hit::Received_Notify IsValid(Player) == false"));
		return false;
	}
	if (!Player->IsLocallyControlled())
	{
		return false;
	}

	FVector DamageOrigin;
	if (IsValid(Player->ActiveItem))
	{
		DamageOrigin = Player->ActiveItem->GetItemMesh()->GetSocketLocation(FName("DamageOrigin"));
	}
	else
	{
		USG_AnimInstance* AnimInst = Cast<USG_AnimInstance>(MeshComp->GetAnimInstance());
		if (!IsValid(AnimInst))
		{
			UE_LOG(LogTemp, Error, TEXT("USG_HitAnimNotify::Received_Notify IsValid(AnimInst) == false"));
			return false;
		}
		if (AnimInst->bIsMirrored)
		{
			DamageOrigin = Player->GetMesh()->GetSocketLocation(Player->GetLeftHandSocket());
		}
		else
		{
			DamageOrigin = Player->GetMesh()->GetSocketLocation(Player->GetRightHandSocket());
		}
	}

	Player->OnHitNotifyRecieved(DamageOrigin);
	return true;
}
