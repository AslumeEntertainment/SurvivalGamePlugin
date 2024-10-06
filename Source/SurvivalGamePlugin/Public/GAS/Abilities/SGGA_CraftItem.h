// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "Headers/ItemDataStruct.h"
#include "SGGA_CraftItem.generated.h"

class UItemIconWidget;
class UCraftingWindowWidget;
class UAbilityTask_WaitInputRelease;
class AHumanCharacter;
class APlayerController;
class USG_AnimInstance;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_CraftItem : public USG_GameplayAbility
{
	GENERATED_BODY()

public:

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "CraftingInfo")
	UDataTable* CraftingInfoTable;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UItemIconWidget> ItemIconWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UCraftingWindowWidget> CraftingWindowWidgetClass;

	UPROPERTY()
	UCraftingWindowWidget* CraftingWindow;

	UPROPERTY()
	UAbilityTask_WaitInputRelease* WaitInputReleaseTask;

	AHumanCharacter* Player;

	APlayerController* PlayerCont;

	USG_AnimInstance* AnimInst;

	float OldSpeed;

	UPROPERTY(Replicated)
	AItemBase* ProductItem;
	UPROPERTY(Replicated)
	FName ProductItemSocketName;

	FCraftingProcessDataStruct* ProductItemData = nullptr;

	UFUNCTION()
	void OpenCraftingWindow();

	UFUNCTION()
	void CloseCraftingWindow();

	UFUNCTION()
	void CancelCrafting(float TimeHeld);

	UFUNCTION()
	void OnItemToCraftSelected(FText itemName);

	UFUNCTION()
	void StartCrafting(FGameplayEventData Payload);

	UFUNCTION()
	void SpawnProductItem();

	UFUNCTION()
	void OnAnimMontageCompleted();

	UFUNCTION()
	void OnAnimMontageCanceled();

	UFUNCTION()
	void OnCraftingFinished(FGameplayEventData Payload);

};
