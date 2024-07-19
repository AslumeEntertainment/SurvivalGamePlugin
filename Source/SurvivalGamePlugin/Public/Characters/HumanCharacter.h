// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SG_Character.h"
#include "HumanCharacter.generated.h"

class AItemBase;

UCLASS()
class SURVIVALGAMEPLUGIN_API AHumanCharacter : public ASG_Character
{
	GENERATED_BODY()
	
private:

	UObject* FocusedInteractableObject = nullptr;

	FName MainHandSocket;
	FName SecondaryHandSocket;

	AItemBase* MainHandItem;
	AItemBase* SecondaryHandItem;

	void EmitInteractionChecker();
	void CheckForInteractables(FHitResult HitResult);

protected:

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bIsRightHanded = true;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionRange = 200;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	UMaterialInterface* InteractableOverlayMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipMainHandAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipSecondaryHandAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseMainHandItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseSecondaryHandItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* AbilityAction;

	void AttemptEquipWithMainHand();
	void AttemptEquipWithSecondaryHand();

	void AttemptUseMainHandItem();
	void AttemptUseSecondaryHandItem();

	void UseAbility();
};
