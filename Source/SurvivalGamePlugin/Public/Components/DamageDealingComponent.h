// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "GameplayTagContainer.h"

#include "DamageDealingComponent.generated.h"

class UDamageEffect;
class USG_TraceEmitterBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitSucceedSignature, UObject*, HittableObject);


UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SURVIVALGAMEPLUGIN_API UDamageDealingComponent : public USceneComponent
{
	GENERATED_BODY()
	
public:

	UDamageDealingComponent();

private:

	void AttemptHit();

	void ProcessHit(TArray<FHitResult> HitResults);

protected:

	UPROPERTY()
	AActor* DamageInstigator = nullptr;

	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;

	UPROPERTY()
	TArray<AActor*> ActorsToIgnoreWhenActive;

	UPROPERTY(EditDefaultsOnly, Category = "DamageSourceDefaults", meta = (Categories = "DamageDealer"))
	FGameplayTag DamageSourceTag;

	UPROPERTY(EditDefaultsOnly, Category = "DamageSourceDefaults", meta = (Categories = "GameplayCue"))
	FGameplayTagContainer OnHitGameplayCueTags;

	UPROPERTY(EditAnywhere, Category = "DamageSourceDefaults")
	TArray<UDamageEffect*> DamageEffects;

	UPROPERTY(EditAnywhere, Category = "DamageSourceDefaults")
	bool bShouldOnlyHitOnce = true;

	UPROPERTY(EditDefaultsOnly, Category = "DamageSourceDefaults")
	USG_TraceEmitterBase* DamageTrace;
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetDamageSourceTag() { return DamageSourceTag; };

	UFUNCTION(BlueprintCallable)
	bool ShouldOnlyHitOnce() { return bShouldOnlyHitOnce; };

	UFUNCTION(BlueprintCallable)
	void ConfigDamageSource(AActor* InDamageInstigator);

	UFUNCTION(BlueprintCallable)
	void SetActiveness(bool NewState);
	
	UFUNCTION(BlueprintCallable)
	void AddActorToIgnored(AActor* ActorToIgnore);

	UFUNCTION(BlueprintCallable)
	void RemoveActorFromIgnored(AActor* ActorToUnignore);

	FHitSucceedSignature OnHitLandedSuccessfully;
};
