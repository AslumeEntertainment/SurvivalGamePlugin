// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/DamageDealingItem.h"

#include "Components/DamageDealingComponent.h"
#include "Components/EquipmentComponent.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"


ADamageDealingItem::ADamageDealingItem()
{
    DamageDealingComponent = CreateDefaultSubobject<UDamageDealingComponent>(TEXT("DamageDealingComponent"));
    DamageDealingComponent->OnHitLandedSuccessfully.AddDynamic(this, &ADamageDealingItem::CalculateDurabilityLost);
}

void ADamageDealingItem::CalculateDurabilityLost(UObject* HitObject)
{
    float DurabilityLost = 20;
    //calculating...
    LoseDurability(DurabilityLost);
}

void ADamageDealingItem::OnItemEquiped(AInGameCharacter* PlayerOwner, FName SocketName)
{
    Super::OnItemEquiped(PlayerOwner, SocketName);

    AInGameCharacter* Player = Cast<AInGameCharacter>(PlayerOwner);
    if (IsValid(Player) == false)
    {
        return;
    }
    UEquipmentComponent* EquipmentComp = Player->GetEquipmentComponent();
    if (IsValid(EquipmentComp) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ADamageDealingItem::OnItemEquiped IsValid(EquipmentComp) == false"));
        return;
    }

    DamageDealingComponent->AddActorToIgnored(PlayerOwner);
}

void ADamageDealingItem::OnItemUnequiped()
{
    AInGameCharacter* PlayerOwner = Cast<AInGameCharacter>(GetAttachParentActor());
    if (IsValid(PlayerOwner) == false)
    {
        return;
    }
    
    DamageDealingComponent->RemoveActorFromIgnored(PlayerOwner);
    
    Super::OnItemUnequiped();
}

UDamageDealingComponent* ADamageDealingItem::GetDamageDealingComponent()
{
    return DamageDealingComponent;
}

