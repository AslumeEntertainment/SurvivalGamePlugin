// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Headers/DamageGameplayEffectStruct.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "DamageDealingComponent.generated.h"

class UGameplayEffect;
class IHittableInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitSucceedSignature, UObject*, HittableObject);

UCLASS()
class SURVIVALGAMEPLUGIN_API UDamageDealingComponent : public USceneComponent
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

	UPROPERTY(EditDefaultsOnly, Category = "DamageRange")
	float DamageCapsuleRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "DamageRange")
	float DamageCapsuleHeight = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "DamageType")
	FGameplayTag DamageComponentTag;

	UPROPERTY(EditDefaultsOnly, Category = "DamageEffect")
	FDamageGameplayEffect DefaultDamageEffect;

	FGameplayEffectSpecHandle DefaultGameplayEffectSpecHandle;
	TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles;
	
public:

	UFUNCTION(BlueprintCallable)
	void SetActivation(bool NewState);
	
	UFUNCTION(BlueprintCallable)
	void AddActorToIgnored(AActor* ActorToIgnore);

	UFUNCTION(BlueprintCallable)
	void RemoveActorFromIgnored(AActor* ActorToUnignore);

	void ChangeDefaultDamageEffect(FDamageGameplayEffect NewGameplayEffect);
	void SetDefaultDamageType(ESG_DamageType NewDamageType);
	void SetDefaultDamageValue(float NewValue);
	void SetDefaultDamageEffectDuration(float NewValue);
	void SetDefaultDamageEffectPeriod(float NewValue);

	void SetDamageComponentTagSuffix(FString Suffix);
	void ResetDamageComponentTagSuffix();

	FGameplayEffectSpecHandle ConfigDamageGameplayEffect(FDamageGameplayEffect NewGameplayEffect);
	void ResetDamageEffects();

	FGameplayTag GetDamageComponentTag();

	FHitSucceedSignature OnHitLandedSuccessfully;
};
