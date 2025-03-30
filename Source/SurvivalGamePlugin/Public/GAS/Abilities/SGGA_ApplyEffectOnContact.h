// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_PassiveGameplayAbility.h"
#include "SGGA_ApplyEffectOnContact.generated.h"

class AStaticMeshActor;
class UGameplayEffectData;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_ApplyEffectOnContact : public USG_PassiveGameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


protected:

	AActor* AvatarActor = nullptr;

	AStaticMeshActor* OverlayMeshActor = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	bool bShouldApplyOnActivation = true;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	bool bShouldAffectUser = false;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<UGameplayEffectData*> EffectsToApply;

	UPROPERTY(EditDefaultsOnly, Category = "OverlayMesh")
	TSubclassOf<AStaticMeshActor> OverlayMeshActorClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "OverlayMesh")
	FName SocketToAttach;

	void OnHitEventReceived(FGameplayEventData Payload) override;

	void OnHitEventDealt(FGameplayEventData Payload) override;

	UFUNCTION()
	void SpawnOverlayMesh();

	UFUNCTION()
	void AttemptHit(const AActor* TargetActor);

	UFUNCTION()
	void OnAvatarActorBeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
