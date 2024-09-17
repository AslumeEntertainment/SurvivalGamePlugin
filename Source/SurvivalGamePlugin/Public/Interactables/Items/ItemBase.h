// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Headers/ItemDataStruct.h"
#include "Interfaces/InteractableInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "ItemBase.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API AItemBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	bool bIsAttached = false;

	UPROPERTY(Replicated)
	float CurrentDurability;

	UPROPERTY()
	FGameplayAbilitySpecHandle AbilitySpecHandle;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<class USG_GameplayAbility> ItemAbility;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	FGameplayTag BaseAbilityTag;

	UPROPERTY(EditDefaultsOnly, Category = "ItemData")
	FDataTableRowHandle ItemDataRow;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh;

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastSetActorEnableCollision(bool NewActorCollision);

public:	

	UPROPERTY()
	FItemDataStruct ItemData;

	UFUNCTION(BlueprintCallable)
	void Use(ASG_Character* PlayerOwner);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerLoseDurability(float LostDurability);

	UFUNCTION(BlueprintCallable)
	virtual void OnItemEquiped(ASG_Character* PlayerOwner, FName SocketName);

	UFUNCTION(BlueprintCallable)
	virtual void OnItemUnequiped(); 

	UFUNCTION(BlueprintCallable)
	virtual void DestroyItem();

	virtual void InteractRequest(AController* InteractingCharacter) override;
	virtual bool bCanInteract(AController* InteractingCharacter) override;

	FName GetItemDataRowName();
	class UStaticMeshComponent* GetItemMesh();
};
