// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Characters/PRBaseCharacter.h"
#include "PRPlayerCharacter.generated.h"

class UPRInputConfigDataAsset;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

struct FInputActionValue;

/**
 * 플레이어 캐릭터 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRPlayerCharacter : public APRBaseCharacter
{
	GENERATED_BODY()

public:
	APRPlayerCharacter();
	
public:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;

#pragma region Input
protected:
	/** 캐릭터의 이동 함수입니다. */
	void Move(const FInputActionValue& Value);

	/** 캐릭터의 시선을 나타내는 카메라를 조절하는 함수입니다. */
	void Look(const FInputActionValue& Value);

	/** 다음 업데이트에서 캐릭터를 점프시키는 함수입니다. */
	virtual void Jump() override;
	
	/** 다음 업데이트 시 캐릭터의 점프를 중지하는 함수입니다. */
	virtual void StopJumping() override;
	
	/** 상호작용을 실행하는 함수입니다. */
	void Interaction(const FInputActionValue& Value);

	/** 캐릭터가 전력질주하는 함수입니다. */
	virtual void Sprint() override;
	
protected:
	/** 캐릭터가 사용하는 EnhancedInputComponent입니다. */
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UEnhancedInputComponent> EnhancedInputComponent;
	
	/** 캐릭터가 사용하는 MappingContext입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	/** 캐릭터가 사용하는 InputAction입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UPRInputConfigDataAsset> InputActions;

	// FEnhancedInputActionValueBinding를 포인터 변수로 가지고 있어야함
	// 지역변수로 사용할 수 있지만 그러면 맨 처음 호출할 때는 작동하지 않음
	/**
	 * 캐릭터의 이동 InputAction의 입력 값을 참조한 구조체입니다.
	 * UPROPERTY()를 사용할 수 없는 구조체입니다.
	 */
private:
	struct FEnhancedInputActionValueBinding* MoveActionBinding;
#pragma endregion

#pragma region DamageSystem
public:
	/** 캐릭터가 사망했을 때 호출하는 함수입니다. */
	virtual void Death() override;
#pragma endregion 

#pragma region Camera
protected:
	/**
	 * 마우스를 사용할 때 사용하는 카메라 좌우 회전 함수입니다.
	 *
	 * @param Value 마우스를 사용한 좌우 회전값입니다.
	 */
	void Turn(const FInputActionValue& Value);
	
	/**
	 * 마우스를 사용할 때 사용하는 카메라 상하 회전 함수입니다.
	 *
	 * @param Value 마우스를 사용한 상하 회전값입니다.
	 */
	void LookUp(const FInputActionValue& Value);
	
	/**
	 * 게임패드를 사용할 때 사용하는 카메라 좌우 회전 함수입니다.
	 *
	 * @param Value 게임패드를 사용한 좌우 회전값입니다.
	 */
	void TurnRate(const FInputActionValue& Value);
	
	/**
	 * 게임패드를 사용할 때 사용하는 카메라 상하 회전 함수입니다.
	 *
	 * @param Value 게임패드를 사용한 상하 회전값입니다.
	 */
	void LookUpRate(const FInputActionValue& Value);
	
protected:
	/** 캐릭터 뒤에 카메라를 배치하는 SpringArm입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> SpringArm;

	/** 캐릭터 뒤에서 따라오는 카메라입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> FollowCamera;

	/** 카메라가 캐릭터 뒤에서 떨어져서 따라오는 거리입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseSpringArmLength;

	/** 카메라의 좌우 기본 회전 값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseTurnRate;

	/** 카메라의 상하 기본 회전 값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookUpRate;

public:
	/** SpringArm을 반환하는 함수입니다. */
	class USpringArmComponent* GetSpringArm() const { return SpringArm; }

	/** FollowCamera를 반환하는 함수입니다. */
	class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
#pragma endregion

#pragma region MovementInput
public:
	/** 전방 이동 입력값을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	float GetMoveForward() const;

	/** 우측 이동 입력값을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	float GetMoveRight() const;

	/** 입력 방향으로 캐릭터를 회전시키는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	void RotationInputDirection(bool bIsReverse = false);
	
protected:
	// /**
	//  * MoveForward 키를 입력할 경우 호출하는 함수입니다.
	//  * 
	//  * @param Value MoveForward의 입력값입니다.
	//  */
	// void MoveForward(float Value);
	//
	// /**
	//  * MoveRight 키를 입력할 경우 호출하는 함수입니다.
	//  * 
	//  * @param Value MoveRight의 입력값입니다.
	//  */
	// void MoveRight(float Value);
	//
	// /**
	//  * 방향에 따라 AddMovementInput을 호출하여 이동하는 함수입니다.
	//  * 
	//  * @param Direction 이동하는 방향입니다.
	//  */
	// virtual void AddPlayerMovementInput(EPRDirection Direction);

	/**
	 * 방향에 따라 AddMovementInput을 호출하여 이동하는 함수입니다.
	 * 
	 * @param MovementVector 이동 벡터입니다.
	 */
	virtual void AddPlayerMovementInput(FVector2D MovementVector);

	/** 컨트롤러의 전방 벡터를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	FVector GetControlForwardVector() const;

	/** 컨트롤러의 우측 벡터를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	FVector GetControlRightVector() const;
	
	/**
	 * 컨트롤러의 전방 벡터와 우측 벡터를 가져오는 함수입니다.
	 * 
	 * @param ForwardVector 컨트롤러의 전방 벡터입니다.
	 * @param RightVector 컨트롤러의 우측 벡터입니다.
	 */ 
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	void GetControlForwardVectorAndRightVector(UPARAM(ref) FVector& ForwardVector, UPARAM(ref) FVector& RightVector) const;

	/**
	 * 게임패드의 아날로그 스틱의 대각선 입력을 보정한 값을 가져오는 함수입니다.
	 * 
	 * @param ForwardAxis MoveForward 값입니다.
	 * @param RightAxis MoveRight 값입니다.
	 * @param FixForwardAxis 대각선 입력을 보정한 MoveForward 값입니다.
	 * @param FixRightAxis 대각선 입력을 보정한 MoveRight 값입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	void FixDiagonalGamepadValues(float ForwardAxis, float RightAxis, UPARAM(ref) float& FixForwardAxis, UPARAM(ref) float& FixRightAxis) const;
#pragma endregion

#pragma region DoubleJump
public:
	/** 더블 점프하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "DoubleJump")
	void DoubleJump();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoubleJump")
	TObjectPtr<UAnimMontage> DoubleJumpAnimMontage;
	
	/** 더블 점프를 할 수 있는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DoubleJump")
	bool bCanDoubleJump;
	
	/** 점프를 한 후 더블 점프를 할 수 있는 딜레이 시간입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoubleJump")
	float DoubleJumpDelay;

	/** 점프를 한 후 더블 점프를 할 수 있는 딜레이를 적용하는 TimerHandle입니다. */
	FTimerHandle DoubleJumpTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoubleJump")
	bool bFlag;
	
#pragma endregion 
};
