// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/ItemBase.h"

#include "Components/StaticMeshComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

#include "Components/DamageDealingComponent.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"

// Sets default values
AItemBase::AItemBase()
{
	bReplicates = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetRootComponent(StaticMesh);
}

void AItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemBase, bIsAttached);
	DOREPLIFETIME(AItemBase, CurrentDurability);
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	HasAuthority() ? Server_SetActorPhysics(true) : NetMulticast_SetActorPhysics(true);

	FItemDataStruct* TempItemData = ItemDataRow.GetRow<FItemDataStruct>("");
	if (TempItemData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::BeginPlay Item actor doesn't have valid item data!"));
		return;
	}
	ItemData = *TempItemData;
	CurrentDurability = ItemData.ItemMaxDurability;
}

void AItemBase::Server_SetActorPhysics_Implementation(bool NewActorPhysics)
{
	NetMulticast_SetActorPhysics(NewActorPhysics);
}

void AItemBase::NetMulticast_SetActorPhysics_Implementation(bool NewActorPhysics)
{
	StaticMesh->SetSimulatePhysics(NewActorPhysics);
	NewActorPhysics ? StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics) : StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemBase::Server_LoseDurability_Implementation(float LostDurability)
{
	LoseDurability(LostDurability);
}

void AItemBase::LoseDurability(float LostDurability)
{
	if (HasAuthority() == false)
	{
		Server_LoseDurability(LostDurability);
		return;
	}

	CurrentDurability -= LostDurability;
	if (CurrentDurability <= 0)
	{
		DestroyItem();
	}
}

float AItemBase::GetDurability() const
{
	return CurrentDurability;
}

void AItemBase::SetDurability(float InDurability)
{
	CurrentDurability = InDurability;
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
	StaticMesh->SetOverlayMaterial(nullptr);

	if (HasAuthority())
	{
		FGameplayAbilitySpec Spec = FGameplayAbilitySpec(ItemData.ItemAbility, 1, INDEX_NONE, PlayerOwner);
		Spec.DynamicAbilityTags.AddTag(ItemData.ItemAbility.GetDefaultObject()->InputTag);
		AbilitySpecHandle = ASC->GiveAbility(Spec);
	}
	
	bIsAttached = true;
}

void AItemBase::OnItemUnequiped()
{
	ASG_Character* PlayerOwner = Cast<ASG_Character>(GetAttachParentActor());
	if (!IsValid(PlayerOwner))
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

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorLocation(PlayerOwner->GetPlayerLookAtLocation(100.f));
	SetActorRotation(PlayerOwner->GetActorRotation());

	HasAuthority() ? Server_SetActorPhysics(true) : NetMulticast_SetActorPhysics(true);

	if (HasAuthority())
	{
		ASC->RemoveAbilityFromSpec(AbilitySpecHandle);
	}
	
	bIsAttached = false;
}

void AItemBase::DestroyItem()
{
	if (IsValid(GetAttachParentActor()))
	{
		OnItemUnequiped();
	}
	Destroy();
}

void AItemBase::InteractRequest(AController* InteractingCharacter)
{
	if (HasAuthority() == false)
	{
		return;
	}

	AInGameCharacter* InteractingPlayer = Cast<AInGameCharacter>(InteractingCharacter->GetPawn());
	if (!IsValid(InteractingPlayer))
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::InteractRequest IsValid(InteractingPlayer) == nullptr"));
		return;
	}

	if (InteractingPlayer->HasEquipmentSlotWithTag(InteractingPlayer->GetMixedEquipmentSlotTag()))
	{
		if (IsValid(InteractingPlayer->GetEquipedItemAtSlot(InteractingPlayer->GetMixedEquipmentSlotTag())))
		{
			InteractingPlayer->GetEquipedItemAtSlot(InteractingPlayer->GetMixedEquipmentSlotTag())->OnItemUnequiped();
		}
		
		InteractingPlayer->SetEquipedItemAtSlot(InteractingPlayer->GetMixedEquipmentSlotTag(), this);
		OnItemEquiped(InteractingPlayer, InteractingPlayer->GetEquipmentSocketWithTag(InteractingPlayer->GetMixedEquipmentSlotTag()));
		
		return;
	}
	if (InteractingPlayer->HasEquipmentSlotWithTag(ItemData.EquipmentSlotTag))
	{
		if (ItemData.EquipmentSlotTag.RequestDirectParent() == FGameplayTag::RequestGameplayTag("EquipmentSlot.Combined"))
		{
			InteractingPlayer->UnequipAllItems();
		}
		else
		{
			if (IsValid(InteractingPlayer->GetEquipedItemAtSlot(FGameplayTag::RequestGameplayTag("EquipmentSlot.Combined"))))
			{
				InteractingPlayer->GetEquipedItemAtSlot(FGameplayTag::RequestGameplayTag("EquipmentSlot.Combined"))->OnItemUnequiped();
				InteractingPlayer->SetEquipedItemAtSlot(FGameplayTag::RequestGameplayTag("EquipmentSlot.Combined"), nullptr);
			}
			if (IsValid(InteractingPlayer->GetEquipedItemAtSlot(ItemData.EquipmentSlotTag)))
			{
				InteractingPlayer->GetEquipedItemAtSlot(ItemData.EquipmentSlotTag)->OnItemUnequiped();
			}
		}

		InteractingPlayer->SetEquipedItemAtSlot(ItemData.EquipmentSlotTag, this);
		OnItemEquiped(InteractingPlayer, InteractingPlayer->GetEquipmentSocketWithTag(ItemData.EquipmentSlotTag));

		UE_LOG(LogTemp, Warning, TEXT("AItemBase::InteractRequest Equiping item in slot"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("AItemBase::InteractRequest Item could not be equiped"));
}

bool AItemBase::bCanInteract(AController* InteractingCharacter)
{
	return !bIsAttached;
}

FName AItemBase::GetItemDataRowName() const
{
	return ItemDataRow.RowName;
}

UStaticMeshComponent* AItemBase::GetItemMesh()
{
	return StaticMesh;
}

