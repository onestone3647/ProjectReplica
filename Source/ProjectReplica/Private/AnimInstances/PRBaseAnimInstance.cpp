// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/PRBaseAnimInstance.h"
#include "Characters/PRBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimCharacterMovementLibrary.h"
#include "KismetAnimationLibrary.h"
#include "Components/PRMovementSystemComponent.h"

UPRBaseAnimInstance::UPRBaseAnimInstance()
{
	PROwner = nullptr;
	CharacterMovement = nullptr;

	Gait = EPRGait::Gait_Idle;
	Velocity = FVector::ZeroVector;
	VelocityXY = FVector::ZeroVector;
	GroundSpeed = 0.0f;
	Acceleration = FVector::ZeroVector;
	AccelerationXY = FVector::ZeroVector;
	ActorRotation = FRotator::ZeroRotator;
	Direction = 0.0f;
	bShouldMove = false;
	bIsFalling = false;
	bIsInAir = false;
	bIsJumping = false;
	bIsOnGround = true;
	ActorYaw = 0.0f;
	ActorYawDelta = 0.0f;
	LeanAngle = 0.0f;
	ApexJumpTime = 0.0f;
	RootYawOffset = 0.0f;
	LastRootYawOffset = 0.0f;
	RootYawMode = EPRRootYawMode::RootYawMode_Accumulate;
	SpringState = FFloatSpringState();
	TurnYawCurveValue = 0.0f;
	LastTurnYawCurveValue = 0.0f;
	TurnYawCurveName = TEXT("TurnYawWeight");
	RemainingTurnYawCurveName = TEXT("RemainingTurnYaw");
	
	// DeltaTime = 0.0f;
	// LocomotionState = EPRLocomotionState::LocomotionState_Idle;
	// Velocity = FVector::ZeroVector;
	// // bHasAcceleration = false;
	// Acceleration = FVector::ZeroVector;
	// MinAccelerationToRunState = 400.0f;
	// // Acceleration2D = FVector2D::ZeroVector;
	// Speed = 0.0f;
	// WalkSpeed = 0.0f;
	// InputVector = FVector::ZeroVector;
	// StartRotation = FRotator::ZeroRotator;
	// PrimaryRotation = FRotator::ZeroRotator;
	// SecondaryRotation = FRotator::ZeroRotator;
	// LocomotionStartAngle = 0.0f;
	// MovementDirection = EPRDirection::Direction_Forward;
	// PlayRate = 0.0f;
	// DistanceToMatch = 0.0f;
	// LastFootOnLand = EPRFoot::EPRFoot_Left;
	//
	// bTrackIdleStateEnterExecuted = false;
	// bTrackIdleStateExitExecuted = false;
	//
	// bTrackRunStateEnterExecuted = false;
	// bTrackRunStateExitExecuted = false;
	//
	// bTrackWalkStateEnterExecuted = false;
	// bTrackWalkStateExitExecuted = false;
}

void UPRBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APRBaseCharacter* NewPROwner = Cast<APRBaseCharacter>(TryGetPawnOwner());
	if(NewPROwner)
	{
		PROwner = NewPROwner;
		CharacterMovement = PROwner->GetCharacterMovement();
		WalkSpeed = PROwner->GetWalkSpeed();
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

	// // LocomotionState 최신화
	// UpdateLocomotionState();
	//
	// UpdateIdleState();
	// UpdateRunState();
	// UpdateWalkState();
}

void UPRBaseAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	UpdateProperties(DeltaSeconds);
	UpdateRootYawOffset(DeltaSeconds);

	// SetupEssentialProperties();
	
	// DistanceToMatch = GetPredictedStopDistance();
}

bool UPRBaseAnimInstance::ReceiveGait(EPRGait NewGait)
{
	Gait = NewGait;
	
	return true;
}

void UPRBaseAnimInstance::UpdateProperties(float DeltaSeconds)
{
	if(GetCharacterMovement())
	{
		Acceleration = GetCharacterMovement()->GetCurrentAcceleration();
		Velocity = GetCharacterMovement()->Velocity;
		GroundSpeed = Velocity.Length();
		ActorRotation = TryGetPawnOwner()->GetActorRotation();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, ActorRotation);
		VelocityXY = Velocity * FVector(1.0f, 1.0f, 0.0f);

		bShouldMove = GroundSpeed > KINDA_SMALL_NUMBER && Acceleration != FVector::ZeroVector;		// 3.0f
		bIsFalling = GetCharacterMovement()->IsFalling();
		bIsOnGround = GetCharacterMovement()->IsMovingOnGround();
		
		AccelerationXY = Acceleration * FVector(1.0f, 1.0f, 0.0f);

		LastActorYaw = ActorYaw;
		ActorYaw = ActorRotation.Yaw;
		ActorYawDelta = ActorYaw - LastActorYaw;

		float MovementDirectionValue = 0.0f;
		switch(MovementDirection)
		{
		case EPRDirection::Direction_Forward:
		case EPRDirection::Direction_Left:
			MovementDirectionValue = 1.0f;
			break;
			
		case EPRDirection::Direction_Backward:
		case EPRDirection::Direction_Right:
			MovementDirectionValue = -1.0f;
			break;
			
		default:
			MovementDirectionValue = 0.0f;
			break;
		}
		
		LeanAngle = UKismetMathLibrary::ClampAngle(UKismetMathLibrary::SafeDivide(ActorYawDelta, DeltaTime) / 4.0f * MovementDirectionValue,
																					-45.0f, 45.0f);

		bIsInAir = GetCharacterMovement()->MovementMode == MOVE_Falling;
		bIsJumping = bIsInAir && Velocity.Z > 0.0f;
		bIsFalling = bIsInAir && Velocity.Z < 0.0f;
		if(bIsJumping)
		{
			ApexJumpTime = (0.0f - Velocity.Z) / (GetCharacterMovement()->GetGravityZ() * GetCharacterMovement()->GravityScale);
		}
		else
		{
			ApexJumpTime = 0.0f;
		}
	}
}

void UPRBaseAnimInstance::SetRootYawOffset(float Angle)
{
	LastRootYawOffset = RootYawOffset;
	RootYawOffset = UKismetMathLibrary::NormalizeAxis(Angle);
}

void UPRBaseAnimInstance::UpdateRootYawOffset(float DeltaSeconds)
{
	if(RootYawMode == EPRRootYawMode::RootYawMode_Accumulate)
	{
		const float NewAngle = RootYawOffset + (ActorYawDelta * -1.0f);
		SetRootYawOffset(NewAngle);
	}

	if(RootYawMode == EPRRootYawMode::RootYawMode_BlendOut)
	{
		const float SpringInterp = UKismetMathLibrary::FloatSpringInterp(RootYawOffset, 0.0f, SpringState,
																		80.0f, 1.0f, DeltaSeconds,
																		1.0f, 0.5);
		SetRootYawOffset(SpringInterp);
	}

	RootYawMode = EPRRootYawMode::RootYawMode_BlendOut;
}

void UPRBaseAnimInstance::ProcessTurnYawCurve()
{
	LastTurnYawCurveValue = TurnYawCurveValue;
	if(GetCurveValue(TurnYawCurveName) < KINDA_SMALL_NUMBER)		// 1.0f
	{
		TurnYawCurveValue = 0.0f;
		LastTurnYawCurveValue = 0.0f;
	}
	else
	{
		TurnYawCurveValue = UKismetMathLibrary::SafeDivide(GetCurveValue(RemainingTurnYawCurveName), GetCurveValue(TurnYawCurveName));

		if(LastTurnYawCurveValue != 0.0f)
		{
			SetRootYawOffset(RootYawOffset - (TurnYawCurveValue - LastTurnYawCurveValue));
		}
	}
}

bool UPRBaseAnimInstance::IsEqualLocomotionState(EPRLocomotionState NewLocomotionState) const
{
	return LocomotionState == NewLocomotionState;
}

void UPRBaseAnimInstance::SetupEssentialProperties()
{
	if(GetCharacterMovement())
	{
		Velocity = GetCharacterMovement()->Velocity;
		Acceleration = GetCharacterMovement()->GetCurrentAcceleration();
		// Acceleration2D = FVector2D(Acceleration.X, Acceleration.Y);
		// bHasAcceleration = !Acceleration.IsNearlyZero();
		// bHasAcceleration = !Acceleration2D.IsNearlyZero();
		Speed = Velocity.Size();
		InputVector = GetCharacterMovement()->GetLastInputVector();
	}
}

void UPRBaseAnimInstance::UpdateLocomotionState()
{
	if(GetCharacterMovement())
	{
		// 정규화된 속도 및 가속도를 계산
		const FVector NormalizeVelocity = UKismetMathLibrary::Normal(Velocity);
		const FVector NormalizeAcceleration = UKismetMathLibrary::Normal(Acceleration);

		// 정규화된 속도롸 가속도의 내적 계산
		const float Dot = UKismetMathLibrary::Dot_VectorVector(NormalizeVelocity, NormalizeAcceleration);
		
		if(Dot < 0.0f)
		{
			// 속도와 가속도가 반대 방향이면 Idle 상태
			LocomotionState = EPRLocomotionState::LocomotionState_Idle;
		}
		else
		{
			// 속도와 가속도에 기반하여 Locomotion 상태를 결정합니다.
			if(Speed > KINDA_SMALL_NUMBER			// or 1.0f
				&& Acceleration.Size() > MinAccelerationToRunState
				&& GetCharacterMovement()->MaxWalkSpeed > WalkSpeed + 5.0f)
			{
				LocomotionState = EPRLocomotionState::LocomotionState_Run;
			}
			else if(Speed > KINDA_SMALL_NUMBER										// or 1.0f
				&& Acceleration.Size() > KINDA_SMALL_NUMBER							// or 0.01f
				&& GetCharacterMovement()->MaxWalkSpeed > KINDA_SMALL_NUMBER)		// or 1.0f
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

			// Locomotion에 들어갑니다.
			return EPRTrackState::TrackState_OnEnter;
		}

		// Locomotion을 지속 상태입니다.
		return EPRTrackState::TrackState_WhileTrue;
	}
	else
	{
		if(!ExitExecuted)
		{
			ExitExecuted = true;
			EnterExecuted = false;

			// Locomotion을 탈출합니다.
			return EPRTrackState::TrackState_OnExit;
		}

		// Locomotion 지속 상태가 아닙니다.
		return EPRTrackState::TrackState_WhileFalse;
	}
}

void UPRBaseAnimInstance::UpdateIdleState()
{
	const EPRTrackState TrackIdle = TrackLocomotionState(EPRLocomotionState::LocomotionState_Idle, bTrackIdleStateEnterExecuted, bTrackIdleStateExitExecuted);
	switch(TrackIdle)
	{
	case EPRTrackState::TrackState_OnEnter:
		break;
	case EPRTrackState::TrackState_OnExit:
		break;
	case EPRTrackState::TrackState_WhileTrue:
		break;
	case EPRTrackState::TrackState_WhileFalse:
		break;
	default:
		break;
	}
}

void UPRBaseAnimInstance::UpdateRunState()
{
	const EPRTrackState TrackRun = TrackLocomotionState(EPRLocomotionState::LocomotionState_Run, bTrackRunStateEnterExecuted, bTrackRunStateExitExecuted);
	switch(TrackRun)
	{
	case EPRTrackState::TrackState_OnEnter:
		UpdateOnRunEnter();
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

void UPRBaseAnimInstance::UpdateWalkState()
{
	const EPRTrackState TrackWalk = TrackLocomotionState(EPRLocomotionState::LocomotionState_Walk, bTrackWalkStateEnterExecuted, bTrackWalkStateExitExecuted);
	switch(TrackWalk)
	{
	case EPRTrackState::TrackState_OnEnter:
		UpdateOnWalkEnter();
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

void UPRBaseAnimInstance::UpdateOnRunEnter()
{
	if(GetPROwner())
	{
		StartRotation = TryGetPawnOwner()->GetActorRotation();
		PrimaryRotation = UKismetMathLibrary::Conv_VectorToRotator(InputVector);
		SecondaryRotation = PrimaryRotation;
		LocomotionStartAngle = UKismetMathLibrary::NormalizedDeltaRotator(PrimaryRotation, StartRotation).Yaw;

		// Locomotion의 시작 방향을 최신화합니다.
		UpdateLocomotionStartDirection(LocomotionStartAngle);
	}
}

void UPRBaseAnimInstance::UpdateOnWalkEnter()
{
	if(GetPROwner())
	{
		StartRotation = TryGetPawnOwner()->GetActorRotation();
		PrimaryRotation = UKismetMathLibrary::Conv_VectorToRotator(InputVector);
		SecondaryRotation = PrimaryRotation;
		LocomotionStartAngle = UKismetMathLibrary::NormalizedDeltaRotator(PrimaryRotation, StartRotation).Yaw;

		// Locomotion의 시작 방향을 최신화합니다.
		UpdateLocomotionStartDirection(LocomotionStartAngle);
	}
}

void UPRBaseAnimInstance::UpdateLocomotionStartDirection(float NewStartAngle)
{
	if(UKismetMathLibrary::InRange_FloatFloat(NewStartAngle, -60.0f, 60.0f))
	{
		MovementDirection = EPRDirection::Direction_Forward;
	}
	else if(UKismetMathLibrary::InRange_FloatFloat(NewStartAngle, 60.0f, 120.0f))
	{
		MovementDirection = EPRDirection::Direction_Right;
	}
	else if(UKismetMathLibrary::InRange_FloatFloat(NewStartAngle, -120.0f, -60.0f))
	{
		MovementDirection = EPRDirection::Direction_Left;
	}
	else if(UKismetMathLibrary::InRange_FloatFloat(NewStartAngle, 121.0f, 180.0f))
	{
		MovementDirection = EPRDirection::Direction_BackwardRight;
	}
	else
	{
		MovementDirection = EPRDirection::Direction_BackwardLeft;
	}
}

void UPRBaseAnimInstance::UpdateLocomotionPlayRate()
{
	float DivideSpeed = UKismetMathLibrary::SafeDivide(Speed, GetCurveValue(TEXT("MovingSpeed")));
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

// void UPRBaseAnimInstance::UpdateCharacterRotation()
// {
// 	if(IsEqualLocomotionState(EPRLocomotionState::LocomotionState_Idle))
// 	{
// 		
// 	}
// 	else
// 	{
// 		UpdateRotationWhileMoving();
// 	}
// }
//
// void UPRBaseAnimInstance::UpdateRotationWhileMoving()
// {
// 	const FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(InputVector);
// 	PrimaryRotation = UKismetMathLibrary::RInterpTo_Constant(PrimaryRotation, TargetRotation, DeltaTime, 1000.0f);
// 	SecondaryRotation = UKismetMathLibrary::RInterpTo_Constant(SecondaryRotation, PrimaryRotation, DeltaTime, 8.0f);
//
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

void UPRBaseAnimInstance::SetLastFootOnLand(EPRFoot NewLastFootOnLand)
{
	LastFootOnLand = NewLastFootOnLand;
}
