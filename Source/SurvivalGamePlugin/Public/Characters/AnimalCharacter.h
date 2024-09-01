// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/InGameCharacter.h"
#include "AnimalCharacter.generated.h"

class UDamageDealingComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API AAnimalCharacter : public AInGameCharacter
{
	GENERATED_BODY()

public:

	AAnimalCharacter();

protected:

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName MouthSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	UDamageDealingComponent* MouthDamageDealingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* AnimalBiteAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* AnimalScratchAction;

	void AttemptBite();
	void AttemptScratch();

public:

	UFUNCTION(BlueprintCallable)
	FName GetMouthSocket();
};
