// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SG_Character.h"
#include "HumanCharacter.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API AHumanCharacter : public ASG_Character
{
	GENERATED_BODY()
	
private:

	UObject* FocusedInteractableObject = nullptr;

	void EmitInteractionChecker();
	void CheckForInteractables(FHitResult HitResult);

protected:

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionDistance = 200;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	UMaterialInterface* InteractableOverlayMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipMainHandAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipSecondaryHandAction;

	void AttemptEquipWithMainHand();
	void AttemptEquipWithSecondaryHand();
	
};
