// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRMovementSystemComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UPRMovementSystemComponent::UPRMovementSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	// Gait
	AllowGait = EPRGait::Gait_Run;
	CurrentGait = EPRGait::Gait_Idle;
	LastGait = EPRGait::Gait_Idle;
	InitializeGaitSettings();
	Velocity = FVector::ZeroVector;
	GroundSpeed = 0.0f;
	Acceleration = FVector::ZeroVector;
	MinAccelerationToRunGait = 400.0f;

	// Aerial
	bActivateAerial = false;
	DefaultGravityScale = 0.0f;
}

void UPRMovementSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateGait();

	if(GetPROwner())
	{
		// 전력질주 후 정지했을 경우 달리기 상태로 설정합니다.
		if(GetPROwner()->GetCharacterMovement()->GetLastUpdateVelocity() == FVector::ZeroVector
			&& IsEqualAllowGait(EPRGait::Gait_Sprint)
			&& IsEqualLastGait(EPRGait::Gait_Sprint))
		{
			SetAllowGait(EPRGait::Gait_Run);
		}
	}
}

#pragma region Gait
void UPRMovementSystemComponent::UpdateGait()
{
	if(GetPROwner())
	{
		Velocity = GetPROwner()->GetCharacterMovement()->Velocity;
		GroundSpeed = Velocity.Size2D();
		Acceleration = GetPROwner()->GetCharacterMovement()->GetCurrentAcceleration();
		
		if(GroundSpeed > 0.0f			// or 1.0f
			&& Acceleration.Size() > MinAccelerationToRunGait
			&& GetPROwner()->GetCharacterMovement()->MaxWalkSpeed > GaitSettings.Find(EPRGait::Gait_Walk)->MovementSpeed
			&& GetPROwner()->GetCharacterMovement()->MaxWalkSpeed < GaitSettings.Find(EPRGait::Gait_Sprint)->MovementSpeed)
		{
			SetCurrentGait(EPRGait::Gait_Run);
		}
		else if(GroundSpeed > 0.0f			// or 1.0f
			&& Acceleration.Size() > MinAccelerationToRunGait
			&& GetPROwner()->GetCharacterMovement()->MaxWalkSpeed > GaitSettings.Find(EPRGait::Gait_Run)->MovementSpeed)
		{
			SetCurrentGait(EPRGait::Gait_Sprint);
		}
		else if(GroundSpeed > 0.0f												// or 1.0f
			&& Acceleration.Size() > 0.0f										// or 0.01f
			&& GetPROwner()->GetCharacterMovement()->MaxWalkSpeed > 0.0f)		// or 1.0f
		{
			SetCurrentGait(EPRGait::Gait_Walk);
		}
		else
		{
			SetCurrentGait(EPRGait::Gait_Idle);
		}
	}
}

void UPRMovementSystemComponent::InitializeGaitSettings()
{
	GaitSettings.Empty();
	
	FPRGaitSettings WalkSettings = FPRGaitSettings(250.0f
													, 2048.0f
													, 1.0f
													, 0.0f
													, true
													, 1000.0f
													, 250.0f);
	GaitSettings.Emplace(EPRGait::Gait_Walk, WalkSettings);
	
	FPRGaitSettings RunSettings = FPRGaitSettings(550.0f
												, 2048.0f
												, 1.0f
												, 0.0f
												, true
												, 1000.0f
												, 250.0f);
	GaitSettings.Emplace(EPRGait::Gait_Run, RunSettings);

	FPRGaitSettings SprintSettings = FPRGaitSettings(670.0f
												, 2048.0f
												, 1.0f
												, 0.0f
												, true
												, 700.0f
												, 250.0f);
	GaitSettings.Emplace(EPRGait::Gait_Sprint, SprintSettings);
}

FPRGaitSettings UPRMovementSystemComponent::GetGaitSettings(EPRGait NewGait) const
{
	return *GaitSettings.Find(NewGait);
}

TMap<EPRGait, FPRGaitSettings> UPRMovementSystemComponent::GetAllGaitSettings() const
{
	return GaitSettings;
}

bool UPRMovementSystemComponent::ApplyGaitSettings(EPRGait ApplyGait)
{
	// 인자에 해당하는 걸음걸이가 없거나 캐릭터가 CharacterMovementComponent가 없을 경우
	if(!GaitSettings.Contains(ApplyGait)
		|| !IsValid(GetPROwner())
		|| !GetPROwner()->GetCharacterMovement())
	{
		return false;
	}

	const FPRGaitSettings NewApplyGaitSettings = GetGaitSettings(ApplyGait);
	UCharacterMovementComponent* CharacterMovement = GetPROwner()->GetCharacterMovement();
	CharacterMovement->MaxWalkSpeed = NewApplyGaitSettings.MovementSpeed;
	CharacterMovement->MaxAcceleration = NewApplyGaitSettings.MaxAcceleration;
	CharacterMovement->BrakingFrictionFactor = NewApplyGaitSettings.BrakingFrictionFactor;
	CharacterMovement->BrakingFriction = NewApplyGaitSettings.BrakingFriction;
	CharacterMovement->bUseSeparateBrakingFriction = NewApplyGaitSettings.bUseSeparateBrakingFriction;
	CharacterMovement->BrakingDecelerationWalking = NewApplyGaitSettings.BrakingDeceleration;
	CharacterMovement->MinAnalogWalkSpeed = NewApplyGaitSettings.MinAnalogWalkSpeed;
	
	return true;
}

bool UPRMovementSystemComponent::IsEqualAllowGait(EPRGait NewGait) const
{
	return AllowGait == NewGait;
}

bool UPRMovementSystemComponent::IsEqualCurrentGait(EPRGait NewGait) const
{
	return CurrentGait == NewGait;
}

bool UPRMovementSystemComponent::IsEqualLastGait(EPRGait NewGait) const
{
	return LastGait == NewGait;
}

void UPRMovementSystemComponent::SetCurrentGait(EPRGait NewGait)
{
	if(CurrentGait != NewGait)
	{
		LastGait = CurrentGait;
		CurrentGait = NewGait;
	}
}

EPRGait UPRMovementSystemComponent::GetAllowGait() const
{
	return AllowGait;
}

void UPRMovementSystemComponent::SetAllowGait(EPRGait NewGait)
{
	AllowGait = NewGait;
	ApplyGaitSettings(NewGait);
}

EPRGait UPRMovementSystemComponent::GetCurrentGait() const
{
	return CurrentGait;
}

float UPRMovementSystemComponent::GetMinAccelerationToRunGait() const
{
	return MinAccelerationToRunGait;
}
#pragma endregion 

#pragma region Aerial
void UPRMovementSystemComponent::ActivateAerial(bool bNewActivateAerial)
{
	if(GetPROwner())
	{
		bActivateAerial = bNewActivateAerial;
		if(bNewActivateAerial)
		{
			DefaultGravityScale = GetPROwner()->GetCharacterMovement()->GravityScale;
			GetPROwner()->GetCharacterMovement()->StopMovementImmediately();
			GetPROwner()->GetCharacterMovement()->GravityScale = 0.0f;
		}
		else
		{
			GetPROwner()->GetCharacterMovement()->GravityScale = DefaultGravityScale;
		}
	}
}
#pragma endregion 
