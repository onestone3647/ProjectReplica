// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PRPlayerController.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/InputDeviceSubsystem.h"

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

bool APRPlayerController::IsUsingGamepad() const
{
	UInputDeviceSubsystem* InputDeviceSubsystem = GetGameInstance()->GetEngine()->GetEngineSubsystem<UInputDeviceSubsystem>();
	if(IsValid(InputDeviceSubsystem))
	{
		if(InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(GetPlatformUserId()).PrimaryDeviceType == EHardwareDevicePrimaryType::Gamepad)
		{
			return true;
		}
	}

	return false;
}
