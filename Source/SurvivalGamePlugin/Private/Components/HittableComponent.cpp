// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HittableComponent.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemInterface.h"

#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/LootTableComponent.h"
#include "Characters/InGameCharacter.h"


UHittableComponent::UHittableComponent()
{
	OnComponentBeginOverlap.AddDynamic(this, &UHittableComponent::OnImpactReceived);
	//OnComponentBeginOverlap.AddDynamic(this, &UHittableComponent::OnActorBeginOverlap);
}

void UHittableComponent::BeginPlay()
{
	Super::BeginPlay();
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
	ASC->ExecuteGameplayCue(OnDestroyedGameplayCueTag);

	if (GetOwner()->HasAuthority())
	{
		ULootTableComponent* LootTableComponent = GetOwner()->GetComponentByClass<ULootTableComponent>();
		if (IsValid(LootTableComponent))
		{
			LootTableComponent->DropLoot();
		}
	}

	GetOwner()->Destroy();
}

bool UHittableComponent::CanEverReceiveGameplayEffects()
{
	USG_AbilitySystemComponent* ASC = GetOwnerAbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UHittableComponent::CanEverReceiveGameplayEffects IsValid(ASC) == false"))
		return false;
	}
	float OwnerHealth = ASC->GetNumericAttribute(USG_AttributeSet::GetHealthAttribute());
	if (OwnerHealth <= 0)
	{
		return false;
	}

	return bCanEverReceiveGameplayEffects;
}

void UHittableComponent::SetCanDisplayDamageIndicator(bool NewValue)
{
	bCanDisplayDamageIndicator = NewValue;
}




