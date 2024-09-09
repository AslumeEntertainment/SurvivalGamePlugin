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
	USG_AnimInstance* AnimInst = Cast<USG_AnimInstance>(MeshComp->GetAnimInstance());
	if (!IsValid(AnimInst))
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_Hit::Notify IsValid(AnimInst) == false"));
		return;
	}

	FGameplayTag FinalDamageDealingComponentTag = DamageDealingComponentTag;
	if (AnimInst->bIsMirrored)
	{
		FinalDamageDealingComponentTag = GetMirroredNotifyDamageComponentTag();
	}

	for (UDamageDealingComponent* DamageDealingComponent : Player->ActiveDamageDealingComponents)
	{
		if (FinalDamageDealingComponentTag == DamageDealingComponent->GetDamageComponentTag())
		{
			DamageDealingComponent->SetActivation(bShouldActivate);
		}
	}
}

FGameplayTag USG_AnimNotify_Hit::GetMirroredNotifyDamageComponentTag()
{
	FString TagName = DamageDealingComponentTag.RequestDirectParent().ToString();
	if (DamageDealingComponentTag.ToString().Contains(".Left"))
	{
		TagName = TagName + TEXT(".Right");
	}
	else if (DamageDealingComponentTag.ToString().Contains(".Right"))
	{
		TagName = TagName + TEXT(".Left");
	}
	return FGameplayTag::RequestGameplayTag(FName(TagName), true);
}
