// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SG_AnimNotify_Hit.h"

#include "Components/DamageDealingComponent.h"
#include "Characters/InGameCharacter.h"
#include "Animation/SG_AnimInstance.h"


void USG_AnimNotify_Hit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AInGameCharacter* Player = Cast<AInGameCharacter>(MeshComp->GetOwner());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_Hit::Notify IsValid(Player) == false"));
		return;
	}
	UDamageDealingComponent* DamageDealingComponent = Player->GetComponentByClass<UDamageDealingComponent>();
	if (IsValid(DamageDealingComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_Hit::Notify IsValid(DamageDealingComponent) == false"));
		return;
	}

	FGameplayTag FinalDamageSourceTag = Player->IsMirrored() ? DamageSourceTag : GetMirroredNotifyDamageComponentTag();

	DamageDealingComponent->SetDamageSourceActivation(FinalDamageSourceTag, bShouldActivate);
}

FGameplayTag USG_AnimNotify_Hit::GetMirroredNotifyDamageComponentTag()
{
	FString TagName = DamageSourceTag.RequestDirectParent().ToString();
	if (DamageSourceTag.ToString().Contains(".Left"))
	{
		TagName = TagName + TEXT(".Right");
	}
	else if (DamageSourceTag.ToString().Contains(".Right"))
	{
		TagName = TagName + TEXT(".Left");
	}
	return FGameplayTag::RequestGameplayTag(FName(TagName), true);
}
