// Fill out your copyright notice in the Description page of Project Settings.


#include "LavaComponent.h"
#include "ScriptProjectCharacter.h"

// Sets default values for this component's properties
ULavaComponent::ULavaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void ULavaComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULavaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULavaComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AScriptProjectCharacter* Chara = Cast<AScriptProjectCharacter>(OtherActor);
	if (Chara == nullptr)
		return;

	Chara->KillPlayer();
}

