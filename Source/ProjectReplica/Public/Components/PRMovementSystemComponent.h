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
		, MinAnalogWalkSpeed(0.0f)
	{}

	FPRGaitSettings(float NewMovementSpeed, float NewMaxAcceleration, float NewBrakingFrictionFactor,
					float NewBrakingFriction, bool bNewUseSeparateBrakingFriction, float NewBrakingDeceleration,
					float NewMinAnalogWalkSpeed)
		: MovementSpeed(NewMovementSpeed)
		, MaxAcceleration(NewMaxAcceleration)
		, BrakingFrictionFactor(NewBrakingFrictionFactor)
		, BrakingFriction(NewBrakingFriction)
		, bUseSeparateBrakingFriction(bNewUseSeparateBrakingFriction)
		, BrakingDeceleration(NewBrakingDeceleration)
		, MinAnalogWalkSpeed(NewMinAnalogWalkSpeed)
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

	/** 최소 아날로그 걷기 속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GaitSettings")
	float MinAnalogWalkSpeed;	
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

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
#pragma region Gait
public:
	/** 걸음걸이를 최신화하는 함수입니다. */
	void UpdateGait();

	/** GaitSettings를 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementSystem")
	void InitializeGaitSettings();	

	/** 인자에 해당하는 걸음걸이의 설정 값을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementSystem")
	FPRGaitSettings GetGaitSettings(EPRGait NewGait) const;

	/** GaitSettings를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementSystem")
	TMap<EPRGait, FPRGaitSettings> GetAllGaitSettings() const;

	/** 인자에 해당하는 걸음걸이의 설정 값을 캐릭터에게 적용하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementSystem")
	bool ApplyGaitSettings(EPRGait ApplyGait);

	/** 인자에 해당하는 걸음걸이가 허용한 걸을걸잉와 같은지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementSystem")
	bool IsEqualAllowGait(EPRGait NewGait) const;

	/** 인자에 해당하는 걸음걸이가 현재 걸음걸이와 같은지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementSystem")
	bool IsEqualCurrentGait(EPRGait NewGait) const;

	/** 인자에 해당하는 걸음걸이가 마지막 걸음걸이와 같은지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementSystem")
	bool IsEqualLastGait(EPRGait NewGait) const;

	/**
	 * 입력받은 인자로 CurrentGait를 설정하는 함수입니다.
	 * 기존의 CurrentGait는 LastGait로 설정합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "MovementSystem")
	void SetCurrentGait(EPRGait NewGait);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	EPRGait TestGait;

	void SetTestGait(EPRGait NewGait) { TestGait = NewGait; }
	
protected:
	/** 허용한 걸을걸이입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementSystem")
	EPRGait AllowGait;	
	
	/** 현재 걸음걸이입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementSystem")
	EPRGait CurrentGait;

	/** 마지막 걸음걸이입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementSystem")
	EPRGait LastGait;

	/** 걸음걸이의 설정 값을 가진 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementSystem")
	TMap<EPRGait, FPRGaitSettings> GaitSettings;

	/** 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance")
	FVector Velocity;

	/** 지상에서의 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance")
	float GroundSpeed;	

	/** 가속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance")
	FVector Acceleration;

	/** 달릴 때의 최소 가속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance")
	float MinAccelerationToRunGait;

public:
	/** AllowGait를 반환하는 함수입니다. */
	EPRGait GetAllowGait() const;

	/** 입력받은 인자로 AllowGait를 설정하는 함수입니다. */
	void SetAllowGait(EPRGait NewGait);

	/** CurrentGait를 반환하는 함수입니다. */
	EPRGait GetCurrentGait() const;

	/** MinAccelerationToRunGait를 반환하는 함수입니다. */
	float GetMinAccelerationToRunGait() const;
#pragma endregion

#pragma region Aerial
public:
	/**
	 * 공중에 뜬(에어리얼) 상태를 활성화하는 함수입니다.
	 *
	 * @param bNewActivateAerial: ture일 경우 이동을 중지(속도를 0으로 설정, 가속이 있는 구성요소의 경우 가속을 0으로 설정)하고
	 *								캐릭터의 GravityScale을 0.0f으로 설정합니다.
	 *								false일 경우 캐럭터의 GravityScale을 DefaultGravityScale로 설정합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "MovementSystem|Aerial")
	void ActivateAerial(bool bNewActivateAerial);

private:
	/** 공중에 뜬(에어리얼) 상태인지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementSystem|Aerial", meta = (AllowPrivateAccess = "true"))
	bool bActivateAerial;
	
	/** 중력 크기의 기본값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementSystem|Aerial", meta = (AllowPrivateAccess = "true"))
	float DefaultGravityScale;
#pragma endregion 
};
