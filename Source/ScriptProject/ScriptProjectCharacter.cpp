// Copyright Epic Games, Inc. All Rights Reserved.

#include "ScriptProjectCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "PickUp.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// AScriptProjectCharacter

AScriptProjectCharacter::AScriptProjectCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	HoldingZone = CreateDefaultSubobject<USceneComponent>(TEXT("Holding Zone"));
	HoldingZone->SetupAttachment(RootComponent);
	FVector HoldLocation(100, 0, 0);
	HoldingZone->SetRelativeLocation(HoldLocation);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AScriptProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AScriptProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AScriptProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AScriptProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AScriptProjectCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AScriptProjectCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AScriptProjectCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AScriptProjectCharacter::OnResetVR);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AScriptProjectCharacter::OnInteract);
}

void AScriptProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsHolding && CurrentItem != nullptr)
	{
		CurrentItem->SetActorLocation(HoldingZone->GetComponentLocation());
	}
}

void AScriptProjectCharacter::GetDamage(float Damage)
{
	GLog->Log("TakeDamage");
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("take damage"));
	this->PlayerHealth = this->PlayerHealth - Damage;

	CheckHealth();
}

void AScriptProjectCharacter::HealDamage(float Heal)
{
	GLog->Log("heal");
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("heal"));
	this->PlayerHealth = this->PlayerHealth + Heal;

	CheckHealth();
}

void AScriptProjectCharacter::CheckHealth()
{
	FString hp = FString::SanitizeFloat(this->PlayerHealth);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Health " + hp));
	if (this->PlayerHealth <= 0)
	{
		DelayKill();
	}
}

void AScriptProjectCharacter::DelayKill()
{
	GLog->Log("Delay kill");
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("delay kill start"));
	SetActorHiddenInGame(true);
	FTimerHandle TimerHandle;

	GLog->Log("After 3sec");
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("3 seconds"));
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundActors);

	auto PlayerStart = Cast<APlayerStart>(FoundActors[0]);
	auto PlayerController = this->GetController();

	PlayerController->UnPossess();
	auto PlayerClass = this->GetClass();
	GLog->Log("Lose control");
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("lose control"));

	auto NewCharacter = GetWorld()->SpawnActor<AActor>(PlayerClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
	{

		if (NewCharacter)
		{
			auto CharacterToPossess = Cast<ACharacter>(NewCharacter);
			if (CharacterToPossess)
			{
				GLog->Log("New body");
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("new body"));
				PlayerController->Possess(CharacterToPossess);
				this->Destroy();
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("destroy"));
				GLog->Log("destroy");
			}
		}
	}, 3, false);
}

void AScriptProjectCharacter::KillPlayer()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundActors);
	auto PlayerStart = Cast<APlayerStart>(FoundActors[0]);
	auto PlayerController = this->GetController();
	PlayerController->UnPossess();
	auto PlayerClass = this->GetClass();

	auto NewCharacter = GetWorld()->SpawnActor<AActor>(PlayerClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());

	if (NewCharacter)
	{
		auto CharacterToPossess = Cast<ACharacter>(NewCharacter);
		if (CharacterToPossess)
		{
			PlayerController->Possess(CharacterToPossess);
			this->Destroy();
		}
	}
	//GetWorld()->GetAuthGameMode()->RestartPlayerAtPlayerStart();
	//AGameModeBase::RestartPlayerAtPlayerStart();
}

void AScriptProjectCharacter::OnResetVR()
{
	// If ScriptProject is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in ScriptProject.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AScriptProjectCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AScriptProjectCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AScriptProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AScriptProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AScriptProjectCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AScriptProjectCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AScriptProjectCharacter::OnInteract() 
{
	if (!IsHolding)
	{
		auto Start = FollowCamera->GetComponentLocation();
		auto ForwardVector = FollowCamera->GetForwardVector();
		auto End = ((ForwardVector * 1000.f) + Start);

		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 100, 0, 1);

		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, DefaultComponentQueryParams, DefaultResponseParam))
		{
			auto HitActor = Hit.GetActor()->GetClass()->GetDefaultObject();
			APickUp* Obj = Cast<APickUp>(HitActor);

			if (Obj == nullptr) 
			{
				GLog->Log("null");
				return;
			}

			CurrentItem = Obj;
			//Obj->PlayerRef = this;
			Obj->DisablePhysics();
			Obj->IsGettingHold = true;
			//CurrentItem->MyMesh->CanEditSimulatePhysics();
			//CurrentItem->MyMesh->SetSimulatePhysics(false);

			IsHolding = true;
		}
	}
	else 
	{
		//CurrentItem->MyMesh->CanEditSimulatePhysics();
		//CurrentItem->MyMesh->SetSimulatePhysics(true);
		CurrentItem->IsGettingHold = false;
		CurrentItem->EnablePhysics();
		//CurrentItem->PlayerRef;
		CurrentItem;
		IsHolding = false;
	}
}
