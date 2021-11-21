// Fill out your copyright notice in the Description page of Project Settings.


#include "HurtBox.h"
#include <ScriptProject\ScriptProjectCharacter.h>
#include "Components/BoxComponent.h"

// Sets default values
AHurtBox::AHurtBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube.1M_Cube'"));
	//Collision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NomMesh"));
	//if (CubeMesh.Succeeded())
	//{
	//	Collision->SetStaticMesh(CubeMesh.Object);
	//}
	//Collision->SetSimulatePhysics(true);
	//Collision->OnComponentHit.AddDynamic(this, &AHurtBox::OnHit);
	//RootComponent = Collision;
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage zone"));
	Collision->SetupAttachment(RootComponent);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AHurtBox::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AHurtBox::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHurtBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHurtBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AScriptProjectCharacter* Chara = Cast<AScriptProjectCharacter>(OtherActor);
	if (Chara == nullptr)
		return;
	//FString hp = FString::SanitizeFloat(Chara->PlayerHealth);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Before damage " + hp));
	Chara->GetDamage(this->Damage);
}

//void AHurtBox::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//	ARenduCharacter* Chara = Cast<ARenduCharacter>(OtherActor);
//	if (Chara == nullptr)
//		return;
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Overlap"));
//	FString hp = FString::SanitizeFloat(Chara->PlayerHealth);
//	GLog->Log(hp);
//	Chara->GetDamage(5);
//}
