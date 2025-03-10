// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SG_Character.h"

#include "Interfaces/DamageDealerInterface.h"
#include "Headers/GeneralDelegates.h"
#include "Headers/ItemDataStruct.h"

#include "InGameCharacter.generated.h"

class UDamageDealingComponent;
class UEquipmentComponent;
class ULootTableComponent;
class UHittableComponent;
class USG_GameplayAbility;

UCLASS()
class SURVIVALGAMEPLUGIN_API AInGameCharacter : public ASG_Character, public IDamageDealerInterface
{
	GENERATED_BODY()
	
public:

	AInGameCharacter();

private:

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_IsAlive)
	bool bIsAlive = true;

	UPROPERTY(Replicated)
	bool bIsExhausted = false;

	UPROPERTY()
	float OldHittableCompHalfHeight = 0.f;

	UPROPERTY()
	TMap<FGameplayTag, UDamageDealingComponent*> DamageSources;

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerHealth(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerMaxHealth(float MaxHealth);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerHunger(float CurrentHunger, float MaxHunger);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerMaxHunger(float MaxHunger);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerEnergy(float CurrentEnergy, float MaxEnergy);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerMaxEnergy(float MaxEnergy);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_OnUpdatePlayerCurrentSpeed(float Velocity);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetMulticast_SetHittableCompHalfHeight(float NewValue);

	UFUNCTION()
	void OnInGameCharacterDeath();

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void PossessedBy(AController* NewController) override;

	UPROPERTY(EditDefaultsOnly, Category = "OnCrouched", meta = (ForceUnits = cm))
	float HittableCompOnCrouchedHalfHeight = 60.f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	float AttackSpeed = 1;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	bool bIsRightHanded = true;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS|CharacterDefaults")
	TSubclassOf<USG_GameplayAbility> MainHandAbility;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS|CharacterDefaults")
	TSubclassOf<USG_GameplayAbility> SecondaryHandAbility;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	UEquipmentComponent* EquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	UHittableComponent* HittableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ULootTableComponent* LootTableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseMainHandItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseSecondaryHandItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void MainHandAction();

	virtual void CancelMainHandAction();

	virtual void SecondaryHandAction();

	virtual void CancelSecondaryHandAction();

	virtual void BindCallbacksToDependencies();

	virtual void InitCharacterDefaults() override;

	UFUNCTION()
	void OnRep_IsAlive();

public:

	virtual void BroadcastInitialValues();

	virtual void Crouch(bool bClientSimulation = false) override;

	virtual void UnCrouch(bool bClientSimulation = false) override;

	void AddDamageSource(UDamageDealingComponent* NewDamageSource) override;

	void RemoveDamageSourceByTag(FGameplayTag DamageSourceTag) override;

	void SetDamageSourceActivenessByTag(FGameplayTag DamageSourceTag, bool ActivenessState) override;

	void SetActivenessForAllDamageSources(bool ActivenessState) override;

	UFUNCTION(Client, Reliable)
	virtual void Client_BroadcastInitialValues();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent; }

	UFUNCTION()
	void SetAttackSpeed(float NewValue);

	UFUNCTION(BlueprintCallable)
	float GetAttackSpeed(AItemBase* HeldItem = nullptr) const;

	UFUNCTION(BlueprintCallable)
	void SetIsRightHanded(bool NewValue);

	UFUNCTION(BlueprintCallable)
	bool IsRightHanded() const;

	UFUNCTION(BlueprintCallable)
	void SetIsAlive(bool NewValue);

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable)
	void SetIsExhausted(bool NewValue);

	UFUNCTION(BlueprintCallable)
	bool IsExhausted() const;

	UFUNCTION(BlueprintCallable)
	void ShowPlayerInfo();

	FVoidDataTransferSignature OnCharacterDeath;

	FFloatTransferSignature OnHealthPercentUpdated;
	FFloatTransferSignature OnMaxHealthUpdated;
	FFloatTransferSignature OnHungerPercentUpdated;
	FFloatTransferSignature OnMaxHungerUpdated;
	FFloatTransferSignature OnEnergyPercentUpdated;
	FFloatTransferSignature OnMaxEnergyUpdated;

	FFloatTransferSignature OnCurrentSpeedUpdated;

};
