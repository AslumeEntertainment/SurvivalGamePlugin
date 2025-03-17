// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_ApplyEffectOnContact.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/ShapeComponent.h"
#include "AbilitySystemInterface.h"
#include "Engine/StaticMeshActor.h"

#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"


void USGGA_ApplyEffectOnContact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	AvatarActor = ActorInfo->AvatarActor.Get();
	if (IsValid(AvatarActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::ActivateAbility IsValid(AvatarActor) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	UShapeComponent* AvatarCollisionComp = AvatarActor->GetComponentByClass<UShapeComponent>();
	if (IsValid(AvatarCollisionComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::ActivateAbility IsValid(AvatarCollisionComp) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (OverlayMeshActorClass != nullptr)
	{
		SpawnOverlayMesh();
	}

	AvatarCollisionComp->OnComponentHit.AddDynamic(this, &USGGA_ApplyEffectOnContact::OnAvatarActorBeginOverlap);

	if (bShouldApplyOnActivation == false)
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	AvatarCollisionComp->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor == AvatarActor && bShouldAffectUser == false)
		{
			continue;
		}
		AttemptHit(Actor);
	}
}

void USGGA_ApplyEffectOnContact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(OverlayMeshActor))
	{
		OverlayMeshActor->Destroy();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGGA_ApplyEffectOnContact::SpawnOverlayMesh()
{
	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::SpawnOverlayMesh IsValid(World) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	OverlayMeshActor = World->SpawnActor<AStaticMeshActor>(OverlayMeshActorClass, AvatarActor->GetActorLocation(), AvatarActor->GetActorRotation());

	UMeshComponent* AvatarMesh = AvatarActor->GetComponentByClass<UMeshComponent>();
	if (IsValid(AvatarMesh) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::SpawnOverlayMesh IsValid(AvatarMesh) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	OverlayMeshActor->AttachToComponent(AvatarMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketToAttach);
}

void USGGA_ApplyEffectOnContact::AttemptHit(const AActor* TargetActor)
{
	const IAbilitySystemInterface* ASI_Actor = Cast<IAbilitySystemInterface>(TargetActor);
	if (ASI_Actor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::AttemptHit ASI_Actor == nullptr"));
		return;
	}

	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(ASI_Actor->GetAbilitySystemComponent());
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::AttemptHit IsValid(ASC) == false"));
		return;
	}

	if (ASC->bCanEverReceiveDamageFromEffects == false)
	{
		return;
	}

	for (TSubclassOf<UGameplayEffect> Effect : EffectsToApply)
	{
		ASC->AddEffectFromClass(Effect, AvatarActor, AvatarActor, AvatarActor);
	}
}

void USGGA_ApplyEffectOnContact::OnHitEventReceived(FGameplayEventData Payload)
{
	AttemptHit(Payload.Instigator);
}

void USGGA_ApplyEffectOnContact::OnHitEventDealt(FGameplayEventData Payload)
{
	AttemptHit(Payload.Target);
}

void USGGA_ApplyEffectOnContact::OnAvatarActorBeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AttemptHit(OtherActor);
}

