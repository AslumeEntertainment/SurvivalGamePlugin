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

private:

	UFUNCTION(Server, Reliable)
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

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/*UFUNCTION()
	void OnHitRegistered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);*/

public:

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetActivation(bool NewState);
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void AddActorToIgnored(AActor* ActorToIgnore);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RemoveActorFromIgnored(AActor* ActorToUnignore);

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
