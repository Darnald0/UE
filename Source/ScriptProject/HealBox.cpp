// Fill out your copyright notice in the Description page of Project Settings.


#include "HealBox.h"
#include <ScriptProject\ScriptProjectCharacter.h>
#include "Components/BoxComponent.h"

// Sets default values
AHealBox::AHealBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Heal zone"));
	Collision->SetupAttachment(RootComponent);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AHealBox::OnOverlapBegin);

}

// Called when the game starts or when spawned
void AHealBox::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHealBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AScriptProjectCharacter* Chara = Cast<AScriptProjectCharacter>(OtherActor);
	if (Chara == nullptr)
		return;

	Chara->HealDamage(this->Heal);
}

