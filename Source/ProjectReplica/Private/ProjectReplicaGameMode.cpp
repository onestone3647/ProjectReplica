// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectReplicaGameMode.h"
// #include "ProjectReplicaCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/PRObjectPoolSystemComponent.h"
#include "Objects/PRDamageAmount.h"

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

	// DamageAmount
	DamageAmountClass = nullptr;
}

void AProjectReplicaGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// ObjectPoolSystem
	GetObjectPoolSystem()->InitializeObjectPool();
}

APRDamageAmount* AProjectReplicaGameMode::ActivateDamageAmount(FVector SpawnLocation, float DamageAmount, bool bIsCritical, EPRElement Element)
{
	if(GetWorld()
		&& DamageAmountClass != nullptr)
	{
		APRDamageAmount* DamageAmountObject = Cast<APRDamageAmount>(GetObjectPoolSystem()->GetActivateablePooledObject(DamageAmountClass));
		if(IsValid(DamageAmountObject))
		{
			DamageAmountObject->Initialize(SpawnLocation, DamageAmount, bIsCritical, Element);
			if(GetObjectPoolSystem()->ActivatePooledObject(DamageAmountObject, SpawnLocation) != nullptr)
			{
				return DamageAmountObject;
			}
		}
	}

	return nullptr;
}
