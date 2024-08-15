// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interfaces/InteractableInterface.h"

#include "PlayerCustomiser.generated.h"


class APlayerController;

DECLARE_DYNAMIC_DELEGATE(FPlayerReadiness);

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

	UFUNCTION(Server, Reliable)
	void ServerToggleReady();

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	virtual void InteractRequest() override;
	virtual bool bCanInteract(AController* InteractingCharacter) override;

	void SetOwningPlayer(AController* InOwner);

	bool GetIsPlayerReady();
	bool IsOwningPlayer(AController* CharacterToCheck);

	FPlayerReadiness OnPlayerReadinessChanged;
};
