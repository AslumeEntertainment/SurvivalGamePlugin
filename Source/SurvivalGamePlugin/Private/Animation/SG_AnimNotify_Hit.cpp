// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SG_AnimNotify_Hit.h"
#include "Animation/SG_AnimInstance.h"
#include "Characters/SG_Character.h"
#include "Components/DamageDealingComponent.h"


void USG_AnimNotify_Hit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ASG_Character* Player = Cast<ASG_Character>(MeshComp->GetOwner());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_Hit::Notify IsValid(Player) == false"));
		return;
	}
	if (!Player->IsLocallyControlled())
	{
		return;
	}
	USG_AnimInstance* AnimInst = Cast<USG_AnimInstance>(MeshComp->GetAnimInstance());
	if (!IsValid(AnimInst))
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_Hit::Notify IsValid(AnimInst) == false"));
		return;
	}

	if (AnimInst->bIsMirrored)
	{
		MirrorNotifyDamageComponentTag();
	}

	for (UDamageDealingComponent* DamageDealingComponent : Player->ActiveDamageDealingComponents)
	{
		if (DamageDealingComponentTag == DamageDealingComponent->GetDamageComponentTag())
		{
			DamageDealingComponent->SetActivation(bShouldActivate);
		}
	}
}

void USG_AnimNotify_Hit::MirrorNotifyDamageComponentTag()
{
	UE_LOG(LogTemp, Warning, TEXT("Before mirror: AnimNotify GameplayTag - %s"), *DamageDealingComponentTag.ToString());
	FString TagName = DamageDealingComponentTag.RequestDirectParent().ToString();
	if (DamageDealingComponentTag.ToString().Contains(".Left"))
	{
		TagName = TagName + TEXT(".Right");
	}
	else if (DamageDealingComponentTag.ToString().Contains(".Right"))
	{
		TagName = TagName + TEXT(".Left");
	}
	DamageDealingComponentTag = DamageDealingComponentTag.RequestGameplayTag(FName(TagName), true);
	UE_LOG(LogTemp, Warning, TEXT("After mirror: AnimNotify GameplayTag - %s"), *DamageDealingComponentTag.ToString());
}
