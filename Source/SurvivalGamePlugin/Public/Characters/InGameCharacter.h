// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SG_Character.h"

#include "Interfaces/HittableInterface.h"
#include "Headers/GeneralDelegates.h"
#include "Headers/ItemDataStruct.h"

#include "InGameCharacter.generated.h"

class UEquipmentComponent;
class UPassiveDamageDealingComponent;


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

	UFUNCTION()
	void OnInGameCharacterDestroyed(AActor* RefToSelf);

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void PossessedBy(AController* NewController) override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	float AttackSpeed = 1;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	bool bIsRightHanded = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	UEquipmentComponent* EquipmentComponent;

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

	virtual void Landed(const FHitResult& Hit) override;

	virtual void MainHandAction();
	virtual void SecondaryHandAction();

	virtual void BindCallbacksToDependencies();

public:

	TArray<UDamageDealingComponent*> ActiveDamageDealingComponents;

	virtual void BroadcastInitialValues();

	UFUNCTION(Client, Reliable)
	virtual void Client_BroadcastInitialValues();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent; }

	UFUNCTION()
	void SetAttackSpeed(float NewValue);

	UFUNCTION(BlueprintCallable)
	float GetAttackSpeed() const;

	virtual void OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles) override;
	
	virtual bool bCanBeHit() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void OnHittableDestroyed() override;

	UFUNCTION(BlueprintCallable)
	bool IsRightHanded();

	UFUNCTION(BlueprintCallable)
	void SetIsRightHanded(bool NewValue);

	UFUNCTION(BlueprintCallable)
	bool IsAlive();

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
