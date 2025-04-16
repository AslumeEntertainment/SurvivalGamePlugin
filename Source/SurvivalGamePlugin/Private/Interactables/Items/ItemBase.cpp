// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/ItemBase.h"

#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

#include "Components/SG_AIPerceptionStimuliSourceComp.h"
#include "Components/DamageDealingComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/LootTableComponent.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


void AItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemBase, bIsAttached);
}

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	StimulusSource = CreateDefaultSubobject<USG_AIPerceptionStimuliSourceComp>(TEXT("StimulusSource"));

	OnItemEquipedDelegate.AddDynamic(this, &AItemBase::OnItemEquiped);
	OnItemUnequipedDelegate.AddDynamic(this, &AItemBase::OnItemUnequiped);
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	FItemDataStruct* TempItemData = ItemDataRow.GetRow<FItemDataStruct>("");
	if (TempItemData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::BeginPlay Item actor doesn't have valid item data!"));
		return;
	}
	ItemData = *TempItemData;

	if (HasAuthority() == false)
	{
		return;
	}

	NetMulticast_SetActorPhysics(true);

	if (IsValid(AbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::BeginPlay IsValid(AbilitySystemComponent) == false"));
		return;
	}
	if (IsValid(AttributeSet) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::BeginPlay IsValid(AttributeSet) == false"));
		return;
	}

	AbilitySystemComponent->SetNumericAttributeBase(USG_AttributeSet::GetMaxHealthAttribute(), ItemData.ItemMaxDurability);
	AbilitySystemComponent->SetNumericAttributeBase(USG_AttributeSet::GetHealthAttribute(), ItemData.ItemMaxDurability);
}

void AItemBase::Server_SetActorPhysics_Implementation(bool NewActorPhysics)
{
	NetMulticast_SetActorPhysics(NewActorPhysics);
}

void AItemBase::NetMulticast_SetActorPhysics_Implementation(bool NewActorPhysics)
{
	StaticMeshComponent->SetSimulatePhysics(NewActorPhysics);
	StaticMeshComponent->SetCollisionEnabled(NewActorPhysics ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	CollisionCapsuleComponent->SetCollisionEnabled(NewActorPhysics ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}

void AItemBase::Server_LoseDurability_Implementation(float LostDurability, bool bSafeDurabilityLoss)
{
	LoseDurability(LostDurability, bSafeDurabilityLoss);
}

void AItemBase::LoseDurability(float LostDurability, bool bSafeDurabilityLoss)
{
	if (HasAuthority() == false)
	{
		Server_LoseDurability(LostDurability, bSafeDurabilityLoss);
		return;
	}

	float CurrentDurability = AbilitySystemComponent->GetNumericAttributeBase(USG_AttributeSet::GetHealthAttribute()) - LostDurability;
	AbilitySystemComponent->SetNumericAttributeBase(USG_AttributeSet::GetHealthAttribute(), CurrentDurability);

	if (CurrentDurability <= 0)
	{
		DestroyHittable(bSafeDurabilityLoss);
		return;
	}
}

UAbilitySystemComponent* AItemBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

float AItemBase::GetDurability() const
{
	return AbilitySystemComponent->GetNumericAttributeBase(USG_AttributeSet::GetHealthAttribute());
}

void AItemBase::SetDurability(float InDurability)
{
	AbilitySystemComponent->SetNumericAttributeBase(USG_AttributeSet::GetHealthAttribute(), InDurability);
}

void AItemBase::OnItemEquiped(AInGameCharacter* PlayerOwner, FName SocketName)
{
	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(PlayerOwner->GetAbilitySystemComponent());
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::OnItemEquiped IsValid(ASC) == false"));
		return;
	}

	HasAuthority() ? Server_SetActorPhysics(false) : NetMulticast_SetActorPhysics(false);

	AttachToComponent(PlayerOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	StaticMeshComponent->SetOverlayMaterial(nullptr);

	FGameplayAbilitySpec Spec;
	if (HasAuthority() && IsValid(ItemData.ItemAbility))
	{
		Spec = FGameplayAbilitySpec(ItemData.ItemAbility, 1, INDEX_NONE, PlayerOwner);
		Spec.DynamicAbilityTags.AddTag(ItemData.ItemAbility.GetDefaultObject()->InputTag);
		AbilitySpecHandle = ASC->GiveAbility(Spec);
	}
	if (HasAuthority() && IsValid(ItemData.ItemSecondaryAbility))
	{
		Spec = FGameplayAbilitySpec(ItemData.ItemSecondaryAbility, 1, INDEX_NONE, PlayerOwner);
		Spec.DynamicAbilityTags.AddTag(ItemData.ItemSecondaryAbility.GetDefaultObject()->InputTag);
		SecondaryAbilitySpecHandle = ASC->GiveAbility(Spec);
	}
	
	ASG_AIController* AICont = Cast<ASG_AIController>(PlayerOwner->GetController());
	if (IsValid(AICont))
	{
		AICont->UnregisterTarget(this);
	}

	bIsAttached = true;
}

void AItemBase::OnItemUnequiped()
{
	ASG_Character* PlayerOwner = Cast<ASG_Character>(GetAttachParentActor());
	if (IsValid(PlayerOwner) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::OnItemUnequiped IsValid(PlayerOwner) == false"));
		return;
	}

	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(PlayerOwner->GetAbilitySystemComponent());
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::OnItemUnequiped IsValid(ASC) == false"));
		return;
	}

	PlayerOwner->DisablePlayerInput(0.01f);

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorLocation(PlayerOwner->GetPlayerLookAtLocation(100.f));
	SetActorRotation(PlayerOwner->GetActorRotation());

	HasAuthority() ? Server_SetActorPhysics(true) : NetMulticast_SetActorPhysics(true);

	if (HasAuthority())
	{
		ASC->RemoveAbilityFromSpec(AbilitySpecHandle);
		ASC->RemoveAbilityFromSpec(SecondaryAbilitySpecHandle);
	}
	
	bIsAttached = false;
}

void AItemBase::DestroyHittable(bool bSafeDestruct)
{
	if (IsValid(AbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::DestroyHittable IsValid(AbilitySystemComponent) == false"));
		return;
	}
	AbilitySystemComponent->OnOwnerDestroyed();

	if (HasAuthority() == false)
	{
		return;
	}

	TArray<AActor*> DroppedLoot = LootTableComponent->DropLoot(bSafeDestruct);

	AInGameCharacter* ItemOwner = Cast<AInGameCharacter>(GetAttachParentActor());
	if (IsValid(ItemOwner) == false)
	{
		Destroy();
		return;
	}

	UEquipmentComponent* EquipmentComp = ItemOwner->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::InteractRequest IsValid(EquipmentComp) == false"));
		return;
	}
	EquipmentComp->UnequipItem(this);

	for (AActor* DroppedActor : DroppedLoot)
	{
		AItemBase* DroppedItemActor = Cast<AItemBase>(DroppedActor);
		if (IsValid(DroppedItemActor) == false)
		{
			continue;
		}

		float DurabilityPercent = AbilitySystemComponent->GetNumericAttributeBase(USG_AttributeSet::GetHealthAttribute()) / 
			AbilitySystemComponent->GetNumericAttributeBase(USG_AttributeSet::GetMaxHealthAttribute());
		
		UE_LOG(LogTemp, Warning, TEXT("DurabilityPercent - %.2f"), DurabilityPercent);
		
		if (DurabilityPercent <= 0.f) DroppedItemActor->SetDurability(DroppedItemActor->GetDurability() / 2.f);
		else DroppedItemActor->SetDurability(DroppedItemActor->GetDurability() * DurabilityPercent);

		EquipmentComp->EquipItem(Cast<AItemBase>(DroppedActor), true);
	}

	Destroy();
}

bool AItemBase::InteractRequest(AController* InteractingCharacter)
{
	if (HasAuthority() == false)
	{
		return false;
	}
	AInGameCharacter* InteractingPlayer = Cast<AInGameCharacter>(InteractingCharacter->GetPawn());
	if (IsValid(InteractingPlayer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::InteractRequest IsValid(InteractingPlayer) == false"));
		return false;
	}
	UEquipmentComponent* EquipmentComp = InteractingPlayer->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::InteractRequest IsValid(EquipmentComp) == false"));
		return false;
	}

	EquipmentComp->EquipItem(this);

	return true;
}

bool AItemBase::bCanInteract(AController* InteractingCharacter) const
{
	AInGameCharacter* InteractingPawn = Cast<AInGameCharacter>(InteractingCharacter->GetPawn());
	if (IsValid(InteractingPawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::bCanInteract IsValid(InteractingPawn) == false"));
		return false;
	}
	UAbilitySystemComponent* ASC = InteractingPawn->GetAbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::bCanInteract IsValid(ASC) == false"));
		return false;
	}
	UEquipmentComponent* EquipmentComp = InteractingPawn->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::bCanInteract IsValid(EquipmentComp) == false"));
		return false;
	}

	if (EquipmentComp->CanEquipItem(this) == false)
	{
		return false;
	}
	
	return !bIsAttached;
}

FText AItemBase::GetInteractableName()
{
	return ItemData.ItemName;
}

FName AItemBase::GetItemDataRowName() const
{
	return ItemDataRow.RowName;
}

FGameplayTagContainer AItemBase::GetItemAbilityTags() const
{
	if (IsValid(ItemData.ItemAbility))
	{
		return ItemData.ItemAbility.GetDefaultObject()->AbilityTags;
	}
	return FGameplayTagContainer();
}

FGameplayTagContainer AItemBase::GetItemSecondaryAbilityTags() const
{
	if (IsValid(ItemData.ItemSecondaryAbility))
	{
		return ItemData.ItemSecondaryAbility.GetDefaultObject()->AbilityTags;
	}
	return GetItemAbilityTags();
}

