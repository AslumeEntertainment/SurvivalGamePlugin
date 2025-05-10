// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SG_AnimInstance.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USG_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite)
	bool bIsMirrored = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* ConsumeAnimMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* RoarAnimMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* DamageAnimMontage = nullptr;

public:

	UAnimMontage* GetConsumeAnimMontage() const;

	UAnimMontage* GetRoarAnimMontage() const;

	UAnimMontage* GetDamageAnimMontage() const;

};
