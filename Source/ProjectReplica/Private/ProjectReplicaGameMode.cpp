// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectReplicaGameMode.h"
// #include "ProjectReplicaCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/PRObjectPoolSystemComponent.h"

AProjectReplicaGameMode::AProjectReplicaGameMode()
{
	// // set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	// if (PlayerPawnBPClass.Class != NULL)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }

	// ObjectPoolSystem
	ObjectPoolSystem = CreateDefaultSubobject<UPRObjectPoolSystemComponent>(TEXT("ObjectPoolSystem"));
}

void AProjectReplicaGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// ObjectPoolSystem
	GetObjectPoolSystem()->InitializeObjectPool();
}
