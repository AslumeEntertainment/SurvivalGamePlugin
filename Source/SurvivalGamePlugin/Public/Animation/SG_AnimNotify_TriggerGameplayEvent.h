// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "SG_AnimNotify_TriggerGameplayEvent.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USG_AnimNotify_TriggerGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "GameplayTag")
	FGameplayTag EventTag;

public:

	UFUNCTION(BlueprintCallable)
	bool Receive_Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) const;

};
