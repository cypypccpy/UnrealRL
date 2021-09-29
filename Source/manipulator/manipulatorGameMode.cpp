// Copyright Epic Games, Inc. All Rights Reserved.

#include "manipulatorGameMode.h"
#include "manipulatorCharacter.h"
#include "UObject/ConstructorHelpers.h"

AmanipulatorGameMode::AmanipulatorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
