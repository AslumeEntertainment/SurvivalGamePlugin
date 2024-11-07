// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PassiveDamageDealingComponent.h"

#include "Abilities/GameplayAbilityTypes.h"

#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/SG_GameplayEffectContext.h"
#include "Interfaces/HittableInterface.h"
#include "GAS/SG_AbilitySystemGlobals.h"


UPassiveDamageDealingComponent::UPassiveDamageDealingComponent()
{
	OnComponentBeginOverlap.AddDynamic(this, &UPassiveDamageDealingComponent::OnImpactReceived);
	OnComponentBeginOverlap.AddDynamic(this, &UPassiveDamageDealingComponent::OnActorBeginOverlap);
}

void UPassiveDamageDealingComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(DamageEffectOnOverlap.DamageEffectClass) == false)
	{
		return;
	}
	if (IsValid(GetOwner()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UPassiveDamageDealingComponent::BeginPlay IsValid(GetOwner()) == false"));
		return;
	}

	FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(USG_AbilitySystemGlobals::Get().AllocGameplayEffectContext());
	FSG_GameplayEffectContext* Context = static_cast<FSG_GameplayEffectContext*>(EffectContextHandle.Get());
	Context->SetDamageType(DamageEffectOnOverlap.DamageType);
	EffectContextHandle.AddSourceObject(this);

	EffectSpecHandle = UAbilitySystemBlueprintLibrary::MakeSpecHandle(DamageEffectOnOverlap.DamageEffectClass.GetDefaultObject(), GetOwner(), GetOwner());
	FGameplayEffectSpec* EffectSpec = EffectSpecHandle.Data.Get();

	EffectSpec->SetContext(EffectContextHandle);
	EffectSpec->SetDuration(DamageEffectOnOverlap.DamageEffectDuration, false);
	EffectSpec->Period = DamageEffectOnOverlap.DamageEffectPeriod;
	EffectSpec->SetSetByCallerMagnitude(DamageEffectOnOverlap.SetByCallerMagnitudeDataTag, -DamageEffectOnOverlap.DamageValue);
}

void UPassiveDamageDealingComponent::OnImpactReceived(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(GetOwner()) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPassiveDamageDealingComponent::OnImpactReceived IsValid(GetOwner()) == false"));
		return;
	}
	if (GetOwner()->HasAuthority() == false)
	{
		return;
	}
	if (SweepResult.Component != nullptr && SweepResult.Component->GetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn) == ECollisionResponse::ECR_Ignore)
	{
		return;
	}
	if (Cast<UMeshComponent>(OtherComp) == nullptr)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("AInGameCharacter::OnImpactReceived - %s, Velocity - %.3f"), *OtherActor->GetName(), GetOwner()->GetVelocity().Length());

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag("CommonEvents.ReceiveDamage.Impact");
	FGameplayEventData EventData = FGameplayEventData();
	EventData.Instigator = OtherActor;

	USG_AbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), EventTag, EventData);

	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("Impact recieved! Player velocity - %f ; OtherActor velocity - %f"), GetVelocity().Length(), OtherActor->GetVelocity().Length()));
}

void UPassiveDamageDealingComponent::OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UAbilitySystemComponent* ASC = USG_AbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (IsValid(ASC) == false)
	{
		return;
	}
	FGameplayEffectSpec* Spec = EffectSpecHandle.Data.Get();
	if (Spec == nullptr)
	{
		return;
	}
	ASC->ApplyGameplayEffectSpecToSelf(*Spec);
}



