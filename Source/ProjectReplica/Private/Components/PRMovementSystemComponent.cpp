// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRMovementSystemComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "AnimInstances/PRBaseAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

UPRMovementSystemComponent::UPRMovementSystemComponent()
{
	// Gait
	CurrentGait = EPRGait::Gait_Idle;
	LastGait = EPRGait::Gait_Idle;
	InitializeGaitSettings();

	// Aerial
	bActivateAerial = false;
	DefaultGravityScale = 0.0f;
}

UPRBaseAnimInstance* UPRMovementSystemComponent::GetPRBaseAnimInstance() const
{
	UPRBaseAnimInstance* PRBaseAnimInstance = nullptr;

	if(GetPROwner())
	{
		PRBaseAnimInstance = Cast<UPRBaseAnimInstance>(GetPROwner()->GetMesh()->GetAnimInstance());
	}

	return PRBaseAnimInstance;
}

#pragma region Gait
void UPRMovementSystemComponent::UpdateGait(EPRGait DesiredGait)
{
	if(GetPROwner())
	{
		LastGait = CurrentGait;
		CurrentGait = DesiredGait;

		if(GetPRBaseAnimInstance() && GetPRBaseAnimInstance()->ReceiveGait(DesiredGait))
		{
			ApplyGaitSettings(DesiredGait);
		}
	}
}

void UPRMovementSystemComponent::InitializeGaitSettings()
{
	GaitSettings.Empty();
	
	FPRGaitSettings WalkSettings = FPRGaitSettings(250.0f
													, 450.0f
													, 1.0f
													, 0.0f
													, true
													, 1000.0f);
	GaitSettings.Emplace(EPRGait::Gait_Walk, WalkSettings);
	
	FPRGaitSettings RunSettings = FPRGaitSettings(550.0f
												, 800.0f
												, 1.0f
												, 0.0f
												, true
												, 800.0f);
	GaitSettings.Emplace(EPRGait::Gait_Run, RunSettings);

	FPRGaitSettings SprintSettings = FPRGaitSettings(670.0f
												, 800.0f
												, 1.0f
												, 0.0f
												, true
												, 450.0f);
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
	
	return true;
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
			PR_LOG_SCREEN("Activate Aerial");
		}
		else
		{
			GetPROwner()->GetCharacterMovement()->GravityScale = DefaultGravityScale;
		}
	}
}
#pragma endregion 
