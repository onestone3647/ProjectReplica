// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PRPlayerController.h"
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
	if(InputDeviceSubsystem)
	{
		// 현재 플랫폼 사용자 ID를 통해 가장 최근에 사용한 하드웨어 디바이스를 가져오고,
		// 이 디바이스의 주 디바이스 유형이 게임패드인지 확인합니다.
		FHardwareDeviceIdentifier MostRecentDevice = InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(GetPlatformUserId());
		if(MostRecentDevice.PrimaryDeviceType == EHardwareDevicePrimaryType::Gamepad)
		{
			return true;
		}
	}

	return false;
}
