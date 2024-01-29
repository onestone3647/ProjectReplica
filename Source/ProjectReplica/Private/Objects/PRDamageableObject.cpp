// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PRDamageableObject.h"

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
	return false;
}
