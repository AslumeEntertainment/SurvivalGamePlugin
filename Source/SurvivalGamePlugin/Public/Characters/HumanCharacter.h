// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/InGameCharacter.h"
#include "Headers/ItemDataStruct.h"
#include "HumanCharacter.generated.h"

class AItemBase;

UCLASS()
class SURVIVALGAMEPLUGIN_API AHumanCharacter : public AInGameCharacter
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* AbilityAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CraftingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* DisassembleAction;

	void MainHandAction() override;
	void SecondaryHandAction() override;

public:

	UPROPERTY(EditAnywhere, Category = "Equipment")
	bool bIsRightHanded = true;

	UFUNCTION(BlueprintCallable)
	void ShowPlayerInfo();

};
