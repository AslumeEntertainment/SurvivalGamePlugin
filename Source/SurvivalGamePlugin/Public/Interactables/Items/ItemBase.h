// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../InteractableInterface.h"
#include "ItemBase.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API AItemBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

private:
	UPROPERTY(Replicated)
	bool bIsAttached = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Testing")
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Use();

	UFUNCTION(NetMulticast, Reliable)
	void OnItemEquiped(ACharacter* PlayerCharacter, FName SocketName);

	UFUNCTION(NetMulticast, Reliable)
	void OnItemUnequiped(ACharacter* PlayerCharacter);


	virtual void InteractRequest() override;
	virtual bool bCanInteract() override;

};
