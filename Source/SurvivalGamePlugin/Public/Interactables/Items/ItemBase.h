// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Headers/ItemDataStruct.h"
#include "Interfaces/InteractableInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "ItemBase.generated.h"

class UStaticMeshComponent;
class AInGameCharacter;
class USG_GameplayAbility;

UCLASS()
class SURVIVALGAMEPLUGIN_API AItemBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	
	AItemBase();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	bool bIsAttached = false;

	UPROPERTY(Replicated)
	float CurrentDurability;

	UPROPERTY()
	FGameplayAbilitySpecHandle AbilitySpecHandle;

	UPROPERTY(EditDefaultsOnly, Category = "ItemData")
	FDataTableRowHandle ItemDataRow;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	UFUNCTION(Server, Reliable)
	void Server_SetActorPhysics(bool NewActorPhysics);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_SetActorPhysics(bool NewActorPhysics);

public:	

	UPROPERTY()
	FItemDataStruct ItemData;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_LoseDurability(float LostDurability);

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

	virtual void InteractRequest(AController* InteractingCharacter) override;
	virtual bool bCanInteract(AController* InteractingCharacter) override;

	FName GetItemDataRowName() const;
	class UStaticMeshComponent* GetItemMesh();
};
