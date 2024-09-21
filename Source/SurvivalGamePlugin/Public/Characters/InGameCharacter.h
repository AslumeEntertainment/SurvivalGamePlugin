// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SG_Character.h"
#include "Headers/GeneralDelegates.h"
#include "InGameCharacter.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API AInGameCharacter : public ASG_Character
{
	GENERATED_BODY()
	
protected:

	void Die() override;

public:

	FVoidDataTransferSignature OnCharacterDeath;

};
