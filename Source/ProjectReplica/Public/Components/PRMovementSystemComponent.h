// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRMovementSystemComponent.generated.h"

class UPRBaseAnimInstance;

/** 걸음걸이를 나타내는 열거형입니다. */
UENUM(BlueprintType)
enum class EPRGait : uint8
{
	Gait_Idle			UMETA(DisplayName = "Idle"),
	Gait_Walk			UMETA(DisplayName = "Walk"),
	Gait_Run			UMETA(DisplayName = "Run"),
	Gait_Sprint			UMETA(DisplayName = "Sprint")
};

/** 걸음걸이의 설정 값을 나타내는 구조체입니다. */
USTRUCT(Atomic, BlueprintType)
struct FPRGaitSettings
{
	GENERATED_BODY()

public:
	FPRGaitSettings()
		: MovementSpeed(0.0f)
		, MaxAcceleration(0.0f)
		, BrakingFrictionFactor(0.0f)
		, BrakingFriction(0.0f)
		, bUseSeparateBrakingFriction(true)
		, BrakingDeceleration(0.0f)
	{}

	FPRGaitSettings(float NewMovementSpeed, float NewMaxAcceleration, float NewBrakingFrictionFactor,
					float NewBrakingFriction, bool bNewUseSeparateBrakingFriction, float NewBrakingDeceleration)
		: MovementSpeed(NewMovementSpeed)
		, MaxAcceleration(NewMaxAcceleration)
		, BrakingFrictionFactor(NewBrakingFrictionFactor)
		, BrakingFriction(NewBrakingFriction)
		, bUseSeparateBrakingFriction(bNewUseSeparateBrakingFriction)
		, BrakingDeceleration(NewBrakingDeceleration)
	{}

public:
	/** 움직임의 속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GaitSettings")
	float MovementSpeed;

	/** 최대 가속입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GaitSettings")
	float MaxAcceleration;

	/** 마찰 인수 제동입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GaitSettings")
	float BrakingFrictionFactor;

	/** 마찰 제동입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GaitSettings")
	float BrakingFriction;

	/** 별도 브레이킹 마찰 사용을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GaitSettings")
	bool bUseSeparateBrakingFriction;

	/** 감속 걷기 제동입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GaitSettings")
	float BrakingDeceleration;
};

/**
 * 캐릭터의 움직임에 관련된 정보를 관리하는 ActorComponent 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UPRMovementSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRMovementSystemComponent();

public:
	/**
	 * 걸음걸이를 최신화하는 함수입니다.
	 *
	 * @param DesiredGait 최신화할 걸음걸이입니다.
	 */
	void UpdateGait(EPRGait DesiredGait);

	/** PROwner의 PRBaseAnimInstance를 반환하는 함수입니다. */
	UPRBaseAnimInstance* GetPRBaseAnimInstance() const;

protected:
	/** 현재 걸음걸이입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem")
	EPRGait CurrentGait;

	/** 마지막 걸음걸이입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem")
	EPRGait LastGait;

	/** 걸음걸이의 설정값을 가진 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRMovementSystem")
	TMap<EPRGait, FPRGaitSettings> GaitSettings;
};
