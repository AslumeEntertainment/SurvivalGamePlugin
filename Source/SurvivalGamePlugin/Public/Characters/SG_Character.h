// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"

#include "Headers/GeneralDelegates.h"

#include "SG_Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UDamageDealingComponent;
class UInputMappingContext;
class UInputAction;
class UAbilityInputConfig;
class USG_AbilitySystemComponent;
class USG_CharacterAttributeSet;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class SURVIVALGAMEPLUGIN_API ASG_Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ASG_Character();

private:

	UPROPERTY(Replicated)
	UObject* FocusedInteractableObject = nullptr;

	UFUNCTION(Server, Reliable)
	void Server_SetFocusedInteractable(UObject* InFocusedInteractable);

	void EmitInteractionChecker();
	void CheckForInteractables(FHitResult HitResult);


protected:

	UPROPERTY(Replicated)
	FRotator ControlRotation;

	//---remove that when you clean the movement: line 135--
	UPROPERTY(EditAnywhere, Category = "Developer")
	bool bIsCameraMovementConstrained = false;
	//-----------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionRange = 200;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	UMaterialInterface* InteractableOverlayMaterial;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS|CharacterDefaults")
	TArray<TSubclassOf<class USG_GameplayAbility>> DefaultAbilities;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS|CharacterDefaults")
	TArray<TSubclassOf<class USG_GameplayAbility>> DefaultPassiveAbilities;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS|CharacterDefaults")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS|Input")
	UAbilityInputConfig* AbilityInputConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	USG_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(Transient)
	USG_CharacterAttributeSet* AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;


	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);


protected:

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Server, Reliable)
	void Server_SetRepControlRotation(FRotator InControlRotation);

	virtual void PossessedBy(AController* NewController) override;

	void OnRep_PlayerState() override;

	void InitCharacterDefaults();

	virtual void BindCallbacksToDependencies() PURE_VIRTUAL(ASG_Character::BindCallbacksToDependencies, );
	virtual void BroadcastInitialValues() PURE_VIRTUAL(ASG_Character::BroadcastInitialValues, );


public:

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShouldDrawDebug = true;

	FORCEINLINE class UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable)
	UObject* GetFocusedInteractableObject();

	UFUNCTION(BlueprintCallable)
	FVector GetPlayerLookAtLocation(float DistanceFromPlayer);

	UFUNCTION(BlueprintCallable)
	FRotator GetRepControlRotation();

	void SendGameplayEventToSelf(FGameplayTag EventTag, FGameplayEventData EventData, bool bServerOnly = false);

private:

	UFUNCTION(Server, Reliable)
	void Server_SendGameplayEventToSelf(FGameplayTag EventTag, FGameplayEventData EventData, bool bServerOnly = false);

};
