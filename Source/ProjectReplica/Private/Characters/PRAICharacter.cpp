// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PRAICharacter.h"

#include "Components/PRWeaponSystemComponent.h"
#include "Controllers/PRBaseAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

APRAICharacter::APRAICharacter()
{
	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;			// 컨트롤러의 방향으로 회전합니다.

	// Activate
	bActivate = false;
}

void APRAICharacter::BeginPlay()
{
	Super::BeginPlay();

	// HealthBar를 생성합니다.
	CreateHealthBarWidget();

	// AI 컨트롤러를 Spawn합니다.
	SpawnDefaultController();
}

#pragma region PooledableInterface
bool APRAICharacter::IsActivate_Implementation() const
{
	return bActivate;
}

void APRAICharacter::Activate_Implementation()
{
	bActivate = true;
	SetActorHiddenInGame(!bActivate);
	SetActorTickEnabled(bActivate);
}

void APRAICharacter::Deactivate_Implementation()
{
	bActivate = false;
	SetActorHiddenInGame(!bActivate);
	SetActorTickEnabled(bActivate);
}

int32 APRAICharacter::GetPoolIndex_Implementation() const
{
	return INDEX_NONE;
}

float APRAICharacter::GetLifespan_Implementation() const
{
	return INDEX_NONE;
}

void APRAICharacter::SetLifespan_Implementation(float NewLifespan)
{
}
#pragma endregion 

void APRAICharacter::CreateHealthBarWidget()
{
	// 자식 클래스에서 구현합니다.
}

void APRAICharacter::Attack_Implementation()
{
}
