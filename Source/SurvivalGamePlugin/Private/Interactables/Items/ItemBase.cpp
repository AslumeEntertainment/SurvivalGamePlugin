// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/ItemBase.h"
#include "Components/StaticMeshComponent.h"
#include "Characters/HumanCharacter.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_GameplayAbility.h"
#include "Headers/SG_GameplayAbilityTargetData.h"

// Sets default values
AItemBase::AItemBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
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

	FItemDataStruct* TempItemData = ItemDataRow.GetRow<FItemDataStruct>("");
	if (!TempItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::BeginPlay Item actor doesn't have valid item data!"))
	}
	ItemData = *TempItemData;
	CurrentDurability = ItemData.ItemDurability;
}

void AItemBase::NetMulticastSetActorEnableCollision_Implementation(bool NewActorCollision)
{
	SetActorEnableCollision(NewActorCollision);
}

void AItemBase::Use(ASG_Character* PlayerOwner)
{
	AHumanCharacter* HumanPlayer = Cast<AHumanCharacter>(PlayerOwner);
	if (!IsValid(HumanPlayer))
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::Use IsValid(PlayerOwner) == false"));
		return;
	}

	FString AbilityTagSuffix;
	if (GetAttachParentSocketName() == PlayerOwner->GetRightHandSocket())
	{
		AbilityTagSuffix = ".Right";
	}
	else
	{
		AbilityTagSuffix = ".Left";
	}

	FGameplayTag AbilityTag;
	AbilityTag.FromExportString(BaseAbilityTag.ToString() + AbilityTagSuffix);
	if (AbilityTag.IsValid())
	{
		FGameplayEventData EventData;
		FSG_GameplayAbilityTargetData* CustomTargetData = new FSG_GameplayAbilityTargetData();
		CustomTargetData->ItemActor = this;
		EventData.TargetData.Add(CustomTargetData);
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerOwner, AbilityTag, EventData);
	}
}

void AItemBase::ServerLoseDurability_Implementation(float LostDurability)
{
	CurrentDurability -= LostDurability;
	if (CurrentDurability <= 0)
	{
		DestroyItem();
	}
}

void AItemBase::OnItemEquiped(ASG_Character* PlayerOwner, FName SocketName)
{
	AttachToComponent(PlayerOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	StaticMesh->SetOverlayMaterial(nullptr);

	NetMulticastSetActorEnableCollision(false);
	AbilitySpecHandle = PlayerOwner->AddAbility(ItemAbility);
	bIsAttached = true;
	SetOwner(PlayerOwner);
}

void AItemBase::OnItemUnequiped()
{
	ASG_Character* PlayerOwner = Cast<ASG_Character>(GetAttachParentActor());
	if (!IsValid(PlayerOwner))
	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::OnItemUnequiped IsValid(PlayerOwner) == false"));
		return;
	}

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorLocation(PlayerOwner->GetPlayerLookAtLocation(100.f));
	SetActorRotation(PlayerOwner->GetActorRotation());

	NetMulticastSetActorEnableCollision(true);
	PlayerOwner->RemoveAbility(AbilitySpecHandle);
	bIsAttached = false;
	SetOwner(nullptr);
}

void AItemBase::DestroyItem()
{
	if (IsValid(GetAttachParentActor()))
	{
		OnItemUnequiped();
	}
	Destroy();
}

void AItemBase::InteractRequest()
{
	UE_LOG(LogTemp, Warning, TEXT("Interacting with item"));
}

bool AItemBase::bCanInteract(AController* InteractingCharacter)
{
	return !bIsAttached;
}

FName AItemBase::GetItemDataRowName()
{
	return ItemDataRow.RowName;
}

UStaticMeshComponent* AItemBase::GetItemMesh()
{
	return StaticMesh;
}

