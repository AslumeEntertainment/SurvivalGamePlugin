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

	ATrapBase();

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UDamageDealingComponent* DamageDealingComponent;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool IsBoxStillOverlapping();

public:	
	

};
