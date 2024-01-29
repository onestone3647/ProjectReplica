// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimInstance.h"
#include "Common/PRCommonEnum.h"
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
	LocomotionState_Run				UMETA(DisplayName = "Run")
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
	/** 입력받은 인자가 LocomotionState와 같은지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance")
	bool IsEqualLocomotionState(EPRLocomotionState NewLocomotionState) const;
	
protected:
	/**
	 * 필수 속성을 설정하는 함수입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance")
	void SetupEssentialProperties();

	/** Locomotion의 상태를 최신화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance")
	void UpdateLocomotionState();

	/** Locomotion의 상태를 추적하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance")
	EPRTrackState TrackLocomotionState(EPRLocomotionState NewLocomotionState, bool& EnterExecuted, bool& ExitExecuted);

	/** Idle 상태를 최신화하는 함수입니다. */
	void UpdateIdleState();

	/** Run 상태를 최신화하는 함수입니다. */
	void UpdateRunState();

	/** Walk 상태를 최신화하는 함수입니다. */
	void UpdateWalkState();

	/** Run 상태일 때 최신화하는 함수입니다. */
	void UpdateOnRunEnter();

	/** Walk 상태일 때 최신화하는 함수입니다. */
	void UpdateOnWalkEnter();

	/** Locomotion의 시작 방향을 최신화하는 함수입니다. */
	void UpdateLocomotionStartDirection(float NewStartAngle);

	// void UpdateCharacterRotation();
	//
	// void UpdateRotationWhileMoving();

	/** Locomotion의 재생 속도를 최신화하는 함수입니다. */
	void UpdateLocomotionPlayRate();
	
protected:
	/** 이전 프레임과 현재 프레임 사이의 시간 간격입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance")
	float DeltaTime;
	
	/** Locomotion의 상태입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	EPRLocomotionState LocomotionState;

	/** 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	FVector Velocity;

	/** 가속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	FVector Acceleration;

	/** 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float Speed;

	/** 걷는 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float WalkSpeed;	

	/** 입력 벡터입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	FVector InputVector;

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

	/** 움직이는 방향입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	EPRDirection MovementDirection;

	/** 애니메이션의 재생 속도입니다. */	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float PlayRate;
	
private:
	/** 이 AnimInstance를 사용하는 PRBaseCharacter입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class APRBaseCharacter> PROwner;

	/** PROwner의 CharacterMovementComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCharacterMovementComponent> CharacterMovement;

	/** TrackLocomotionState 함수에서 Idle 상태를 추적하는 Flag 변수입니다. */
	bool bTrackIdleStateEnterExecuted;
	bool bTrackIdleStateExitExecuted;

	/** TrackLocomotionState 함수에서 Run 상태를 추적하는 Flag 변수입니다. */
	bool bTrackRunStateEnterExecuted;
	bool bTrackRunStateExitExecuted;

	/** TrackLocomotionState 함수에서 Walk 상태를 추적하는 Flag 변수입니다. */
	bool bTrackWalkStateEnterExecuted;
	bool bTrackWalkStateExitExecuted;

public:
	/** PROwner를 반환하는 함수입니다. */
	class APRBaseCharacter* GetPROwner() const;

	/** PROwner의 CharacterMovementComponent를 반환하는 함수입니다. */
	class UCharacterMovementComponent* GetCharacterMovement() const;
};
