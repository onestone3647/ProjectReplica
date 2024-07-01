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
#include "Components/CapsuleComponent.h"
#include "Components/PRMovementSystemComponent.h"
#include "MotionWarpingComponent.h"
#include "NiagaraComponent.h"
#include "Components/PREffectSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Controllers/PRPlayerController.h"
#include "Effects/PRNiagaraEffect.h"

APRPlayerCharacter::APRPlayerCharacter()
{
	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = true;		// 캐릭터가 이동하는 방향으로 회전합니다.
	// GetCharacterMovement()->bUseControllerDesiredRotation = false;

	// LockOn일 때
	// GetCharacterMovement()->bOrientRotationToMovement = false;
	// GetCharacterMovement()->bUseControllerDesiredRotation = true;

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
	DoubleJumpNiagaraEffect = nullptr;
	RootSocketName = FName("root");
	LeftFootSocketName = FName("foot_l");
	RightFootSocketName = FName("foot_r");
	DoubleJumpAnimMontage = nullptr;
	bCanDoubleJump = true;
	DoubleJumpDelay = 0.2f;

	// VaultCollision
	VaultCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("VaultCollision"));
	VaultCollision->InitCapsuleSize(10.0f, 38.0f);
	VaultCollision->SetRelativeLocation(FVector(30.0f, 0.0f, -30.0f));
	VaultCollision->SetCollisionProfileName(TEXT("VaultCollision"));
	VaultCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VaultCollision->SetupAttachment(RootComponent);
	
	// Vault
	bVaultDebug = false;
	bCanVaultWarp = false;
	VaultStartName = FName("VaultStart");
	VaultingName = FName("Vaulting");
	VaultLandName = FName("VaultLand");
	VaultStartLocation = FVector::ZeroVector;
	VaultingLocation = FVector::ZeroVector;
	VaultLandLocation = FVector::ZeroVector;
	VaultableObjectTraceCount = 3;
	VaultableObjectTraceInterval = -30.0f;
	VaultableObjectDistance = 100.0f;
	VaultableObjectTraceRadius = 10.0f;
	// DepthTrace
	DepthTraceCount = 5;
	DepthTraceInterval = 50.0f;
	DepthTraceUpOffset = 80.0f;
	DepthTraceDownOffset = 100.f;
	// Landing
	VaultLandDistance = 40.0f;
	VaultLandDownOffset = 1000.0f;
	VaultLandLocationZOffset = 50.0f;
}

void APRPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APRPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// VaultCollision
	VaultCollision->OnComponentBeginOverlap.AddDynamic(this, &APRPlayerCharacter::OnVaultCollisionBeginOverlap);
	if(bVaultDebug)
	{
		VaultCollision->SetHiddenInGame(false);
	}
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

			// 전력 질주
			EnhancedInputComponent->BindAction(InputActions->InputSprint, ETriggerEvent::Triggered, this, &APRBaseCharacter::ToggleSprint);
		
			// 공격
			EnhancedInputComponent->BindAction(InputActions->InputNormalAttack, ETriggerEvent::Triggered, this, &APRPlayerCharacter::Attack);
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
bool APRPlayerCharacter::IsUsingGamepad() const
{
	APRPlayerController* PRPlayerController = Cast<APRPlayerController>(GetController());
	if(IsValid(PRPlayerController))
	{
		return PRPlayerController->IsUsingGamepad();
	}

	return false;
}

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

		// 캐릭터가 이동할 때 무기를 발도 상태일 경우 납도합니다.
		if(!MovementVector.IsZero() && GetWeaponSystem()->IsDrawWeapon())
		{
			GetWeaponSystem()->SheatheWeapon(true, false);
		}

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
	// 무기를 발도하고 있을 경우 납도합니다.
	if(GetWeaponSystem()->IsDrawWeapon())
	{
		GetWeaponSystem()->SheatheWeapon(true, false);
	}
	
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

void APRPlayerCharacter::ToggleSprint()
{
	Super::ToggleSprint();
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

	// 게임패드를 사용하고 전력질주 상태가 아닐 떄
	// 아날로그 스틱의 1/3 기울기보다 작을 경우 걷기 상태, 클 경우 달리기 상태로 설정합니다.
	if(IsUsingGamepad() && !GetMovementSystem()->IsEqualAllowGait(EPRGait::Gait_Sprint))
	{
		if(FMath::Abs(MovementVector.X) >= 1.0f / 3.0f || FMath::Abs(MovementVector.Y) >= 1.0f/ 3.0f)
		{
			// 달리기 상태
			GetMovementSystem()->SetAllowGait(EPRGait::Gait_Run);
		}
		else
		{
			// 걷기 상태
			GetMovementSystem()->SetAllowGait(EPRGait::Gait_Walk);
		}
	}
	
	// 전력 질주 상태일 때 Vault를 실행합니다.
	if(GetMovementSystem()->IsEqualAllowGait(EPRGait::Gait_Sprint))
	{
		// 전력 질주를 허용한 상태일 때 입력이 있을 경우 VaultCollision을 활성화합니다.
		if(MoveForward != 0.0f || MoveRight != 0.0f)
		{
			VaultCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
	else
	{
		// 전력 질주를 허용한 상태가 아닐 때 VaultCollision을 비활성화합니다.
		VaultCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
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
	if(DoubleJumpNiagaraEffect)
	{
		const FVector CenterLocation = GetMesh()->GetSocketLocation(FName("root"));
		const FVector LeftFootLocation = GetMesh()->GetSocketLocation(FName("foot_l"));
		const FVector RightFootLocation = GetMesh()->GetSocketLocation(FName("foot_r"));
		const FVector NewSpawnEffectLocation = FVector(CenterLocation.X, CenterLocation.Y, UKismetMathLibrary::Min(LeftFootLocation.Z, RightFootLocation.Z));

		APRNiagaraEffect* DoubleJumpEffect = GetEffectSystem()->SpawnNiagaraEffectAtLocation(DoubleJumpNiagaraEffect, NewSpawnEffectLocation);
		// if(DoubleJumpEffect)
		// {
		// 	DoubleJumpEffect->GetNiagaraEffect()->SetVariableLinearColor(EffectColor, Signaturecolor)
		// }
	}

	// 처음 더블점프를 하게 되면 전방속도를 제대로 받지 못함
	// 두번째부터는 정상적으로 속도를 받음
	const FVector ForwardVector = (GetControlForwardVector() * MoveForward).GetSafeNormal() * CurrentSpeed;
	const FVector RightVector = (GetControlRightVector() * MoveRight).GetSafeNormal() * CurrentSpeed;
	const FVector Velocity = ForwardVector + RightVector + FVector(0.0f, 0.0f, GetCharacterMovement()->JumpZVelocity);
	LaunchCharacter(Velocity, true, true);
	ActivateAerial(false);
}
#pragma endregion 

#pragma region Vaulting
void APRPlayerCharacter::ExecuteVault()
{
	// 뛰어넘을 오브젝트를 탐색합니다.
	for(int Index = 0; Index < VaultableObjectTraceCount; Index++)
	{
		FHitResult HitResult;
		const FVector TraceStart = GetActorLocation() + FVector(0.0f, 0.0f, Index * VaultableObjectTraceInterval);
		const FVector TraceEnd = TraceStart + (GetActorForwardVector() * VaultableObjectDistance);
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		// 디버그 옵션을 설정합니다.
		EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
		if(bVaultDebug)
		{
			DebugType = EDrawDebugTrace::ForDuration;
		}

		// 캐릭터가 뛰어넘을 수 있는 거리 안에 오브젝트가 존재하는 Trace를 실행합니다.
		bool bIsHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceStart, TraceEnd, VaultableObjectTraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility),
																false, ActorsToIgnore, DebugType, HitResult, true);
		if(bIsHit)
		{
			if(bVaultDebug)
			{
				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Blue, false, 5.0f);
			}
			
			CalculateVaultableObjectDepth(HitResult.ImpactPoint);
			
			// 뛰어넘을 오브젝트를 탐색했을 경우 탐색을 마칩니다.
			break;
		}
	}
}

void APRPlayerCharacter::DisableVaultWarp()
{
	bCanVaultWarp = false;
	
	// ExecuteVaultMotionWarp 함수의 bInZOffset 변수에 영향을 줘서
	// 장애물을 뛰어넘을 수 없을 경우에 생기는 버그를 방지하기 위해서 초기화합니다.
	VaultLandLocation = FVector(0.0f, 0.0f, 20000.0f);
}

void APRPlayerCharacter::SetVaultState()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APRPlayerCharacter::ResetVaultState()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DisableVaultWarp();
}

void APRPlayerCharacter::CalculateVaultableObjectDepth(FVector TraceImpactPoint)
{
	// 뛰어넘을 오브젝트의 치수를 계산합니다.
	for(int Index = 0; Index < DepthTraceCount; Index++)
	{
		FHitResult HitResult;
		const FVector TraceStart = TraceImpactPoint
									+ FVector(0.0f, 0.0f, DepthTraceUpOffset)
									+ (GetActorForwardVector() * Index * DepthTraceInterval);
		const FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, DepthTraceDownOffset);
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		// 디버그 옵션을 설정합니다.
		EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
		if(bVaultDebug)
		{
			DebugType = EDrawDebugTrace::ForDuration;
		}

		// 뛰어넘을 장애물과 캐릭터 사이의 거리를 측정합니다.
		bool bIsHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceStart, TraceEnd, VaultableObjectTraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility),
																false, ActorsToIgnore, DebugType, HitResult, true);
		if(bIsHit)
		{
			// HitResult.bStartPenetrating: Break Hit Result 블루프린트 노드에서 Initial Overlap 변수로 나타냅니다.
			// Trace 시작 시 충돌이 발생했는지 여부를 나타내는 변수입니다.
			if(HitResult.bStartPenetrating)		
			{
				// Trace를 시작할 때 Trace 원점에서 충돌이 발생했을(Trace가 충돌로 시작됐을) 경우 Vault를 비활성화합니다.
				DisableVaultWarp();
				break;
			}
			else
			{
				// 점차 거리를 늘려가면서 탐색합니다.
				// 첫 번째 탐색일 경우
				if(Index == 0)
				{
					VaultStartLocation = HitResult.ImpactPoint;
					if(bVaultDebug)
					{
						DrawDebugSphere(GetWorld(), VaultStartLocation, 15.0f, 12, FColor::White, false, 5.0f);
					}
				}

				VaultingLocation = HitResult.ImpactPoint;
				if(bVaultDebug)
				{
					DrawDebugSphere(GetWorld(), VaultingLocation, 10.0f, 12, FColor::Yellow, false, 5.0f);
				}
			
				bCanVaultWarp = true;
			}
		}
		else
		{
			// 탐색을 마칠 경우 장애물을 넘어서 탐색한 것이므로 착지할 위치를 계산합니다.
			CalculateVaultLandLocation(HitResult.TraceStart);
			break;
		}
	}

	if(bVaultDebug && VaultingLocation != FVector::ZeroVector)
	{
		DrawDebugSphere(GetWorld(), VaultingLocation, 15.0f, 12, FColor::Purple, false, 5.0f);
	}

	ExecuteVaultMotionWarp();
}

void APRPlayerCharacter::CalculateVaultLandLocation(FVector TraceEndLocation)
{
	FHitResult HitResult;
	const FVector TraceStart = TraceEndLocation + (GetActorForwardVector() * VaultLandDistance);
	const FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, VaultLandDownOffset);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	// 디버그 옵션을 설정합니다.
	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
	if(bVaultDebug)
	{
		DebugType = EDrawDebugTrace::ForDuration;
	}
	
	bool bIsLandHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, UEngineTypes::ConvertToTraceType(ECC_Visibility),
																	true, ActorsToIgnore, DebugType, HitResult, true);
	if(bIsLandHit)
	{
		VaultLandLocation = HitResult.ImpactPoint;
		if(bVaultDebug)
		{
			DrawDebugSphere(GetWorld(), VaultLandLocation, 10.0f, 12, FColor::Cyan, false, 5.0f);
		}
	}
}

void APRPlayerCharacter::ExecuteVaultMotionWarp()
{
	bool bInZOffset = UKismetMathLibrary::InRange_FloatFloat(VaultLandLocation.Z,
																GetMesh()->GetComponentLocation().Z - VaultLandLocationZOffset,
																GetMesh()->GetComponentLocation().Z + VaultLandLocationZOffset);
	if(bCanVaultWarp && bInZOffset)
	{
		if(GetMotionWarping())
		{
			// VaultStart
			FMotionWarpingTarget VaultStartMotionWarpingTarget = FMotionWarpingTarget();
			VaultStartMotionWarpingTarget.Name = VaultStartName;
			VaultStartMotionWarpingTarget.Location = VaultStartLocation;
			VaultStartMotionWarpingTarget.Rotation = GetActorRotation();
			GetMotionWarping()->AddOrUpdateWarpTarget(VaultStartMotionWarpingTarget);

			// Vaulting
			FMotionWarpingTarget VaultingMotionWarpingTarget = FMotionWarpingTarget();
			VaultingMotionWarpingTarget.Name = VaultingName;
			VaultingMotionWarpingTarget.Location = VaultingLocation;
			VaultingMotionWarpingTarget.Rotation = GetActorRotation();
			GetMotionWarping()->AddOrUpdateWarpTarget(VaultingMotionWarpingTarget);
			
			// VaultLand
			FMotionWarpingTarget VaultLandMotionWarpingTarget = FMotionWarpingTarget();
			VaultLandMotionWarpingTarget.Name = VaultLandName;
			VaultLandMotionWarpingTarget.Location = VaultLandLocation;
			VaultLandMotionWarpingTarget.Rotation = GetActorRotation();
			GetMotionWarping()->AddOrUpdateWarpTarget(VaultLandMotionWarpingTarget);

			PlayAnimMontage(VaultAnimMontage, 1.2f);
			GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &APRPlayerCharacter::OnVaultAnimMontageEnded);
		}
	}
}

void APRPlayerCharacter::OnVaultAnimMontageEnded(UAnimMontage* NewVaultAnimMontage, bool bInterrupted)
{
	if(VaultAnimMontage == NewVaultAnimMontage)
	{
		DisableVaultWarp();
	}

	GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveDynamic(this, &APRPlayerCharacter::OnVaultAnimMontageEnded);
}

void APRPlayerCharacter::OnVaultCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ExecuteVault();
}

void APRPlayerCharacter::Attack_Implementation()
{
	Super::Attack_Implementation();

	DoDamage();
}
#pragma endregion 
