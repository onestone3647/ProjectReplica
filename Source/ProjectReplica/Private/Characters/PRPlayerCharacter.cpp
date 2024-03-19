// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PRPlayerCharacter.h"
#include "Data/PRInputConfigDataAsset.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/PRMovementSystemComponent.h"

APRPlayerCharacter::APRPlayerCharacter()
{
	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = true;		// 캐릭터가 이동하는 방향으로 회전합니다.

	// Input
	DefaultMappingContext = nullptr;
	InputActions = nullptr;
	
	// Camera
	SpringArm = nullptr;
	FollowCamera = nullptr;
	BaseSpringArmLength = 400.0f;
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	
	// SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = BaseSpringArmLength;
	SpringArm->SocketOffset = FVector(0.0f, 0.0f, 200.0f);
	SpringArm->bUsePawnControlRotation = true;										// 컨트롤러를 기준으로 SpringArm을 회전합니다.
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 10.0f;
	SpringArm->CameraRotationLagSpeed = 20.0f;

	// FollowCamera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;									// 카메라가 SpringArm을 기준으로 회전하지 않습니다.
	FollowCamera->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
	
	// DoubleJump
	DoubleJumpAnimMontage = nullptr;
	bCanDoubleJump = true;
	DoubleJumpDelay = 0.2f;
}

void APRPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APRPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(DefaultMappingContext && InputActions)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		if(PlayerController)
		{
			UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
			if(SubSystem)
			{
				// MappingContext를 추가합니다.
				SubSystem->ClearAllMappings();
				SubSystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}

		EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
		if(EnhancedInputComponent)
		{
			// 이동
			EnhancedInputComponent->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &APRPlayerCharacter::Move);
			MoveActionBinding = &EnhancedInputComponent->BindActionValue(InputActions->InputMove);		// 이동 InputAction의 입력 값을 바인딩합니다.
			
			// 마우스 시선
			EnhancedInputComponent->BindAction(InputActions->InputTurn, ETriggerEvent::Triggered, this, &APRPlayerCharacter::Turn);
			EnhancedInputComponent->BindAction(InputActions->InputLookUp, ETriggerEvent::Triggered, this, &APRPlayerCharacter::LookUp);
			
			// 게임패드 시선
			EnhancedInputComponent->BindAction(InputActions->InputTurnRate, ETriggerEvent::Triggered, this, &APRPlayerCharacter::TurnRate);
			EnhancedInputComponent->BindAction(InputActions->InputLookUpRate, ETriggerEvent::Triggered, this, &APRPlayerCharacter::LookUpRate);

			// 점프
			EnhancedInputComponent->BindAction(InputActions->InputJump, ETriggerEvent::Started, this, &APRPlayerCharacter::Jump);
			EnhancedInputComponent->BindAction(InputActions->InputJump, ETriggerEvent::Completed, this, &APRPlayerCharacter::StopJumping);

			// 걷기
			EnhancedInputComponent->BindAction(InputActions->InputWalk, ETriggerEvent::Triggered, this, &APRBaseCharacter::ToggleWalk);
		
			// 공격
			EnhancedInputComponent->BindAction(InputActions->InputNormalAttack, ETriggerEvent::Triggered, this, &APRBaseCharacter::DoDamage);
		}
	}
}

void APRPlayerCharacter::Landed(const FHitResult& Hit)
{
	ACharacter::Landed(Hit);

	// 땅에 착지했을 때 더블 점프 관련 변수를 초기화합니다.
	bCanDoubleJump = true;
	GetWorldTimerManager().ClearTimer(DoubleJumpTimerHandle);
}

#pragma region Input
void APRPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if(Controller)
	{
		// // 어느쪽이 앞인지 확인합니다.
		// const FRotator Rotation = Controller->GetControlRotation();
		// const FRotator YawRotation(0, Rotation.Yaw, 0);
		//
		// // 전방 벡터를 구합니다.
		// const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//
		// // 우측 벡터를 구합니다.
		// const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		//
		// AddMovementInput(ForwardDirection, MovementVector.Y);
		// AddMovementInput(RightDirection, MovementVector.X);

		AddPlayerMovementInput(MovementVector);
	}
}

void APRPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if(Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APRPlayerCharacter::Jump()
{
	if(GetCharacterMovement()->IsFalling() == false)
	{
		Super::Jump();

		bCanDoubleJump = false;		// 단일 점프 후 더블 점프 비활성화

		// 딜레이 이후 더블 점프를 하기 위해 딜레이 타이머 실행
		GetWorldTimerManager().SetTimer(DoubleJumpTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			bCanDoubleJump = true;
		}), DoubleJumpDelay, false);
	}
	else
	{
		if(bCanDoubleJump)
		{
			DoubleJump();
		}
	}
}

void APRPlayerCharacter::StopJumping()
{
	Super::StopJumping();
}

void APRPlayerCharacter::Interaction(const FInputActionValue& Value)
{
}

void APRPlayerCharacter::Sprint()
{
	Super::Sprint();
}
#pragma endregion

#pragma region DamageSystem
void APRPlayerCharacter::Death()
{
	Super::Death();
}
#pragma endregion

#pragma region Camera
void APRPlayerCharacter::Turn(const FInputActionValue& Value)
{
	float TurnValue = Value.Get<float>();
	if(!FMath::IsNearlyZero(TurnValue))
	{
		AddControllerYawInput(TurnValue);
	}
}

void APRPlayerCharacter::LookUp(const FInputActionValue& Value)
{
	float LookValue = Value.Get<float>();
	if(!FMath::IsNearlyZero(LookValue))
	{
		AddControllerPitchInput(LookValue);
	}
}

void APRPlayerCharacter::TurnRate(const FInputActionValue& Value)
{
	float TurnValue = Value.Get<float>();
	if(!FMath::IsNearlyZero(TurnValue))
	{
		AddControllerYawInput(TurnValue * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void APRPlayerCharacter::LookUpRate(const FInputActionValue& Value)
{
	float LookValue = Value.Get<float>();
	if(!FMath::IsNearlyZero(LookValue))
	{
		AddControllerPitchInput(LookValue * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}
#pragma endregion 

#pragma region MovementInput
float APRPlayerCharacter::GetMoveForward() const
{
	if(EnhancedInputComponent && InputActions && InputActions->InputMove)
	{
		// FEnhancedInputActionValueBinding* MoveActionBindingNotBind = &EnhancedInputComponent->BindActionValue(InputActions->InputMove);
		// return MoveActionBindingNotBind->GetValue().Get<FVector2D>().Y;
		return MoveActionBinding->GetValue().Get<FVector2D>().Y;
	}
	
	return 0.0f;
}

float APRPlayerCharacter::GetMoveRight() const
{
	if(EnhancedInputComponent && InputActions && InputActions->InputMove)
	{
		// FEnhancedInputActionValueBinding* MoveActionBindingNotBind = &EnhancedInputComponent->BindActionValue(InputActions->InputMove);
		// return MoveActionBindingNotBind->GetValue().Get<FVector2D>().X;
		return MoveActionBinding->GetValue().Get<FVector2D>().X;
	}

	return 0.0f;
}

void APRPlayerCharacter::RotationInputDirection(bool bIsReverse)
{
	// 이동 입력 값을 얻습니다.
	const float MoveForward = GetMoveForward();
	const float MoveRight = GetMoveRight();

	// 입력 벡터를 생성합니다.
	FVector InputVector = FVector(MoveForward, MoveRight, 0.0f);

	// 입력 방향의 반대 방향으로 회전할 경우 입력 벡터를 반전합니다.
	if(bIsReverse)
	{
		InputVector *= -1.0f;
	}

	// 입력 벡터를 기반으로 회전 각도를 계산합니다.
	const FRotator InputRotator = UKismetMathLibrary::MakeRotFromX(InputVector);
	float InputDirectionYaw = InputRotator.Yaw;
	
	// 입력이 있으면 컨트롤러의 회전을 기준으로, 없으면 액터의 회전을 기준으로 회전합니다.
	InputDirectionYaw += FMath::IsNearlyZero(MoveForward) && FMath::IsNearlyZero(MoveRight) ? GetActorRotation().Yaw : GetControlRotation().Yaw;

	// 회전 각도를 적용하여 액터를 회전합니다.
	const FRotator InputDirectionRotator = FRotator(0.0f, InputDirectionYaw, 0.0f);
	SetActorRotation(InputDirectionRotator);
}

// void APRPlayerCharacter::MoveForward(float Value)
// {
// 	if(Controller && Value != 0.0f)
// 	{
// 		AddPlayerMovementInput(EPRDirection::Direction_Forward);
// 	}
// }
//
// void APRPlayerCharacter::MoveRight(float Value)
// {
// 	if(Controller && Value != 0.0f)
// 	{
// 		AddPlayerMovementInput(EPRDirection::Direction_Right);
// 	}
// }
//
// void APRPlayerCharacter::AddPlayerMovementInput(EPRDirection Direction)
// {
// 	// 컨트롤러의 전방 벡터와 오른쪽 벡터를 구합니다.
// 	FVector ForwardVector = FVector::ZeroVector;
// 	FVector RightVector = FVector::ZeroVector;
// 	GetControlForwardVectorAndRightVector(ForwardVector, RightVector);
//
// 	// 게임패드의 아날로그 스틱의 대각선 입력을 보정한 값을 구합니다.
// 	float MoveForward = 0.0f;
// 	float MoveRight = 0.0f;
// 	FixDiagonalGamepadValues(GetMoveForward(), GetMoveRight(), MoveForward, MoveRight);
// 	
// 	switch(Direction)
// 	{
// 	case EPRDirection::Direction_Forward:
// 		AddMovementInput(ForwardVector, MoveForward);
// 		break;
// 	case EPRDirection::Direction_Right:
// 		AddMovementInput(RightVector, MoveRight);
// 		break;
// 	default:
// 		return;
// 	}
// }

void APRPlayerCharacter::AddPlayerMovementInput(FVector2D MovementVector)
{
	// 컨트롤러의 전방 벡터와 오른쪽 벡터를 구합니다.
	FVector ForwardVector = FVector::ZeroVector;
	FVector RightVector = FVector::ZeroVector;
	GetControlForwardVectorAndRightVector(ForwardVector, RightVector);

	// 게임패드의 아날로그 스틱의 대각선 입력을 보정한 값을 구합니다.
	float MoveForward = 0.0f;
	float MoveRight = 0.0f;
	FixDiagonalGamepadValues(MovementVector.Y, MovementVector.X, MoveForward, MoveRight);
	
	AddMovementInput(ForwardVector, MoveForward);
	AddMovementInput(RightVector, MoveRight);
}

FVector APRPlayerCharacter::GetControlForwardVector() const
{
	const FRotator NewControlRotation = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);
	
	return UKismetMathLibrary::GetForwardVector(NewControlRotation);
}

FVector APRPlayerCharacter::GetControlRightVector() const
{
	const FRotator NewControlRotation = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);
	
	return UKismetMathLibrary::GetRightVector(NewControlRotation);
}

void APRPlayerCharacter::GetControlForwardVectorAndRightVector(FVector& ForwardVector, FVector& RightVector) const
{
	// const FRotator NewControlRotation = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);
	//
	// ForwardVector = UKismetMathLibrary::GetForwardVector(NewControlRotation);
	// RightVector = UKismetMathLibrary::GetRightVector(NewControlRotation);

	ForwardVector = GetControlForwardVector();
	RightVector = GetControlRightVector();
}

void APRPlayerCharacter::FixDiagonalGamepadValues(float ForwardAxis, float RightAxis, float& FixForwardAxis, float& FixRightAxis) const
{
	const float NewForwardAxis = ForwardAxis * FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 0.6f),
																					FVector2D(1.0f, 1.2f),
																					FMath::Abs(RightAxis));
	const float NewRightAxis = RightAxis * FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 0.6f),
																				FVector2D(1.0f, 1.2f),
																				FMath::Abs(ForwardAxis));

	FixForwardAxis = FMath::Clamp(NewForwardAxis, -1.0f, 1.0f);
	FixRightAxis = FMath::Clamp(NewRightAxis, -1.0f, 1.0f);
}
#pragma endregion 

#pragma region DoubleJump
void APRPlayerCharacter::DoubleJump()
{
	bCanDoubleJump = false;
	GetWorldTimerManager().ClearTimer(DoubleJumpTimerHandle);

	const float CurrentSpeed = GetVelocity().Size2D() * 0.8f;		// 떨어지는 속도는 제외합니다.
	const float MoveForward = GetMoveForward(); 
	const float MoveRight = GetMoveRight();

	ActivateAerial(true);
	RotationInputDirection();
	PlayAnimMontage(DoubleJumpAnimMontage);

	// 더블점프 이펙트 생성

	// 처음 더블점프를 하게 되면 전방속도를 제대로 받지 못함
	// 두번째부터는 정상적으로 속도를 받음
	const FVector ForwardVector = (GetControlForwardVector() * MoveForward).GetSafeNormal() * CurrentSpeed;
	const FVector RightVector = (GetControlRightVector() * MoveRight).GetSafeNormal() * CurrentSpeed;
	const FVector Velocity = ForwardVector + RightVector + FVector(0.0f, 0.0f, GetCharacterMovement()->JumpZVelocity);
	LaunchCharacter(Velocity, true, true);
	ActivateAerial(false);
}
#pragma endregion 
