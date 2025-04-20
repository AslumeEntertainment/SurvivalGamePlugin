// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "SG_AnimNotify_Hit.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USG_AnimNotify_Hit : public UAnimNotify
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, meta = (Categories = "DamageDealer"))
	FGameplayTag DamageSourceTag;

	UPROPERTY(EditAnywhere)
	bool bShouldActivate = false;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	FGameplayTag GetMirroredNotifyDamageComponentTag();

};
