// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SG_AnimNotify_ReportSoundEvent.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USG_AnimNotify_ReportSoundEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	TObjectPtr<USoundBase> Sound = nullptr;

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	float Volume = 1.f;

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	float Loudness = 1.f;

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	bool bOverrideAttenuationRangeIfSet = false;

	UPROPERTY(EditAnywhere, Category = "AnimNotify", meta = (ToolTip = "This will be used if the given sound doesn't have attenuation settings"))
	float DefaultMaxRange = 1000.f;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};
