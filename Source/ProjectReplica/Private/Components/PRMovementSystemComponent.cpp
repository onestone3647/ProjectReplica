// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRMovementSystemComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "AnimInstances/PRBaseAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

UPRMovementSystemComponent::UPRMovementSystemComponent()
{
	CurrentGait = EPRGait::Gait_Idle;
	LastGait = EPRGait::Gait_Idle;
	
	InitializeGaitSettings();
}

void UPRMovementSystemComponent::UpdateGait(EPRGait DesiredGait)
{
	if(GetPROwner())
	{
		LastGait = CurrentGait;
		CurrentGait = DesiredGait;

		if(GetPRBaseAnimInstance() && GetPRBaseAnimInstance()->ReceiveGait(DesiredGait))
		{
			const FPRGaitSettings DesiredGaitSettings = GetGaitSettings(DesiredGait);
			
			UCharacterMovementComponent* CharacterMovement = GetPROwner()->GetCharacterMovement();
			CharacterMovement->MaxWalkSpeed = DesiredGaitSettings.MovementSpeed;
			CharacterMovement->MaxAcceleration = DesiredGaitSettings.MaxAcceleration;
			CharacterMovement->BrakingFrictionFactor = DesiredGaitSettings.BrakingFrictionFactor;
			CharacterMovement->BrakingFriction = DesiredGaitSettings.BrakingFriction;
			CharacterMovement->bUseSeparateBrakingFriction = DesiredGaitSettings.bUseSeparateBrakingFriction;
			CharacterMovement->BrakingDecelerationWalking = DesiredGaitSettings.BrakingDeceleration;
		}
	}
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
