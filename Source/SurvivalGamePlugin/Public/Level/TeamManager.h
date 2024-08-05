// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Headers/TeamManagementStructs.h"

#include "TeamManager.generated.h"

class UBoxComponent;
class USceneComponent;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FTeamChangeSignature, AController*, TriggeringPlayer, ETeam, TeamToJoin);

UCLASS()
class SURVIVALGAMEPLUGIN_API ATeamManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeamManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere)
	UBoxComponent* SurvivorRoom;

	UPROPERTY(EditAnywhere)
	UBoxComponent* HunterRoom;

	UFUNCTION()
	void OnHunterEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSurvivorEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRoomExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void JoinTeam(ACharacter* TriggeringPlayerCharacter, ETeam TeamToJoin);

public:	
	
	FTeamChangeSignature OnPlayerChangingTeams;
};
