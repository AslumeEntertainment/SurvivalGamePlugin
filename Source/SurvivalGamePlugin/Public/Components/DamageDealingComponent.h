// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Headers/DamageGameplayEffectStruct.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"

#include "DamageDealingComponent.generated.h"

class UGameplayEffect;
class IHittableInterface;
class UDamageSource;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitSucceedSignature, UObject*, HittableObject);

UCLASS()
class SURVIVALGAMEPLUGIN_API UDamageDealingComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	UDamageDealingComponent();

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	void AttemptHit();

protected:

	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;

	UPROPERTY()
	TArray<AActor*> ActorsToIgnoreWhenActive;

	UPROPERTY(EditDefaultsOnly, Category = "DamageSources")
	TMap<FGameplayTag, UDamageSource*> DamageSources;
	
public:

	UFUNCTION(BlueprintCallable)
	TMap<FGameplayTag, UDamageSource*> GetDamageSources();

	UFUNCTION(BlueprintCallable)
	void AddDamageSource(UDamageSource* DamageSource, FGameplayTag DamageSourceTag);

	UFUNCTION(BlueprintCallable)
	void RemoveDamageSource(FGameplayTag DamageSourceTag);

	UFUNCTION(BlueprintCallable)
	void SetDamageSourceActivation(FGameplayTag DamageSourceTag, bool bShouldActivate);

	UFUNCTION(BlueprintCallable)
	void SetActivation(bool NewState);
	
	UFUNCTION(BlueprintCallable)
	void AddActorToIgnored(AActor* ActorToIgnore);

	UFUNCTION(BlueprintCallable)
	void RemoveActorFromIgnored(AActor* ActorToUnignore);

	FGameplayEffectSpecHandle ConfigDamageGameplayEffect(FDamageGameplayEffect NewGameplayEffect);

	FHitSucceedSignature OnHitLandedSuccessfully;
};
