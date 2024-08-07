// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SG_HitAnimNotify.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USG_HitAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	bool Receive_Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) const;

};
