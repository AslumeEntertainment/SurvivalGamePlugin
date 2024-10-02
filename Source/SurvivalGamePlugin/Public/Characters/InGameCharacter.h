// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SG_Character.h"
#include "Interfaces/HittableInterface.h"
#include "Headers/GeneralDelegates.h"
#include "Headers/ItemDataStruct.h"
#include "InGameCharacter.generated.h"

class AItemBase;
class ADamageDealingItem;

UCLASS()
class SURVIVALGAMEPLUGIN_API AInGameCharacter : public ASG_Character, public IHittableInterface
{
	GENERATED_BODY()
	
public:

	AInGameCharacter();

private:

	bool bIsAlive = true;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipmentSlot, AItemBase*> EquipmentSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipmentSlot, FName> EquipmentSockets;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	UDamageDealingComponent* LeftHandDamageDealingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	UDamageDealingComponent* RightHandDamageDealingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseMainHandItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseSecondaryHandItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UnequipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void MainHandAction() PURE_VIRTUAL(AInGameCharacter::MainHandAction, );
	virtual void SecondaryHandAction() PURE_VIRTUAL(AInGameCharacter::SecondaryHandAction, );

	virtual void UnequipItem() PURE_VIRTUAL(AInGameCharacter::UnequipItem, );

	void StartSprint();
	void FinishSprint();


public:

	TArray<UDamageDealingComponent*> ActiveDamageDealingComponents;

	FVoidDataTransferSignature OnCharacterDeath;

	UPROPERTY(BlueprintCallable)
	FFloatTransferSignature OnHealthPercentUpdated;

	UPROPERTY(BlueprintCallable)
	FFloatTransferSignature OnHungerPercentUpdated;

	UPROPERTY(BlueprintCallable)
	FFloatTransferSignature OnEnergyPercentUpdated;

	virtual void OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles) override;

	virtual bool bCanBeHit() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void OnHittableDestroyed() override;

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerHealth(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerHunger(float CurrentHunger, float MaxHunger);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerEnergy(float CurrentEnergy, float MaxEnergy);

	UFUNCTION(BlueprintCallable)
	bool HasEquipmentSlotOfType(EEquipmentSlot SlotType) const;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void SetEquipedItemAtSlot(EEquipmentSlot SlotType, AItemBase* Item);

	UFUNCTION(BlueprintCallable)
	AItemBase* GetEquipedItemAtSlot(EEquipmentSlot SlotType) const;

	UFUNCTION(BlueprintCallable)
	TArray<ADamageDealingItem*> GetAllEquipedWeapons() const;

	UFUNCTION(BlueprintCallable)
	FName GetEquipmentSocketOfType(EEquipmentSlot SlotType) const;

	UFUNCTION(BlueprintCallable)
	bool IsAlive();

};
