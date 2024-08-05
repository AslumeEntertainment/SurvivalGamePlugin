// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Headers/ItemDataStruct.h"
#include "../InteractableInterface.h"
#include "GameplayAbilitySpecHandle.h"
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
	class ASG_Character* PlayerOwner;

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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	FItemDataStruct ItemData;

	UFUNCTION(BlueprintCallable)
	void Use();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerLoseDurability(float LostDurability);

	UFUNCTION(BlueprintCallable)
	virtual void OnItemEquiped(ASG_Character* NewPlayerOwner, FName SocketName);

	UFUNCTION(BlueprintCallable)
	virtual void OnItemUnequiped();

	virtual void InteractRequest() override;
	virtual bool bCanInteract() override;

	FName GetItemDataRowName();

};
