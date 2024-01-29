// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PRAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

APRAICharacter::APRAICharacter()
{
	// CharacterMovement
	GetCharacterMovement()->bUseControllerDesiredRotation = true;			// 컨트롤러의 방향으로 회전합니다.
}

void APRAICharacter::BeginPlay()
{
	Super::BeginPlay();

	// HealthBar를 생성합니다.
	CreateHealthBarWidget();
}

void APRAICharacter::CreateHealthBarWidget()
{
	// 자식 클래스에서 구현합니다.
}
