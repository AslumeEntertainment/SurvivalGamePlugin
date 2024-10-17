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
class UPassiveDamageDealingComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API AInGameCharacter : public ASG_Character, public IHittableInterface
{
	GENERATED_BODY()
	
public:

	AInGameCharacter();

private:

	bool bIsAlive = true;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags")
	FGameplayTag MainEquipmentSlotTag;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags")
	FGameplayTag SecondaryEquipmentSlotTag;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags")
	FGameplayTag MixedEquipmentSlotTag;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags")
	FGameplayTag CombinedEquipmentSlotTag;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags")
	FGameplayTag CombinedMainEquipmentSlotTag;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags")
	FGameplayTag CombinedSecondaryEquipmentSlotTag;

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerHealth(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerHunger(float CurrentHunger, float MaxHunger);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerEnergy(float CurrentEnergy, float MaxEnergy);

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void BeginPlay() override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	float AttackSpeed = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TMap<FGameplayTag, AItemBase*> EquipmentSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TMap<FGameplayTag, FName> EquipmentSockets;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	UDamageDealingComponent* LeftHandDamageDealingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	UDamageDealingComponent* RightHandDamageDealingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	UPassiveDamageDealingComponent* PassiveDamageDealingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseMainHandItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseSecondaryHandItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void GrantTemporalInvinsibility(float Duration);

	void RemoveInvincibility();

	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION()
	virtual void OnImpactReceived(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void MainHandAction() PURE_VIRTUAL(AInGameCharacter::MainHandAction, );
	virtual void SecondaryHandAction() PURE_VIRTUAL(AInGameCharacter::SecondaryHandAction, );

	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;


public:

	FGameplayTag GetMainEquipmentSlotTag()                { return MainEquipmentSlotTag; }
	FGameplayTag GetSecondaryEquipmentSlotTag()           { return SecondaryEquipmentSlotTag; }
	FGameplayTag GetMixedEquipmentSlotTag()               { return MixedEquipmentSlotTag; }
	FGameplayTag GetCombinedEquipmentSlotTag()            { return CombinedEquipmentSlotTag; }
	FGameplayTag GetCombinedMainEquipmentSlotTag()        { return CombinedMainEquipmentSlotTag; }
	FGameplayTag GetCombinedSecondaryEquipmentSlotTag()   { return CombinedSecondaryEquipmentSlotTag; }

	TArray<UDamageDealingComponent*> ActiveDamageDealingComponents;

	UFUNCTION(BlueprintCallable)
	float GetAttackSpeed() const;

	virtual void OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles) override;
	
	virtual bool bCanBeHit() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void OnHittableDestroyed() override;

	UFUNCTION(BlueprintCallable)
	bool HasEquipmentSlotWithTag(FGameplayTag SlotTag, bool CheckParentTag = true) const;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void SetEquipedItemAtSlot(FGameplayTag SlotTag, AItemBase* Item, bool CheckParentTag = true);

	UFUNCTION(BlueprintCallable)
	AItemBase* GetEquipedItemAtSlot(FGameplayTag SlotTag, bool CheckParentTag = true) const;

	UFUNCTION(BlueprintCallable)
	TArray<AItemBase*> GetAllEquipedItems() const;

	UFUNCTION(BlueprintCallable)
	void UnequipAllItems();

	UFUNCTION(BlueprintCallable)
	TArray<ADamageDealingItem*> GetAllEquipedWeapons() const;

	UFUNCTION(BlueprintCallable)
	bool HasEquipmentSocketWithTag(FGameplayTag SlotTag, bool CheckParentTag = true) const;

	UFUNCTION(BlueprintCallable)
	FName GetEquipmentSocketWithTag(FGameplayTag SlotTag, bool CheckParentTag = true) const;

	UFUNCTION(BlueprintCallable)
	bool IsAlive();

	UFUNCTION(BlueprintCallable)
	void ShowPlayerInfo();

	FVoidDataTransferSignature OnCharacterDeath;

	UPROPERTY(BlueprintCallable)
	FFloatTransferSignature OnHealthPercentUpdated;

	UPROPERTY(BlueprintCallable)
	FFloatTransferSignature OnHungerPercentUpdated;

	UPROPERTY(BlueprintCallable)
	FFloatTransferSignature OnEnergyPercentUpdated;

};
