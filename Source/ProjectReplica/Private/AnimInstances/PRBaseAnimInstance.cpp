// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/PRBaseAnimInstance.h"
#include "Characters/PRBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimCharacterMovementLibrary.h"
#include "KismetAnimationLibrary.h"
#include "Dataflow/DataflowSelection.h"

UPRBaseAnimInstance::UPRBaseAnimInstance()
{
	PROwner = nullptr;
	CharacterMovement = nullptr;
	GaitSettings.Empty();

	DeltaTime = 0.0f;
	LocomotionState = EPRLocomotionState::LocomotionState_Idle;
	Gait = EPRGait::Gait_Idle;
	Velocity = FVector::ZeroVector;
	GroundSpeed = 0.0f;
	WalkSpeed = 0.0f;
	RunSpeed = 0.0f;
	SprintSpeed = 0.0f;
	Acceleration = FVector::ZeroVector;
	MinAccelerationToRunGait = 400.0f;
	bShouldMove = false;
	Direction = 0.0f;
	bIsFalling = false;
	InputVector = FVector::ZeroVector;
	bAttemptTurn = false;
	PlayRate = 0.0f;
	DistanceToMatch = 0.0f;

	bTrackIdleStateEnterExecuted = false;
	bTrackIdleStateExitExecuted = false;
	bTrackRunStateEnterExecuted = false;
	bTrackRunStateExitExecuted = false;
	bTrackSprintStateEnterExecuted = false;
	bTrackSprintStateExitExecuted = false;
	bTrackWalkStateEnterExecuted = false;
	bTrackWalkStateExitExecuted = false;
}

void UPRBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APRBaseCharacter* NewPROwner = Cast<APRBaseCharacter>(TryGetPawnOwner());
	if(NewPROwner)
	{
		PROwner = NewPROwner;
		CharacterMovement = NewPROwner->GetCharacterMovement();
		if(NewPROwner->GetMovementSystem())
		{
			GaitSettings = NewPROwner->GetMovementSystem()->GetAllGaitSettings();
			WalkSpeed = NewPROwner->GetMovementSystem()->GetGaitSettings(EPRGait::Gait_Walk).MovementSpeed;
			RunSpeed = NewPROwner->GetMovementSystem()->GetGaitSettings(EPRGait::Gait_Run).MovementSpeed;
			SprintSpeed = NewPROwner->GetMovementSystem()->GetGaitSettings(EPRGait::Gait_Sprint).MovementSpeed;
		}
	}
}

// void UPRBaseAnimInstance::NativePostEvaluateAnimation()
// {
// 	Super::NativePostEvaluateAnimation();
//
// 	
// }

void UPRBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	DeltaTime = DeltaSeconds;
	
	// LocomotionState 최신화
	UpdateLocomotionState();
	
	TrackIdleState();
	TrackRunState();
	TrackSprintState();
	TrackWalkState();
}

void UPRBaseAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	UpdateProperties(DeltaSeconds);

	if(LocomotionState == EPRLocomotionState::LocomotionState_Idle)
	{
		// Idle 상태로 돌아갈 때 StopDistance를 계산합니다.
		DistanceToMatch = GetPredictedStopDistance();
	}
	else
	{
		DistanceToMatch = 0.0f;
	}
}

bool UPRBaseAnimInstance::ReceiveGait(EPRGait NewGait)
{
	Gait = NewGait;
	
	return true;
}

void UPRBaseAnimInstance::SetRootLock(bool bRootLock)
{
	if(bRootLock)
	{
		EnableRootLock();
	}
	else
	{
		DisableRootLock();
	}
}

void UPRBaseAnimInstance::EnableRootLock()
{
	SetRootMotionMode(ERootMotionMode::RootMotionFromEverything);
}

void UPRBaseAnimInstance::DisableRootLock()
{
	SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
}

void UPRBaseAnimInstance::UpdateProperties(float DeltaSeconds)
{
	if(GetCharacterMovement())
	{
		Velocity = GetCharacterMovement()->Velocity;
		GroundSpeed = Velocity.Size2D();
		Acceleration = GetCharacterMovement()->GetCurrentAcceleration();
		bShouldMove = GroundSpeed > 3.0f && Acceleration != FVector::ZeroVector;
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, TryGetPawnOwner()->GetActorRotation());
		InputVector = GetCharacterMovement()->GetLastInputVector();

		bIsFalling = GetCharacterMovement()->IsFalling();

		// QuickTurn 잠시 보류
		// bAttemptTurn = UKismetMathLibrary::Dot_VectorVector(UKismetMathLibrary::Normal(Velocity), UKismetMathLibrary::Normal(Acceleration)) < 0.0f;
	}
}

void UPRBaseAnimInstance::UpdateLocomotionState()
{
	if(GetCharacterMovement())
	{
		// 정규화된 속도 및 가속도를 계산
		const FVector NormalizeVelocity = UKismetMathLibrary::Normal(Velocity);
		const FVector NormalizeAcceleration = UKismetMathLibrary::Normal(Acceleration);
	
		// 정규화된 속도와 가속도의 내적 계산
		const float Dot = UKismetMathLibrary::Dot_VectorVector(NormalizeVelocity, NormalizeAcceleration);
		
		if(Dot < 0.0f)
		{
			// 속도와 가속도가 반대 방향이면 Idle 상태
			LocomotionState = EPRLocomotionState::LocomotionState_Idle;
		}
		else
		{
			// // 속도와 가속도에 기반하여 Locomotion 상태를 결정합니다.
			// if(GroundSpeed > 0.0f			// or 1.0f
			// 	&& Acceleration.Size() > MinAccelerationToRunGait
			// 	&& GetCharacterMovement()->MaxWalkSpeed > GaitSettings.Find(EPRGait::Gait_Walk)->MovementSpeed + 5.0f)
			// {
			// 	LocomotionState = EPRLocomotionState::LocomotionState_Run;
			// }
			// else if(GroundSpeed > 0.0f									// or 1.0f
			// 	&& Acceleration.Size() > 0.0f							// or 0.01f
			// 	&& GetCharacterMovement()->MaxWalkSpeed > 0.0f)			// or 1.0f
			// {
			// 	LocomotionState = EPRLocomotionState::LocomotionState_Walk;
			// }
			// else
			// {
			// 	LocomotionState = EPRLocomotionState::LocomotionState_Idle;
			// }
			// 속도와 가속도에 기반하여 Locomotion 상태를 결정합니다.
			if(GroundSpeed > 0.0f			// or 1.0f
				&& Acceleration.Size() > MinAccelerationToRunGait
				&& GetCharacterMovement()->MaxWalkSpeed > GaitSettings.Find(EPRGait::Gait_Walk)->MovementSpeed
				&& GetCharacterMovement()->MaxWalkSpeed < GaitSettings.Find(EPRGait::Gait_Sprint)->MovementSpeed)
			{
				LocomotionState = EPRLocomotionState::LocomotionState_Run;
			}
			else if(GroundSpeed > 0.0f			// or 1.0f
				&& Acceleration.Size() > MinAccelerationToRunGait
				&& GetCharacterMovement()->MaxWalkSpeed > GaitSettings.Find(EPRGait::Gait_Run)->MovementSpeed)
			{
				LocomotionState = EPRLocomotionState::LocomotionState_Sprint;
			}
			else if(GroundSpeed > 0.0f									// or 1.0f
				&& Acceleration.Size() > 0.0f							// or 0.01f
				&& GetCharacterMovement()->MaxWalkSpeed > 0.0f)			// or 1.0f
			{
				LocomotionState = EPRLocomotionState::LocomotionState_Walk;
			}
			else
			{
				LocomotionState = EPRLocomotionState::LocomotionState_Idle;
			}
		}
	}
}

EPRTrackState UPRBaseAnimInstance::TrackLocomotionState(EPRLocomotionState NewLocomotionState, bool& EnterExecuted, bool& ExitExecuted)
{
	if(LocomotionState == NewLocomotionState)
	{
		if(!EnterExecuted)
		{
			EnterExecuted = true;
			ExitExecuted = false;

			// Gait에 들어갑니다.
			return EPRTrackState::TrackState_OnEnter;
		}

		// Gait 지속 상태입니다.
		return EPRTrackState::TrackState_WhileTrue;
	}
	else
	{
		if(!ExitExecuted)
		{
			ExitExecuted = true;
			EnterExecuted = false;

			// Gait를 탈출합니다.
			return EPRTrackState::TrackState_OnExit;
		}

		// Gait 지속 상태가 아닙니다.
		return EPRTrackState::TrackState_WhileFalse;
	}
}

void UPRBaseAnimInstance::TrackIdleState()
{
}

void UPRBaseAnimInstance::TrackRunState()
{
	const EPRTrackState TrackRun = TrackLocomotionState(EPRLocomotionState::LocomotionState_Run, bTrackRunStateEnterExecuted, bTrackRunStateExitExecuted);
	switch(TrackRun)
	{
	case EPRTrackState::TrackState_OnEnter:
		break;
	case EPRTrackState::TrackState_OnExit:
		break;
	case EPRTrackState::TrackState_WhileTrue:
		UpdateLocomotionPlayRate();
		break;
	case EPRTrackState::TrackState_WhileFalse:
		break;
	default:
		break;
	}
}

void UPRBaseAnimInstance::TrackSprintState()
{
	const EPRTrackState TrackSprint = TrackLocomotionState(EPRLocomotionState::LocomotionState_Sprint, bTrackSprintStateEnterExecuted, bTrackSprintStateExitExecuted);
	switch(TrackSprint)
	{
	case EPRTrackState::TrackState_OnEnter:
		break;
	case EPRTrackState::TrackState_OnExit:
		// InitializeSprintState();
		break;
	case EPRTrackState::TrackState_WhileTrue:
		UpdateLocomotionPlayRate();
		break;
	case EPRTrackState::TrackState_WhileFalse:
		break;
	default:
		break;
	}
}

void UPRBaseAnimInstance::TrackWalkState()
{
	const EPRTrackState TrackWalk = TrackLocomotionState(EPRLocomotionState::LocomotionState_Walk, bTrackWalkStateEnterExecuted, bTrackWalkStateExitExecuted);
	switch(TrackWalk)
	{
	case EPRTrackState::TrackState_OnEnter:
		break;
	case EPRTrackState::TrackState_OnExit:
		break;
	case EPRTrackState::TrackState_WhileTrue:
		UpdateLocomotionPlayRate();
		break;
	case EPRTrackState::TrackState_WhileFalse:
		break;
	default:
		break;
	}
}

void UPRBaseAnimInstance::UpdateLocomotionPlayRate()
{
	float DivideSpeed = UKismetMathLibrary::SafeDivide(GroundSpeed, GetCurveValue(TEXT("MovingSpeed")));
	PlayRate = UKismetMathLibrary::FClamp(DivideSpeed, 0.5f, 1.75f);	
}

float UPRBaseAnimInstance::GetPredictedStopDistance() const
{
	float NewDistanceToMatch = 0.0f;
	
	if(GetCharacterMovement())
	{
		FVector MovementStopLocation = UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(GetCharacterMovement()->Velocity,
																										GetCharacterMovement()->bUseSeparateBrakingFriction,
																										GetCharacterMovement()->BrakingFriction,
																										GetCharacterMovement()->GroundFriction,
																										GetCharacterMovement()->BrakingFrictionFactor,
																										GetCharacterMovement()->BrakingDecelerationWalking);

		NewDistanceToMatch = MovementStopLocation.Length();
	}
	
	return NewDistanceToMatch;
}

// void UPRBaseAnimInstance::InitializeSprintState()
// {
// 	// 전력 질주 상태에서 벗어나면 달리기 상태로 초기화합니다.
// 	if(IsValid(GetPROwner()) && GetPROwner()->GetMovementSystem())
// 	{
// 		GetPROwner()->GetMovementSystem()->ApplyGaitSettings(EPRGait::Gait_Run);
// 	}
// }

APRBaseCharacter* UPRBaseAnimInstance::GetPROwner() const
{
	return PROwner;
}

UCharacterMovementComponent* UPRBaseAnimInstance::GetCharacterMovement() const
{
	if(GetPROwner())
	{
		return GetPROwner()->GetCharacterMovement();
	}

	return nullptr;
}
