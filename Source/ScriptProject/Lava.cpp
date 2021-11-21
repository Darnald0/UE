// Fill out your copyright notice in the Description page of Project Settings.


#include "Lava.h"
#include <ScriptProject\ScriptProjectCharacter.h>
#include "Components/BoxComponent.h"

// Sets default values
ALava::ALava()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("LavaZone"));
	Collision->SetupAttachment(RootComponent);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ALava::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ALava::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALava::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALava::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("lava touch"));
	AScriptProjectCharacter* Chara = Cast<AScriptProjectCharacter>(OtherActor);
	if (Chara == nullptr)
		return;

	GLog->Log("Lava death");
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("lava death"));
	Chara->DelayKill();
}

