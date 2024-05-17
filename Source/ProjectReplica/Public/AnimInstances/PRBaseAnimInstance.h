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

	/** 허용한 걸을걸이입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementSystem")
	EPRGait AllowGait;	

	/** 현재 걸음걸이입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	EPRGait CurrentGait;

	/** 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance")
	FVector Velocity;

	/** 지상에서의 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance")
	float GroundSpeed;	
	
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

	/** 애니메이션의 재생 속도입니다. */	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float PlayRate;

	/** Distance Matching에서 사용하는 일치하는 거리입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float DistanceToMatch;

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
};
