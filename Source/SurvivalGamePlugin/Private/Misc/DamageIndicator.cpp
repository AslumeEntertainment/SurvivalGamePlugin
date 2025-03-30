// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/DamageIndicator.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

#include "UI/Widgets/Core/DamageIndicatorWidget.h"

void ADamageIndicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ADamageIndicator, Damage, COND_None, REPNOTIFY_Always);
}

ADamageIndicator::ADamageIndicator()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	SetRootComponent(WidgetComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	MovementComponent->MaxSpeed = 200;
	MovementComponent->ProjectileGravityScale = 0.0f;
}

void ADamageIndicator::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ADamageIndicator::BeginPlay IsValid(World) == false"));
		return;
	}

	UDamageIndicatorWidget* DamageIndicatorWidget = Cast<UDamageIndicatorWidget>(WidgetComponent->GetWidget());
	if (IsValid(DamageIndicatorWidget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ADamageIndicator::BeginPlay IsValid(DamageIndicatorWidget) == false"));
		return;
	}

	DamageIndicatorWidget->SetDamageText(Damage);
	if (DamageIndicatorFontForDamageType.Contains(DamageType))
	{
		DamageIndicatorWidget->SetFont(DamageIndicatorFontForDamageType[DamageType]);
	}

	FVector Direction = FVector(FMath::RandRange(-0.25f, 0.25f), FMath::RandRange(-0.25f, 0.25f), 1);
	MovementComponent->Velocity = Direction * 100;

	FTimerHandle DestroyTimer;
	World->GetTimerManager().SetTimer(DestroyTimer, this, &ADamageIndicator::DestroyDamageIndicator, 1.f);
}

void ADamageIndicator::DestroyDamageIndicator()
{
	Destroy();
}


