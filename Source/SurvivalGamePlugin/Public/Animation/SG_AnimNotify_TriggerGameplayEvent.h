// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayTagContainer.h"
#include "SG_AnimNotify_TriggerGameplayEvent.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USG_AnimNotify_TriggerGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "GameplayTag")
	FGameplayTag EventTag;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};
