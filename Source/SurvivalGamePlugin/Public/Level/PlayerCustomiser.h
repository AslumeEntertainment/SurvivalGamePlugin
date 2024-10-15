// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Headers/GeneralDelegates.h"
#include "Headers/GameManagingStructs.h"
#include "Interfaces/InteractableInterface.h"

#include "PlayerCustomiser.generated.h"

class ASG_PlayerController;
class AInGameCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerCustomiserTransferSignature, APlayerCustomiser*, TriggeringPlayerCustomiser);

UCLASS()
class SURVIVALGAMEPLUGIN_API APlayerCustomiser : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerCustomiser();

private:

	UPROPERTY(Replicated)
	bool bIsActive = false;

	UPROPERTY(Replicated)
	bool bIsTaken = false;

	UPROPERTY()
	APawn* MainPawn = nullptr;

	UPROPERTY()
	AInGameCharacter* GamePawn = nullptr;

	UPROPERTY()
	ASG_PlayerController* InteractingPlayer = nullptr;

	UFUNCTION(Server, Reliable)
	void Server_ToggleReady();

	UFUNCTION()
	void InGameCharacterDead();

protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AInGameCharacter> DefaultHeroClass;

public:	

	virtual void InteractRequest(AController* InteractingCharacter) override;
	virtual bool bCanInteract(AController* InteractingCharacter) override;

	UFUNCTION()
	void Server_StopInteracting();

	UFUNCTION()
	void Server_SetIsActive(bool State);

	UFUNCTION()
	void SpawnHero(FVector Location, FRotator Rotation);

	UFUNCTION()
	void DespawnHero();

	UFUNCTION()
	void PossessMainPawn();

	UFUNCTION()
	void PossessGamePawn();

	FPlayerCustomiserTransferSignature OnPlayerStartInteracting;
	FPlayerCustomiserTransferSignature OnPlayerStopInteracting;
	FPlayerCustomiserTransferSignature OnPlayerReadinessChanged;
	FPlayerCustomiserTransferSignature OnInGameCharacterDeath;
};
