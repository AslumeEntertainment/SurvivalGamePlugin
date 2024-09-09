// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SG_Character.h"
#include "InGameCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterDeath);

UCLASS()
class SURVIVALGAMEPLUGIN_API AInGameCharacter : public ASG_Character
{
	GENERATED_BODY()
	
protected:

	//void Die() override;

public:

	FCharacterDeath OnCharacterDeath;

};
