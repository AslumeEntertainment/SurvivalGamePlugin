// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "Components/TimelineComponent.h"
#include "GameplayTagContainer.h"

#include "Headers/GeneralDelegates.h"

#include "SG_Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UCurveFloat;
struct FInputActionValue;

class UAbilityInputConfig;
class UDamageDealingComponent;
class USG_AbilitySystemComponent;
class USG_CharacterAttributeSet;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class SURVIVALGAMEPLUGIN_API ASG_Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ASG_Character();

private:

	bool bIsFirstPerson = true;

	bool bIsInputEnabled = true;

	bool bAlreadySetUp = false;

	float CurrentInteractionRange;

	FTimeline CameraViewResetTimeline;

	FTimerHandle CameraRotationTimer;

	UPROPERTY(Replicated)
	UObject* FocusedInteractableObject = nullptr;

	UPROPERTY(Replicated)
	bool bIsMirrored = false;

	UFUNCTION()
	void UpdateCamera(float Alpha);

	UFUNCTION(Server, Reliable)
	void Server_SetFocusedInteractable(UObject* InFocusedInteractable);

	void SetFocusedInteractable(UObject* InFocusedInteractable);

	void EmitInteractionChecker();
	void CheckForInteractables(FHitResult HitResult);

	UFUNCTION(Server, Reliable)
	void Server_SendGameplayEventToSelf(FGameplayTag EventTag, FGameplayEventData EventData, bool bServerOnly = false);

protected:

	UPROPERTY(Replicated)
	FRotator ControlRotation;

	UPROPERTY(EditAnywhere, Category = "Developer")
	bool bIsCameraMovementConstrained = false;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionRange = 200;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	UMaterialInterface* InteractableOverlayMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float TargetingDistance = 650.f;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float TargetingTraceRadius = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float CameraInterpSpeed = 20.f;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CameraDistanceFromPlayer = 200;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	UCurveFloat* CameraViewResetCurve = nullptr;

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
	UInputAction* CameraModeToggleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* TargetAction;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);


protected:

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ToggleFirstPerson();

	UFUNCTION(Server, Reliable)
	void Server_SetRepControlRotation(FRotator InControlRotation);

	void SetRepControlRotation(FRotator InControlRotation);

	virtual void PossessedBy(AController* NewController) override;

	virtual void InitCharacterDefaults();


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer")
	bool bUsesEightDirectionalMovement = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShouldDrawDebug = true;

	FORCEINLINE class UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable)
	UObject* GetFocusedInteractableObject();

	UFUNCTION(BlueprintCallable)
	FVector GetPlayerLookAtLocation(float DistanceFromPlayer);

	UFUNCTION(BlueprintCallable)
	FRotator GetRepControlRotation();

	UFUNCTION(BlueprintCallable)
	void ResetCameraView();

	UFUNCTION(BlueprintCallable)
	void StartTarget();

	UFUNCTION(BlueprintCallable)
	void StopTarget();

	UFUNCTION(BlueprintCallable)
	void PointCameraTowardsActor(UWorld* World, AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void DisablePlayerInput(float Duration = 0.f);

	UFUNCTION(BlueprintCallable)
	void EnablePlayerInput();

	UFUNCTION(BlueprintCallable)
	bool IsPlayerInputEnabled();

	UFUNCTION(BlueprintCallable)
	bool IsMoving();

	UFUNCTION(BlueprintCallable)
	bool IsMirrored();

	UFUNCTION(BlueprintCallable)
	void SetIsMirrored(bool NewValue);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SetIsMirrored(bool NewValue);

	UFUNCTION(BlueprintCallable)
	void SendGameplayEventToSelf(FGameplayTag EventTag, FGameplayEventData EventData, bool bServerOnly = false);

	FTextTransferSignature InteractIndicationTextDelegate;

	UFUNCTION(Client, Reliable)
	void Cliet_UnbindOverlayDelegates();

	UFUNCTION()
	void UnbindOverlayDelegates();

};
