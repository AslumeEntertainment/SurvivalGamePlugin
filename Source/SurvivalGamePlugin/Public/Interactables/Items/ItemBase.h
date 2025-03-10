// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/SG_HittableActor.h"

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

UCLASS()
class SURVIVALGAMEPLUGIN_API AItemBase : public ASG_HittableActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	
	AItemBase();

private:

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_LoseDurability(float LostDurability);

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	bool bIsAttached = false;

	UPROPERTY(Replicated)
	float CurrentDurability;

	UPROPERTY()
	FGameplayAbilitySpecHandle AbilitySpecHandle;

	UPROPERTY()
	FGameplayAbilitySpecHandle SecondaryAbilitySpecHandle;

	UPROPERTY(EditDefaultsOnly, Category = "ItemData")
	FDataTableRowHandle ItemDataRow;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SetActorPhysics(bool NewActorPhysics);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetMulticast_SetActorPhysics(bool NewActorPhysics);

public:	

	UPROPERTY()
	FItemDataStruct ItemData;

	virtual void LoseDurability(float LostDurability);

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION()
	float GetDurability() const;

	UFUNCTION()
	void SetDurability(float InDurability);

	UFUNCTION(BlueprintCallable)
	virtual void OnItemEquiped(AInGameCharacter* PlayerOwner, FName SocketName);

	UFUNCTION(BlueprintCallable)
	virtual void OnItemUnequiped(); 

	UFUNCTION(BlueprintCallable)
	virtual void DestroyItem();

	virtual bool InteractRequest(AController* InteractingCharacter) override;
	virtual bool bCanInteract(AController* InteractingCharacter) override;
	virtual FText GetInteractableName() override;

	FName GetItemDataRowName() const;

	FGameplayTagContainer GetItemAbilityTags() const;

	FGameplayTagContainer GetItemSecondaryAbilityTags() const;
};
