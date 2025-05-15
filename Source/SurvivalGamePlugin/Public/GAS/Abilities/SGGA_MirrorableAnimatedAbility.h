// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "SGGA_MirrorableAnimatedAbility.generated.h"

class USkeletalMeshComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_MirrorableAnimatedAbility : public USG_GameplayAbility
{
	GENERATED_BODY()
	
private:

	FTimerHandle EmergencyAbilityEndTimer;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Animation Details")
	bool bWaitForAnimationToComplete = true;

	UPROPERTY(EditDefaultsOnly, Category = "Animation Details")
	bool bCanMoveDuringAnimation = true;

	UPROPERTY(EditDefaultsOnly, Category = "Animation Details")
	UAnimMontage* AnimationMontage;

	UFUNCTION()
	virtual void OnAnimMontageCompleted();

	UFUNCTION()
	virtual void OnAnimMontageBlendOut();

	UFUNCTION()
	virtual void OnAnimMontageInterrupted();

	UFUNCTION()
	virtual void OnAnimMontageCanceled();

	bool DoesAnimationMatchSkeleton(USkeletalMeshComponent* SkeletalMesh);
};
