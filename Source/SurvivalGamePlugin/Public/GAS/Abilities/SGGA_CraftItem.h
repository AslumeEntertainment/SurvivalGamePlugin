// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "Headers/ItemDataStruct.h"
#include "SGGA_CraftItem.generated.h"

class UUnitIconWidget;
class UCraftingWindowWidget;
class UAbilityTask_WaitInputRelease;
class AInGameCharacter;
class UEquipmentComponent;
class APlayerController;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_CraftItem : public USG_GameplayAbility
{
	GENERATED_BODY()

public:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

protected:

	AInGameCharacter* Player;
	APlayerController* PlayerCont;
	UEquipmentComponent* EquipmentComp;

	UPROPERTY()
	FTimerHandle CraftTimer;

	TArray<FName> PossibleProductItemNames;

	FCraftingProcessDataStruct* ProductItemData = nullptr;

	FActiveGameplayEffectHandle SpeedReductionEffectSpecHandle;

	UPROPERTY(EditDefaultsOnly, Category = "CraftingProcessInfo")
	UDataTable* CraftingInfoTable;

	UPROPERTY(EditDefaultsOnly, Category = "CraftingProcessInfo")
	TSubclassOf<UGameplayEffect> SpeedReductionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "CraftingProcessInfo")
	FGameplayTag CraftingProcessContinueTag;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUnitIconWidget> ItemIconWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UCraftingWindowWidget> CraftingWindowWidgetClass;

	UPROPERTY()
	UCraftingWindowWidget* CraftingWindow;

	UPROPERTY()
	UAbilityTask_WaitInputRelease* WaitInputReleaseTask;

	UPROPERTY()
	AItemBase* ProductItem;

	UPROPERTY()
	FName ProductItemSocketName;

	UFUNCTION()
	void OpenCraftingWindow();

	UFUNCTION()
	void CloseCraftingWindow();

	UFUNCTION()
	void CancelCrafting(float TimeHeld);

	UFUNCTION()
	void OnItemToCraftSelected(int IconID);

	UFUNCTION()
	void StartCrafting(FGameplayEventData Payload);

	UFUNCTION()
	void OnCraftingFinished();

};
