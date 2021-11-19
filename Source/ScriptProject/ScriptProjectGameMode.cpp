// Copyright Epic Games, Inc. All Rights Reserved.

#include "ScriptProjectGameMode.h"
#include "ScriptProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AScriptProjectGameMode::AScriptProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
