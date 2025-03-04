// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrapBase.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UDamageDealingComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API ATrapBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrapBase();

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly/*, Category = "Damage", meta = (AllowPrivateAccess = "true")*/)
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly/*, Category = "Damage", meta = (AllowPrivateAccess = "true")*/)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly/*, Category = "Damage", meta = (AllowPrivateAccess = "true")*/)
	UDamageDealingComponent* DamageDealingComponent;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool IsBoxStillOverlapping();

public:	
	

};
