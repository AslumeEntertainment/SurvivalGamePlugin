// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/ItemBase.h"
#include "Components/StaticMeshComponent.h"
#include "Characters/HumanCharacter.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_GameplayAbility.h"

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
}

void AItemBase::NetMulticastSetActorEnableCollision_Implementation(bool NewActorCollision)
{
	SetActorEnableCollision(NewActorCollision);
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemBase::Use(ASG_Character* PlayerOwner)
{
	UE_LOG(LogTemp, Warning, TEXT("Using item"));

	AHumanCharacter* HumanPlayer = Cast<AHumanCharacter>(PlayerOwner);
	if (!IsValid(HumanPlayer))
	{
		return;
	}

	FString AbilityTagSuffix;
	if ((this == HumanPlayer->MainHandItem && HumanPlayer->bIsRightHanded) || (this == HumanPlayer->SecondaryHandItem) && !HumanPlayer->bIsRightHanded)
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
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerOwner, AbilityTag, EventData);
	}
}

void AItemBase::OnItemEquiped(ASG_Character* PlayerOwner, FName SocketName)
{
	AttachToComponent(PlayerOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	StaticMesh->SetOverlayMaterial(nullptr);

	NetMulticastSetActorEnableCollision(false);
	AbilitySpecHandle = PlayerOwner->AddAbility(ItemAbility);
	bIsAttached = true;
}

void AItemBase::OnItemUnequiped(ASG_Character* PlayerOwner)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorLocation(PlayerOwner->GetActorLocation() + PlayerOwner->GetFollowCamera()->GetForwardVector() * 100);
	SetActorRotation(PlayerOwner->GetActorRotation());

	NetMulticastSetActorEnableCollision(true);
	PlayerOwner->RemoveAbility(AbilitySpecHandle);
	bIsAttached = false;
}

void AItemBase::InteractRequest()
{
	UE_LOG(LogTemp, Warning, TEXT("Interacting with item"));
}

bool AItemBase::bCanInteract()
{
	return !bIsAttached;
}

FName AItemBase::GetItemDataRowName()
{
	return ItemDataRow.RowName;
}

