// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PRDamageableObject.h"
#include "ProjectReplicaGameMode.h"
#include "Kismet/GameplayStatics.h"

APRDamageableObject::APRDamageableObject()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APRDamageableObject::BeginPlay()
{
	Super::BeginPlay();
	
}

bool APRDamageableObject::TakeDamage_Implementation(FPRDamageInfo DamageInfo)
{
	AProjectReplicaGameMode* PRGameMode = Cast<AProjectReplicaGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if(IsValid(PRGameMode))
	{
		PRGameMode->ActivateDamageAmount(DamageInfo.ImpactLocation, DamageInfo.Amount, DamageInfo.bIsCritical, DamageInfo.DamageElement);
		Destroy();

		return true;
	}

	return false;
}
