// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HittableComponent.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemInterface.h"

#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/SG_GameplayEffectContext.h"
#include "GAS/SG_AbilitySystemGlobals.h"
#include "Components/EquipmentComponent.h"
#include "Components/LootTableComponent.h"
#include "Characters/InGameCharacter.h"


UHittableComponent::UHittableComponent()
{
	OnComponentBeginOverlap.AddDynamic(this, &UHittableComponent::OnImpactReceived);
	OnComponentBeginOverlap.AddDynamic(this, &UHittableComponent::OnActorBeginOverlap);
}

void UHittableComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(DamageEffectOnOverlap.DamageEffectClass) == false)
	{
		return;
	}
	if (IsValid(GetOwner()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UHittableComponent::BeginPlay IsValid(GetOwner()) == false"));
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

void UHittableComponent::OnImpactReceived(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(GetOwner()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UHittableComponent::OnImpactReceived IsValid(GetOwner()) == false"));
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
	UE_LOG(LogTemp, Log, TEXT("UHittableComponent::OnImpactReceived - %s, Velocity - %.3f"), *OtherActor->GetName(), GetOwner()->GetVelocity().Length());

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag("CommonEvents.ReceiveDamage.Impact");
	FGameplayEventData EventData = FGameplayEventData();
	EventData.Instigator = OtherActor;

	USG_AbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), EventTag, EventData);
}

void UHittableComponent::OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

USG_AbilitySystemComponent* UHittableComponent::GetOwnerAbilitySystemComponent()
{
	if (IsValid(GetOwner()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UHittableComponent::GetOwnerAbilitySystemComponent IsValid(GetOwner()) == false"));
		return nullptr;
	}
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner());
	if(ASI == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UHittableComponent::GetOwnerAbilitySystemComponent ASI == nullptr"));
		return nullptr;
	}
	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(ASI->GetAbilitySystemComponent());
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UHittableComponent::GetOwnerAbilitySystemComponent IsValid(ASC) == false"));
		return nullptr;
	}
	return ASC;
}

void UHittableComponent::OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles)
{
	USG_AbilitySystemComponent* ASC = GetOwnerAbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UHittableComponent::OnHit IsValid(ASC) == false"));
		return;
	}
	for (FGameplayEffectSpecHandle SpecHandle : GameplayEffectSpecHandles)
	{
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void UHittableComponent::OnHittableDestroyed()
{
	if (IsValid(GetOwner()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UHittableComponent::OnHittableDestroyed IsValid(GetOwner()) == false"));
		return;
	}
	USG_AbilitySystemComponent* ASC = GetOwnerAbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UHittableComponent::OnHittableDestroyed IsValid(ASC) == false"));
		return;
	}

	ASC->CancelAllAbilities();
	ASC->RemoveAllGameplayCues();
	ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Explode"));

	if (GetOwner()->HasAuthority())
	{
		ULootTableComponent* LootTableComponent = GetOwner()->GetComponentByClass<ULootTableComponent>();
		if (IsValid(LootTableComponent))
		{
			LootTableComponent->DropLoot();
		}
	}

	AInGameCharacter* Character = Cast<AInGameCharacter>(GetOwner());
	if (IsValid(Character))
	{
		UEquipmentComponent* EquipmentComp = Character->GetComponentByClass<UEquipmentComponent>();
		if (IsValid(EquipmentComp) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UHittableComponent::OnHittableDestroyed IsValid(EquipmentComp) == false"));
			return;
		}
		EquipmentComp->UnequipAllItems();

		Character->SetIsAlive(false);
		Character->OnCharacterDeath.Broadcast();
	}
	else
	{
		GetOwner()->Destroy();
	}
}




