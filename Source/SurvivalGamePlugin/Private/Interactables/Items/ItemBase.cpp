// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/ItemBase.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemBase::Use()
{
	UE_LOG(LogTemp, Warning, TEXT("Using item"));
}

void AItemBase::OnItemEquiped_Implementation(ACharacter* PlayerCharacter, FName SocketName)
{
	UE_LOG(LogTemp, Warning, TEXT("Equiping item"));
	AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	bIsAttached = true;
	//Destroy();
}

void AItemBase::OnItemUnequiped_Implementation(ACharacter* PlayerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("Unequiping item"));
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
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

