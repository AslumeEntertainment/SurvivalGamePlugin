// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/DamageDealingItem.h"

#include "Components/DamageDealingComponent.h"
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
    Server_LoseDurability(DurabilityLost);
}

void ADamageDealingItem::OnItemEquiped(AInGameCharacter* PlayerOwner, FName SocketName)
{
    Super::OnItemEquiped(PlayerOwner, SocketName);

    AHumanCharacter* HumanPlayer = Cast<AHumanCharacter>(PlayerOwner);
    if (IsValid(HumanPlayer) == false)
    {
        return;
    }

    if (SocketName == HumanPlayer->GetEquipmentSocketOfType(EEquipmentSlot::Main) && HumanPlayer->bIsRightHanded
        || SocketName == HumanPlayer->GetEquipmentSocketOfType(EEquipmentSlot::Secondary) && HumanPlayer->bIsRightHanded == false)
    {
        DamageDealingComponent->SetDamageComponentTagSuffix(".Right");
    }
    else if (SocketName == HumanPlayer->GetEquipmentSocketOfType(EEquipmentSlot::Secondary) && HumanPlayer->bIsRightHanded
        || SocketName == HumanPlayer->GetEquipmentSocketOfType(EEquipmentSlot::Main) && HumanPlayer->bIsRightHanded == false)
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

