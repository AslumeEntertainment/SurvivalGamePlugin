// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interfaces/Hittable.h"
#include "AbilitySystemInterface.h"

#include "SG_HittableActor.generated.h"

class UStaticMeshComponent;
class UCapsuleComponent;
class ULootTableComponent;
class USG_AbilitySystemComponent;
class UAbilitySystemComponent;
class USG_AttributeSet;

UCLASS()
class SURVIVALGAMEPLUGIN_API ASG_HittableActor : public AActor, public IHittable, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	
	ASG_HittableActor();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "General")
	bool bStartWithPhysicsEnabled = true;

	UPROPERTY(EditDefaultsOnly, Category = "General")
	bool bHasCollisionWhenPhysicsDisabled = true;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> InitializingEffect;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent *CollisionCapsuleComponent;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	ULootTableComponent* LootTableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USG_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(Transient)
	USG_AttributeSet* AttributeSet;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SetActorPhysics(bool NewActorPhysics);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetMulticast_SetActorPhysics(bool NewActorPhysics);

	virtual UShapeComponent* GetImpactChecker() override;

	virtual void OnComponentBeginTouch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

public:	

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UStaticMeshComponent* GetStaticMeshComponent();

	ULootTableComponent* GetLootTableComponent();

	virtual void DestroyHittable(bool bSafeDestruct = false) override;

	virtual USceneComponent* GetBoundsComponent() override;

	UFUNCTION(BlueprintCallable)
	void SetActorStatic();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SetActorStatic();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetMulticast_SetActorStatic();

};
