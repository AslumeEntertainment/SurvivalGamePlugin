// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/DamageDealingItem.h"
#include "Characters/SG_Character.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Components/DamageDealingComponent.h"


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

void ADamageDealingItem::OnItemEquiped(ASG_Character* PlayerOwner, FName SocketName)
{
    Super::OnItemEquiped(PlayerOwner, SocketName);

    if (SocketName == PlayerOwner->GetLeftHandSocket())
    {
        DamageDealingComponent->SetDamageComponentTagSuffix(".Left");
    }
    else if (SocketName == PlayerOwner->GetRightHandSocket())
    {
        DamageDealingComponent->SetDamageComponentTagSuffix(".Right");
    }
    DamageDealingComponent->AddActorToIgnored(PlayerOwner);
    //UE_LOG(LogTemp, Warning, TEXT("Item equiped, it now has the tag %s"), *(DamageDealingComponent->GetDamageComponentTag().ToString()));
}

void ADamageDealingItem::OnItemUnequiped()
{
    FName SocketName = GetAttachParentSocketName();
    ASG_Character* PlayerOwner = Cast<ASG_Character>(GetAttachParentActor());
    if (!IsValid(PlayerOwner))
    {
        UE_LOG(LogTemp, Error, TEXT("ADamageDealingItem::OnItemUnequiped IsValid(PlayerOwner) == false"));
        return;
    }
    
    DamageDealingComponent->ResetDamageComponentTagSuffix();
    //UE_LOG(LogTemp, Warning, TEXT("Item unequiped, it now has the tag %s"), *(DamageDealingComponent->GetDamageComponentTag().ToString()));
    
    DamageDealingComponent->RemoveActorFromIgnored(PlayerOwner);
    
    Super::OnItemUnequiped();
}

UDamageDealingComponent* ADamageDealingItem::GetDamageDealingComponent()
{
    return DamageDealingComponent;
}

