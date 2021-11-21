// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUp.h"
#include "ScriptProjectCharacter.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
APickUp::APickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Mesh"));
	MyMesh->CanEditSimulatePhysics();
	MyMesh->SetSimulatePhysics(true);
	RootComponent = MyMesh;
	//auto PC = UGameplayStatics::GetPlayerCharacter(this, 0);
	//PlayerRef = Cast<AScriptProjectCharacter>(PC);

	//PlayerRef = UGameplayStatics::GetPlayerPawn(0);
}

// Called when the game starts or when spawned
void APickUp::BeginPlay()
{
	Super::BeginPlay();

	//if (IsGettingHold)
	//{
	//	SetActorLocation(PlayerRef->HoldingZone->GetComponentLocation());
	//}
}

// Called every frame
void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickUp::DisablePhysics()
{
	GLog->Log("disable physics");
	MyMesh->SetSimulatePhysics(false);
}

void APickUp::EnablePhysics()
{
	GLog->Log("enable physics");
	MyMesh->SetSimulatePhysics(true);
}