// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SG_AnimNotify_Hit.h"
#include "Animation/SG_AnimInstance.h"
#include "Characters/HumanCharacter.h"
#include "Characters/AnimalCharacter.h"
#include "Interactables/Items/ItemBase.h"


bool USG_AnimNotify_Hit::Receive_Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) const
{
	ASG_Character* Player = Cast<ASG_Character>(MeshComp->GetOwner());
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

	switch (DamageSource)
	{
		case EDamageSource::None:
		{
			break;
		}
			

		case EDamageSource::Hand:
		{
			USG_AnimInstance* AnimInst = Cast<USG_AnimInstance>(MeshComp->GetAnimInstance());
			if (!IsValid(AnimInst))
			{
				UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_Hit::Received_Notify IsValid(AnimInst) == false"));
				return false;
			}

			DamageOrigin = AnimInst->bIsMirrored ? DamageOrigin = Player->GetMesh()->GetSocketLocation(Player->GetLeftHandSocket()) : DamageOrigin = Player->GetMesh()->GetSocketLocation(Player->GetRightHandSocket());
			break;
		}

		case EDamageSource::Item:
		{
			AHumanCharacter* HumanPlayer = Cast<AHumanCharacter>(Player);
			if (!IsValid(HumanPlayer))
			{
				UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_Hit::Received_Notify IsValid(AnimInst) == false"));
				return false;
			}

			if (!IsValid(HumanPlayer->ActiveItem))
			{
				UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_Hit::Received_Notify IsValid(HumanPlayer->ActiveItem) == false"));
				return false;
			}

			DamageOrigin = HumanPlayer->ActiveItem->GetItemMesh()->GetSocketLocation(FName("DamageOrigin"));
			break;
		}

		case EDamageSource::Mouth:
		{
			AAnimalCharacter* AnimalPlayer = Cast<AAnimalCharacter>(Player);
			if (!IsValid(AnimalPlayer))
			{
				UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_Hit::Received_Notify IsValid(AnimalPlayer) == false"));
				return false;
			}

			DamageOrigin = AnimalPlayer->GetMesh()->GetSocketLocation(AnimalPlayer->GetMouthSocket());
			break;
		}
	}

	Player->OnHitNotifyRecieved(DamageOrigin);
	return true;
}
