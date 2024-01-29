// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PRPlayerController.h"

APRPlayerController::APRPlayerController()
{
}

void APRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 게임을 시작하면 ViewPort에 마우스를 가둡니다.
	const FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}
