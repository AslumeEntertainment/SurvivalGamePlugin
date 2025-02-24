// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "GameplayTagContainer.h"

#include "DamageDealingComponent.generated.h"

class UDamageEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitSucceedSignature, UObject*, HittableObject);

UENUM(BlueprintType)
enum class EDamageSourceTraceType : uint8
{
	Box,
	Sphere,
	Capsule
};

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

	UPROPERTY(EditDefaultsOnly, Category = "DamageSourceDefaults")
	FGameplayTag DamageSourceTag;

	UPROPERTY(EditAnywhere, Category = "DamageSourceDefaults")
	TArray<UDamageEffect*> DamageEffects;

	UPROPERTY(EditDefaultsOnly, Category = "DamageSourceDefaults|HitTrace")
	EDamageSourceTraceType TraceType;

	UPROPERTY(EditDefaultsOnly, Category = "DamageSourceDefaults|HitTrace")
	float HitTraceHalfWidth = 22.f;

	UPROPERTY(EditDefaultsOnly, Category = "DamageSourceDefaults|HitTrace")
	float HitTraceHalfHeight = 44.f;
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetDamageSourceTag();

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
