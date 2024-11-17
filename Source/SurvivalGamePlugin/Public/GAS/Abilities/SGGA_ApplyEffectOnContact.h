// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "SGGA_ApplyEffectOnContact.generated.h"

class AStaticMeshActor;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_ApplyEffectOnContact : public USG_GameplayAbility
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

	UPROPERTY(EditDefaultsOnly, Category = "OverlayMesh")
	TSubclassOf<AStaticMeshActor> OverlayMeshActorClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "OverlayMesh")
	FName SocketToAttach;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> EffectsToApply;

	UFUNCTION()
	void SpawnOverlayMesh();

	UFUNCTION()
	void AttemptHit(AActor* TargetActor);

	UFUNCTION()
	void OnAvatarActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
