// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/ItemBase.h"

#include "Components/StaticMeshComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

#include "Components/DamageDealingComponent.h"
#include "Components/EquipmentComponent.h"
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

	if (HasAuthority() && IsValid(ItemData.ItemAbility))
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
	if (IsValid(InteractingPlayer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::InteractRequest IsValid(InteractingPlayer) == false"));
		return;
	}
	UEquipmentComponent* EquipmentComp = InteractingPlayer->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::InteractRequest IsValid(EquipmentComp) == false"));
		return;
	}
	UAbilitySystemComponent* ASC = InteractingPlayer->GetAbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::InteractRequest IsValid(ASC) == false"));
		return;
	}

	FGameplayTag SlotToEquipAtTag = ItemData.EquipmentSlotTag;

	if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasBarbarianStrength")) && SlotToEquipAtTag.ToString().Contains(".Combined"))
	{
		if (ItemData.EquipmentSlotTag == EquipmentComp->GetCombinedMainEquipmentSlotTag())
		{
			SlotToEquipAtTag = EquipmentComp->GetMainEquipmentSlotTag();
		}
		else if (ItemData.EquipmentSlotTag == EquipmentComp->GetCombinedSecondaryEquipmentSlotTag())
		{
			SlotToEquipAtTag = EquipmentComp->GetSecondaryEquipmentSlotTag();
		}
	}

	if (EquipmentComp->HasEquipmentSlotWithTag(EquipmentComp->GetMixedEquipmentSlotTag()) &&
		SlotToEquipAtTag.RequestDirectParent() != EquipmentComp->GetCombinedEquipmentSlotTag())
	{
		if (IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMixedEquipmentSlotTag())))
		{
			EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMixedEquipmentSlotTag())->OnItemUnequiped();
			EquipmentComp->SetEquipedItemAtSlot(EquipmentComp->GetMixedEquipmentSlotTag(), nullptr);
		}
		
		SlotToEquipAtTag = EquipmentComp->GetMixedEquipmentSlotTag();
		EquipmentComp->SetEquipedItemAtSlot(EquipmentComp->GetMixedEquipmentSlotTag(), this);
		OnItemEquiped(InteractingPlayer, EquipmentComp->GetEquipmentSocketWithTag(EquipmentComp->GetMixedEquipmentSlotTag()));
		
		return;
	}
	
	if (EquipmentComp->HasEquipmentSlotWithTag(SlotToEquipAtTag))
	{
		if (SlotToEquipAtTag.RequestDirectParent() == EquipmentComp->GetCombinedEquipmentSlotTag())
		{
			EquipmentComp->UnequipAllItems();
		}
		else
		{
			if (IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetCombinedEquipmentSlotTag())))
			{
				EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetCombinedEquipmentSlotTag())->OnItemUnequiped();
				EquipmentComp->SetEquipedItemAtSlot(EquipmentComp->GetCombinedEquipmentSlotTag(), nullptr);
			}
			if (IsValid(EquipmentComp->GetEquipedItemAtSlot(SlotToEquipAtTag)))
			{
				EquipmentComp->GetEquipedItemAtSlot(SlotToEquipAtTag)->OnItemUnequiped();
			}
		}
		
		EquipmentComp->SetEquipedItemAtSlot(SlotToEquipAtTag, this);
		OnItemEquiped(InteractingPlayer, EquipmentComp->GetEquipmentSocketWithTag(SlotToEquipAtTag));

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

