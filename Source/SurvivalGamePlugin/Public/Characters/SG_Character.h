// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "Interfaces/HittableInterface.h"
#include "Headers/GameplayAbilityInputID.h"
#include "SG_Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USG_AbilitySystemComponent;
class USG_AttributeSet;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthUpdateSignature, float, HealthPercent);

UCLASS(config = Game)
class SURVIVALGAMEPLUGIN_API ASG_Character : public ACharacter, public IAbilitySystemInterface, public IHittableInterface
{
	GENERATED_BODY()

public:

	ASG_Character();

private:

	bool bIsAlive = true;

protected:

	UPROPERTY(Replicated)
	FRotator ControlRotation;

	UPROPERTY(EditAnywhere, Category = "Developer")
	bool bIsCamersMovementConstrained = false;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	float BaseMaxWalkingSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName LeftHandSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName RightHandSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BaseDamage = 10.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	USG_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY()
	USG_AttributeSet* AttributeSet;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);


protected:

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Server, Reliable)
	void ServerSetRepControlRotation(FRotator InControlRotation);


public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<class USG_GameplayAbility>> DefaultAbilities;

	UPROPERTY(BlueprintCallable)
	FHealthUpdateSignature OnHealthPercentUpdated;

	FORCEINLINE class UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }
	
	void MultiplyMaxWalkingSpeed(float SpeedMultiplier);
	void ResetWalkingSpeed();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OnHitNotifyRecieved(FVector DamageOrigin);

	virtual void OnHit(ASG_Character* InstigatingPlayer) override;
	virtual bool bCanBeHit() override;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetMulticast_OnUpdatePlayerHealth(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Die();

	UFUNCTION(BlueprintCallable)
	FName GetLeftHandSocket();

	UFUNCTION(BlueprintCallable)
	FName GetRightHandSocket();

	UFUNCTION(BlueprintCallable)
	FVector GetPlayerLookAtLocation(float DistanceFromPlayer);

	UFUNCTION(BlueprintCallable)
	FRotator GetRepControlRotation();

	UFUNCTION(BlueprintCallable)
	float GetBaseDamage();

	UFUNCTION(BlueprintCallable)
	bool IsAlive();
	
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void InitializeAttributes();
	virtual void GiveAbilities();

	FGameplayAbilitySpecHandle AddAbility(TSubclassOf<class USG_GameplayAbility> AbilityClass);
	void RemoveAbility(FGameplayAbilitySpecHandle RemovedAbilitySpecHandle);

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	void SendLocalInputToAbilitySystemComponent(bool bIsPressed, const EGameplayAbilityInputID AbilityInputID);

};
