// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimInstance.h"
#include "Common/PRCommonEnum.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/PRMovementSystemComponent.h"
#include "PRBaseAnimInstance.generated.h"

class APRBaseCharacter;
class UCharacterMovementComponent;

#pragma region Before
/**
 *  Locomotion의 상태를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRLocomotionState : uint8
{
	LocomotionState_Idle			UMETA(DisplayName = "Idle"),
	LocomotionState_Walk			UMETA(DisplayName = "Walk"),
	LocomotionState_Run				UMETA(DisplayName = "Run"),
	LocomotionState_Sprint			UMETA(DisplayName = "Sprint")
};

/**
 * 추적 상태를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRTrackState : uint8
{
	TrackState_OnEnter				UMETA(DisplayName = "OnEnter"),
	TrackState_OnExit				UMETA(DisplayName = "OnExit"),
	TrackState_WhileTrue			UMETA(DisplayName = "WhileTrue"),
	TrackState_WhileFalse			UMETA(DisplayName = "WhileFalse")
};

/**
 * 발을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRFoot : uint8
{
	EPRFoot_Left			UMETA(DisplayName = "LeftFoot"),
	EPRFoot_Right			UMETA(DisplayName = "RightFoot")
};
#pragma endregion

/**
 *  Root의 Yaw의 모드를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRRootYawMode : uint8
{
	RootYawMode_Accumulate			UMETA(DisplayName = "Accumulate"),
	RootYawMode_BlendOut			UMETA(DisplayName = "BlendOut"),
	RootYawMode_Hold				UMETA(DisplayName = "Hold")
};

/**
 * 캐릭터의 기본 AnimInstance 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPRBaseAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;
	// virtual void NativePostEvaluateAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

public:
	/**
	 * 걸음걸이를 받는 함수입니다.
	 *
	 * @param NewGait 받는 걸음걸이입니다.
	 * @return 받은 경우 true, 받지 않은 경우 false를 반환합니다.
	 */
	UFUNCTION(Blueprintable, Category = "PRBaseAnimInstance")
	bool ReceiveGait(EPRGait NewGait);

	/**
	 * RootLock을 설정하는 함수입니다.
	 * RootLock이 true일 경우 RootLock을 활성화합니다.
	 * RootLock이 false일 경우 RootLock을 비활성화합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance", meta = (BlueprintThreadSafe))
	void SetRootLock(bool bRootLock);

	/**
	 * RootLock을 활성화하는 함수입니다.
	 * 모든 애니메이션의 RootMotion을 실행합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance", meta = (BlueprintThreadSafe))
	void EnableRootLock();

	/**
	 * RootLock을 비활성화하는 함수입니다.
	 * 애님몽타주만 RootMotion을 실행합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance", meta = (BlueprintThreadSafe))
	void DisableRootLock();	

protected:
	/** 프로퍼티들을 최신화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance", meta = (BlueprintThreadSafe))
	void UpdateProperties(float DeltaSeconds);

	/** LocomotionState를 최신화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance")
	void UpdateLocomotionState();

	/** LocomotionState를 추적하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance")
	EPRTrackState TrackLocomotionState(EPRLocomotionState NewLocomotionState, bool& EnterExecuted, bool& ExitExecuted);

	/** Idle LocomotionState를 추적하는 함수입니다. */
	void TrackIdleState();

	/** Run LocomotionState를 추적하는 함수입니다. */
	void TrackRunState();

	/** Sprint LocomotionState를 추적하는 함수입니다. */
	void TrackSprintState();

	/** Walk LocomotionState를 추적하는 함수입니다. */
	void TrackWalkState();

	/** Run 상태에 진입했을 때 최신화하는 함수입니다. */
	void UpdateOnRunStateEnter();

	/** Walk 상태에 진입했을 때 최신화하는 함수입니다. */
	void UpdateOnWalkStateEnter();

	/** Locomotion의 재생 속도를 최신화하는 함수입니다. */
	void UpdateLocomotionPlayRate();

	/** 예상 정지 거리를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance", meta = (BlueprintThreadSafe))
	float GetPredictedStopDistance() const;
	
protected:
	/** 이전 프레임과 현재 프레임 사이의 시간 간격입니다. */
	UPROPERTY(BlueprintReadOnly, Category = "PRBaseAnimInstance")
	float DeltaTime;
	
	/** 캐릭터의 이동 상태입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	EPRLocomotionState LocomotionState;

	/** 제거 예정 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	EPRGait Gait;

	/** 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance")
	FVector Velocity;

	/** 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance")
	float Speed;
	
	/** 걷기 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance")
	float WalkSpeed;	

	/** 달리기 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance")
	float RunSpeed;

	/** 전력질주 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance")
	float SprintSpeed;
	
	/** 가속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance")
	FVector Acceleration;

	/** 달릴 때의 최소 가속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float MinAccelerationToRunGait;

	/** 움직이는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	bool bShouldMove;

	/** 방향입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float Direction;

	/** 공중에서 떨어지고 있는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	bool bIsFalling;

	/** 입력 벡터입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	FVector InputVector;
	
	/** 회전 시도 여부입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	bool bAttemptTurn;

private:
	/** 이 AnimInstance를 사용하는 PRBaseCharacter입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class APRBaseCharacter> PROwner;

	/** PROwner의 CharacterMovementComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCharacterMovementComponent> CharacterMovement;

	/** PROwner의 MovementSystem에서 가져온 걸음걸이의 설정 값을 가진 Map입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance", meta = (AllowPrivateAccess = "true"))
	TMap<EPRGait, FPRGaitSettings> GaitSettings;

private:
	/** TrackLocomotionState 함수에서 Idle 상태를 추적하는 Flag 변수입니다. */
	bool bTrackIdleStateEnterExecuted;
	bool bTrackIdleStateExitExecuted;

	/** TrackLocomotionState 함수에서 Run 상태를 추적하는 Flag 변수입니다. */
	bool bTrackRunStateEnterExecuted;
	bool bTrackRunStateExitExecuted;

	/** TrackLocomotionState 함수에서 Sprint 상태를 추적하는 Flag 변수입니다. */
	bool bTrackSprintStateEnterExecuted;
	bool bTrackSprintStateExitExecuted;

	/** TrackLocomotionState 함수에서 Walk 상태를 추적하는 Flag 변수입니다. */
	bool bTrackWalkStateEnterExecuted;
	bool bTrackWalkStateExitExecuted;
	
public:
	/** PROwner를 반환하는 함수입니다. */
	class APRBaseCharacter* GetPROwner() const;

	/** PROwner의 CharacterMovementComponent를 반환하는 함수입니다. */
	class UCharacterMovementComponent* GetCharacterMovement() const;





	

#pragma region LockOn
	// LockOn에서 사용할 것들
protected:
	/** Locomotion의 시작 방향을 최신화하는 함수입니다. */
	void UpdateLocomotionStartDirection(float NewStartAngle);

protected:
	/** 시작 회전값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	FRotator StartRotation;

	/** 1차 회전값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	FRotator PrimaryRotation;

	/** 2차 회전값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	FRotator SecondaryRotation;

	/** Locomotion의 시작 각(Angle)입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float LocomotionStartAngle;

#pragma endregion 













	


#pragma region Before2
protected:
	/** RootYawOffset을 설정하는 함수입니다. */	
	void SetRootYawOffset(float Angle);

	/** RootYawOffset을 최신화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance", meta = (BlueprintThreadSafe))
	void UpdateRootYawOffset(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance", meta = (BlueprintThreadSafe))
	void ProcessTurnYawCurve();

	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance", meta = (BlueprintThreadSafe))
	void SetOwnerRotation(FRotator NewRotation);

	
protected:
	/** Z축을 뺀 XY에서의 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	FVector VelocityXY;

	/** 지상에서의 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float GroundSpeed;

	/** Z축을 뺀 XY에서의 가속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	FVector AccelerationXY;

	/** 액터의 회전값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	FRotator ActorRotation;

	/** 움직이는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	bool bIsMoving;	

	/** 공중에 존재하는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	bool bIsInAir;

	/** 점프하고 있는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	bool bIsJumping;	

	/** 지상에 존재하는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	bool bIsOnGround;

	/** Actor의 Yaw값입니다. */ 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float ActorYaw;

	/** Actor의 마지막 Yaw값입니다. */ 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float LastActorYaw;

	/** Actor의 Yaw의 Delta값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float ActorYawDelta;	

	/** 경사 각도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float LeanAngle;

	/** 정점 점프 시간입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float ApexJumpTime;

	/** Root의 Yaw값의 Offset입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float RootYawOffset;

	/** Root의 Yaw값의 마지막 Offset입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float LastRootYawOffset;

	/** Root의 Yaw의 모드입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	EPRRootYawMode RootYawMode;

	/** 스프링 시스템의 상태입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	FFloatSpringState SpringState;

	/** Yaw의 회전값 커브 값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float TurnYawCurveValue;

	/** Yaw의 마지막 회전값 커브 값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float LastTurnYawCurveValue;

	/** Yaw의 회전값 커브의 이름입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	FName TurnYawCurveName;

	/** 남은 Yaw의 회전값 커브의 이름입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	FName RemainingTurnYawCurveName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float ForwardValue;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float BackwardValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float LeftValue;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float RightValue;
#pragma endregion 
	
#pragma region Before
public:
	/** 입력받은 인자가 LocomotionState와 같은지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance")
	bool IsEqualLocomotionState(EPRLocomotionState NewLocomotionState) const;
	
protected:
	/**
	 * 필수 속성을 설정하는 함수입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance", meta = (BlueprintThreadSafe))
	void SetupEssentialProperties();

	/** Idle 상태를 최신화하는 함수입니다. */
	void UpdateIdleState();

	/** Run 상태를 최신화하는 함수입니다. */
	void UpdateRunState();

	/** Walk 상태를 최신화하는 함수입니다. */
	void UpdateWalkState();

	// void UpdateCharacterRotation();
	//
	// void UpdateRotationWhileMoving();
	
protected:

	/** 가속 여부입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	bool bHasAcceleration;

	// /** 가속도입니다. */
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	// FVector Acceleration;

	/** 달리기 상태의 최소 가속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float MinAccelerationToRunState;	

	/** 2D상의 가속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	FVector2D Acceleration2D;

	/** 움직이는 방향입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	EPRDirection MovementDirection;

	/** 애니메이션의 재생 속도입니다. */	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float PlayRate;

	/** Distance Matching에서 사용하는 일치하는 거리입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float DistanceToMatch;

	/** 마지막으로 바닥에 닿은 발입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	EPRFoot LastFootOnLand;

#pragma endregion 

public:

	/** 입력받은 인자로 LastFootOnLand를 설정하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance")
	void SetLastFootOnLand(EPRFoot NewLastFootOnLand);
};
