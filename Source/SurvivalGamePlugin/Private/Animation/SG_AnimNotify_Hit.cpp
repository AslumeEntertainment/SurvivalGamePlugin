// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SG_AnimNotify_Hit.h"

#include "Interfaces/DamageDealerInterface.h"
#include "Characters/InGameCharacter.h"


USG_AnimNotify_Hit::USG_AnimNotify_Hit()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void USG_AnimNotify_Hit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AInGameCharacter* Player = Cast<AInGameCharacter>(MeshComp->GetOwner());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_Hit::Notify IsValid(Player) == false"));
		return;
	}

	Player->SetDamageSourceActivenessByTag(DamageSourceTag, bShouldActivate);
}
