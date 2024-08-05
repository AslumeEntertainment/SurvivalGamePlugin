// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/TeamManager.h"

#include "GameFramework/Character.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ATeamManager::ATeamManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	HunterRoom = CreateDefaultSubobject<UBoxComponent>(TEXT("HunterRoomBounds"));
	SurvivorRoom = CreateDefaultSubobject<UBoxComponent>(TEXT("SurvivorRoomBounds"));

	HunterRoom->SetupAttachment(GetRootComponent());
	SurvivorRoom->SetupAttachment(GetRootComponent());
	
}

// Called when the game starts or when spawned
void ATeamManager::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())
	{
		return;
	}

	HunterRoom->OnComponentBeginOverlap.AddDynamic(this, &ATeamManager::OnHunterEnter);
	SurvivorRoom->OnComponentBeginOverlap.AddDynamic(this, &ATeamManager::OnSurvivorEnter);

	HunterRoom->OnComponentEndOverlap.AddDynamic(this, &ATeamManager::OnRoomExit);
	SurvivorRoom->OnComponentEndOverlap.AddDynamic(this, &ATeamManager::OnRoomExit);
}

void ATeamManager::OnHunterEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (IsValid(Character) == false)
	{
		return;
	}

	JoinTeam(Character, ETeam::Hunter);
}

void ATeamManager::OnSurvivorEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (IsValid(Character) == false)
	{
		return;
	}

	JoinTeam(Character, ETeam::Survivor);

}

void ATeamManager::OnRoomExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (IsValid(Character) == false)
	{
		return;
	}

	JoinTeam(Character, ETeam::Teamless);
}

void ATeamManager::JoinTeam(ACharacter* TriggeringPlayerCharacter, ETeam TeamToJoin)
{
	AController* PC = TriggeringPlayerCharacter->GetController();
	OnPlayerChangingTeams.ExecuteIfBound(PC, TeamToJoin);
}
