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

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerHealth(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerHunger(float CurrentHunger, float MaxHunger);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerEnergy(float CurrentEnergy, float MaxEnergy);

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TMap<FGameplayTag, AItemBase*> EquipmentSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TMap<FGameplayTag, FName> EquipmentSockets;

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

	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION()
	virtual void OnImpactReceived(AActor* OverlappedActor, AActor* OtherActor) override;

	virtual void MainHandAction() PURE_VIRTUAL(AInGameCharacter::MainHandAction, );
	virtual void SecondaryHandAction() PURE_VIRTUAL(AInGameCharacter::SecondaryHandAction, );

	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;


public:

	TArray<UDamageDealingComponent*> ActiveDamageDealingComponents;

	virtual void OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles) override;

	virtual bool bCanBeHit() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void OnHittableDestroyed() override;

	UFUNCTION(BlueprintCallable)
	bool HasEquipmentSlotWithTag(FGameplayTag SlotTag) const;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void SetEquipedItemAtSlot(FGameplayTag SlotTag, AItemBase* Item);

	UFUNCTION(BlueprintCallable)
	AItemBase* GetEquipedItemAtSlot(FGameplayTag SlotTag) const;

	UFUNCTION(BlueprintCallable)
	TArray<AItemBase*> GetAllEquipedItems() const;

	UFUNCTION(BlueprintCallable)
	TArray<ADamageDealingItem*> GetAllEquipedWeapons() const;

	UFUNCTION(BlueprintCallable)
	FName GetEquipmentSocketWithTag(FGameplayTag SlotTag) const;

	UFUNCTION(BlueprintCallable)
	bool IsAlive();

	FVoidDataTransferSignature OnCharacterDeath;

	UPROPERTY(BlueprintCallable)
	FFloatTransferSignature OnHealthPercentUpdated;

	UPROPERTY(BlueprintCallable)
	FFloatTransferSignature OnHungerPercentUpdated;

	UPROPERTY(BlueprintCallable)
	FFloatTransferSignature OnEnergyPercentUpdated;

};
