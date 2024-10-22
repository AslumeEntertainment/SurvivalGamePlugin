// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/DamageDealingItem.h"

#include "Components/DamageDealingComponent.h"
#include "Components/EquipmentComponent.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/HumanCharacter.h"


ADamageDealingItem::ADamageDealingItem()
{
    DamageDealingComponent = CreateDefaultSubobject<UDamageDealingComponent>(TEXT("DamageOrigin"));
    DamageDealingComponent->SetupAttachment(GetItemMesh());
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

    AHumanCharacter* HumanPlayer = Cast<AHumanCharacter>(PlayerOwner);
    if (IsValid(HumanPlayer) == false)
    {
        return;
    }
    UEquipmentComponent* EquipmentComp = HumanPlayer->GetEquipmentComponent();
    if (IsValid(EquipmentComp) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ADamageDealingItem::OnItemEquiped IsValid(EquipmentComp) == false"));
        return;
    }

    if (SocketName == EquipmentComp->GetEquipmentSocketWithTag(EquipmentComp->GetMainEquipmentSlotTag()) && HumanPlayer->IsRightHanded()
        || SocketName == EquipmentComp->GetEquipmentSocketWithTag(EquipmentComp->GetSecondaryEquipmentSlotTag()) && HumanPlayer->IsRightHanded() == false)
    {
        DamageDealingComponent->SetDamageComponentTagSuffix(".Right");
    }
    else if (SocketName == EquipmentComp->GetEquipmentSocketWithTag(EquipmentComp->GetSecondaryEquipmentSlotTag()) && HumanPlayer->IsRightHanded()
        || SocketName == EquipmentComp->GetEquipmentSocketWithTag(EquipmentComp->GetMainEquipmentSlotTag()) && HumanPlayer->IsRightHanded() == false)
    {
        DamageDealingComponent->SetDamageComponentTagSuffix(".Left");
    }
    DamageDealingComponent->AddActorToIgnored(PlayerOwner);
}

void ADamageDealingItem::OnItemUnequiped()
{
    AHumanCharacter* PlayerOwner = Cast<AHumanCharacter>(GetAttachParentActor());
    if (IsValid(PlayerOwner) == false)
    {
        return;
    }
    
    DamageDealingComponent->ResetDamageComponentTagSuffix();
    DamageDealingComponent->RemoveActorFromIgnored(PlayerOwner);
    
    Super::OnItemUnequiped();
}

UDamageDealingComponent* ADamageDealingItem::GetDamageDealingComponent()
{
    return DamageDealingComponent;
}

