// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/DamageDealingItem.h"

#include "Components/DamageDealingComponent.h"
#include "Components/EquipmentComponent.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"


ADamageDealingItem::ADamageDealingItem()
{
    DamageDealingComponent = CreateDefaultSubobject<UDamageDealingComponent>(TEXT("DamageDealingComponent"));
    DamageDealingComponent->SetupAttachment(GetRootComponent());
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

    Player->AddDamageSource(DamageDealingComponent);
    DamageDealingComponent->AddActorToIgnored(PlayerOwner);
    DamageDealingComponent->ConfigDamageSource(PlayerOwner);
}

void ADamageDealingItem::OnItemUnequiped()
{
    AInGameCharacter* PlayerOwner = Cast<AInGameCharacter>(GetAttachParentActor());
    if (IsValid(PlayerOwner) == false)
    {
        return;
    }
    
    PlayerOwner->RemoveDamageSourceByTag(DamageDealingComponent->GetDamageSourceTag());
    DamageDealingComponent->RemoveActorFromIgnored(PlayerOwner);
    DamageDealingComponent->ConfigDamageSource(nullptr);
    
    Super::OnItemUnequiped();
}

UDamageDealingComponent* ADamageDealingItem::GetDamageDealingComponent()
{
    return DamageDealingComponent;
}

