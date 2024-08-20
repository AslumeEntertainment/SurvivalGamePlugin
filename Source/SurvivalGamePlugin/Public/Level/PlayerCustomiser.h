// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interfaces/InteractableInterface.h"

#include "PlayerCustomiser.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerReadiness);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerReturningToLobby, APlayerCustomiser*, TriggeringPlayerCustomiser);

UCLASS()
class SURVIVALGAMEPLUGIN_API APlayerCustomiser : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerCustomiser();

private:
	
	UPROPERTY(Replicated)
	bool bIsPlayerReady = false;

	UPROPERTY(Replicated)
	AController* OwningPlayer = nullptr;

	APawn* MainPawn = nullptr;

	APawn* GamePawn = nullptr;

	UFUNCTION(Server, Reliable)
	void ServerToggleReady();

	UFUNCTION()
	void AnnounceDeath();

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	virtual void InteractRequest() override;
	virtual bool bCanInteract(AController* InteractingCharacter) override;

	//---------------------Called Only on Server by the game mode-------------------
	void SetOwningPlayer(AController* InOwner);
	void SetGamePawn(APawn* InGamePawn);

	bool GetIsPlayerReady();
	bool IsOwningPlayer(AController* CharacterToCheck);
	bool IsPossesingMainPawn();

	void PossessMainPawn();
	void PossessGamePawn();

	void DestroyGamePawn();
	//-----------------------------------------------------------------------------

	FPlayerReadiness OnPlayerReadinessChanged;
	FPlayerReturningToLobby OnGamePawnDeath;
};
