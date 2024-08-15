// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SG_AnimNotify_Hit.generated.h"

UENUM(BlueprintType)
enum class EDamageSource : uint8
{
	None,
	Hand,
	Item,
	Mouth
};

UCLASS()
class SURVIVALGAMEPLUGIN_API USG_AnimNotify_Hit : public UAnimNotify
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly)
	EDamageSource DamageSource;

public:

	UFUNCTION(BlueprintCallable)
	bool Receive_Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) const;

};
