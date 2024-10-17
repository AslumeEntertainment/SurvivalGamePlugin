// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SG_AnimInstance.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USG_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	bool bIsMirrored = false;

	/*bool IsMirrored();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SetIsMirrored(bool NewValue);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void NetMulticast_SetIsMirrored(bool NewValue);*/

};
