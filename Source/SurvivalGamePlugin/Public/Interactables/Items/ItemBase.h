// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/SG_HittableActor.h"

#include "Headers/GeneralDelegates.h"
#include "Headers/ItemDataStruct.h"
#include "Interfaces/InteractableInterface.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"

#include "ItemBase.generated.h"

class UStaticMeshComponent;
class AInGameCharacter;
class USG_GameplayAbility;
class UHittableComponent;
class ULootTableComponent;
class USG_AbilitySystemComponent;
class USG_AttributeSet;
class USG_AIPerceptionStimuliSourceComp;

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemEquipedSignature, AInGameCharacter*, PlayerOwner, FName, SocketName);

UCLASS()
class SURVIVALGAMEPLUGIN_API AItemBase : public ASG_HittableActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	
	AItemBase();

private:

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_LoseDurability(float LostDurability, bool bSafeDurabilityLoss);

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	bool bIsAttached = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USG_AIPerceptionStimuliSourceComp* StimulusSource;

	UPROPERTY()
	FGameplayAbilitySpecHandle AbilitySpecHandle;

	UPROPERTY()
	FGameplayAbilitySpecHandle SecondaryAbilitySpecHandle;

	UPROPERTY(EditDefaultsOnly, Category = "ItemData")
	FDataTableRowHandle ItemDataRow;

	UFUNCTION(BlueprintCallable)
	virtual void OnItemEquiped(AInGameCharacter* PlayerOwner, FName SocketName);

	UFUNCTION(BlueprintCallable)
	virtual void OnItemUnequiped();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SetActorPhysics(bool NewActorPhysics);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetMulticast_SetActorPhysics(bool NewActorPhysics);

public:	

	UPROPERTY()
	FItemDataStruct ItemData;

	virtual void LoseDurability(float LostDurability, bool bSafeDurabilityLoss = false);

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION()
	float GetDurability() const;

	UFUNCTION()
	void SetDurability(float InDurability);

	virtual void DestroyHittable(bool bSafeDestruct = false) override;

	virtual bool InteractRequest(AController* InteractingCharacter) override;
	virtual bool bCanInteract(AController* InteractingCharacter) const override;
	virtual FText GetInteractableName() override;

	FName GetItemDataRowName() const;

	FGameplayTagContainer GetItemAbilityTags() const;

	FGameplayTagContainer GetItemSecondaryAbilityTags() const;

	FOnItemEquipedSignature OnItemEquipedDelegate;
	FVoidDataTransferSignature OnItemUnequipedDelegate;
};
