// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PRBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/PRDamageSystemComponent.h"
#include "Components/PRStatSystemComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRObjectPoolSystemComponent.h"
#include "Components/PRMovementSystemComponent.h"

// 임시
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

APRBaseCharacter::APRBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// CapsuleComponent
	GetCapsuleComponent()->InitCapsuleSize(30.0f, 94.0f);

	// CharacterMovement
	// GetCharacterMovement()->MaxAcceleration = 1000.0f;
	// GetCharacterMovement()->BrakingFrictionFactor = 0.5f;
	// GetCharacterMovement()->GroundFriction = 5.0f;
	// GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	// GetCharacterMovement()->BrakingDecelerationWalking = 1000.0f;
	
	// Advanced Movement System 용
	// GetCharacterMovement()->RotationRate = FRotator::ZeroRotator;

	// LockOn일 때
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// LockOn이 아닐 때
	// GetCharacterMovement()->bUseControllerDesiredRotation = false;
	// GetCharacterMovement()->bOrientRotationToMovement = true;

	// Pawn
	// 컨트롤러가 회전할 때 캐릭터가 같이 회전하지 않도록 설정합니다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// SkeletalMesh
	// 기본 스켈레탈 메시를 설정합니다.
	// GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -94.0f), FRotator(0.0f, 270.0f, 0.0f));
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -96.5f), FRotator(0.0f, 270.0f, 0.0f));
	// static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Mannequin(TEXT("/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin"));
	// if(SK_Mannequin.Succeeded() == true)
	// {
	// 	GetMesh()->SetSkeletalMesh(SK_Mannequin.Object);
	// }

	// DamageSystem
	DamageSystem = CreateDefaultSubobject<UPRDamageSystemComponent>(TEXT("DamageSystem"));
	
	// StatSystem
	StatSystem = CreateDefaultSubobject<UPRStatSystemComponent>(TEXT("StatSystem"));

	// StateSystem
	StateSystem = CreateDefaultSubobject<UPRStateSystemComponent>(TEXT("StateSystem"));

	// ObjectPoolSystem
	ObjectPoolSystem = CreateDefaultSubobject<UPRObjectPoolSystemComponent>(TEXT("ObjectPoolSystem"));

	// MovementSystem
	MovementSystem = CreateDefaultSubobject<UPRMovementSystemComponent>(TEXT("MovementSystem"));

	// Locomotion
	WalkSpeed = 240.0f;
	RunSpeed = 550.0f;
	SprintSpeed = 700.0f;
}

void APRBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// CharacterMovement
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

	// DamageSystem
	GetDamageSystem()->OnDeathDelegate.AddDynamic(this, &APRBaseCharacter::Death);
	GetDamageSystem()->OnBlockedDelegate.AddDynamic(this, &APRBaseCharacter::Blocked);
	GetDamageSystem()->OnDamageResponseDelegate.AddDynamic(this, &APRBaseCharacter::DamageResponse);

	// ObjectPoolSystem
	GetObjectPoolSystem()->InitializeObjectPool();

	// MovementSystem
	GetMovementSystem()->UpdateGait(EPRGait::Gait_Run);
}

void APRBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void APRBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#pragma region Interface_Damageable
float APRBaseCharacter::GetCurrentHealth_Implementation()
{
	if(GetStatSystem())
	{
		return GetStatSystem()->GetCharacterStat().Health;
	}
	
	return 0.0f;
}

float APRBaseCharacter::GetMaxHealth_Implementation()
{
	if(GetStatSystem())
	{
		return GetStatSystem()->GetCharacterStat().MaxHealth;
	}

	return 0.0f;
}

float APRBaseCharacter::Heal_Implementation(float Amount)
{
	if(GetDamageSystem())
	{
		return GetDamageSystem()->Heal(Amount);
	}

	return 0.0f;
}

bool APRBaseCharacter::TakeDamage_Implementation(FPRDamageInfo DamageInfo)
{
	if(GetDamageSystem())
	{
		return GetDamageSystem()->TakeDamage(DamageInfo);
	}

	return false;
}
#pragma endregion 

#pragma region DamageSystem
void APRBaseCharacter::Death()
{
	GetStateSystem()->SetIsDead(true);

	// Collision을 비활성화합니다.
	SetActorEnableCollision(false);

	// 사망 애니메이션 실행
}

void APRBaseCharacter::Blocked(bool bCanBeParried)
{
	PR_LOG_SCREEN("Blocked");
}

void APRBaseCharacter::DamageResponse(EPRDamageResponse DamageResponse)
{
	PR_LOG_SCREEN("DamageResponse: %s", *UEnum::GetValueAsString(DamageResponse));
}

void APRBaseCharacter::DoDamage()
{
	if(IsDead() && !GetStatSystem())
	{
		return;
	}
	
	TArray<FHitResult> HitResults;
	bool bIsHit = false;
	const FVector TraceStart = GetActorLocation();
	const FVector TraceEnd = TraceStart + GetActorForwardVector() * 100.0f;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	
	// ObjectTypeQuery3 = ObjectType Pawn
	ObjectTypes.Emplace(EObjectTypeQuery::ObjectTypeQuery3);
			
	// 자신을 Trace 대상에서 제외합니다.
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Emplace(this);

	// Debug 실행을 설정합니다.
	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
	if(bDamageSystemDebug)
	{
		DebugType = EDrawDebugTrace::ForDuration;
	}

	TSet<AActor*> UniqueActors;

	bIsHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), TraceStart, TraceEnd, 20.0f, ObjectTypes, false, ActorsToIgnore, DebugType, HitResults, true);
	if(bIsHit)
	{
		for(FHitResult HitResult : HitResults)
		{
			if(IsValid(HitResult.GetActor())
				&& HitResult.GetActor()->GetClass()->ImplementsInterface(UInterface_PRDamageable::StaticClass())
				&& UniqueActors.Find(HitResult.GetActor()) == nullptr)
			{
				PR_LOG_SCREEN("Hit Actor Name: %s", *HitResult.GetActor()->GetName());
				
				UniqueActors.Emplace(HitResult.GetActor());
				FPRDamageInfo DamageInfo;
				DamageInfo.DamageType = EPRDamageType::DamageType_Melee;
				DamageInfo.DamageElement = DamageElement;
				DamageInfo.DamageResponse = EPRDamageResponse::DamageResponse_HitReaction;
				DamageInfo.ImpactLocation = HitResult.ImpactPoint;

				// 치명타 확률(%)
				const float CriticalRate = GetStatSystem()->GetCharacterStat().CriticalRate;

				// 확률을 나타내는 Rate 변수와 비교하여 치명타 또는 일반 공격을 판단합니다.
				float Rate = FMath::FRand() * 100.0f; // 0.0f에서 100.0f 사이의 난수를 생성합니다.
				if(Rate <= CriticalRate)
				{
					DamageInfo.Amount = DamageAmount * GetStatSystem()->GetCharacterStat().CriticalDamage * 0.01f;
					DamageInfo.bIsCritical = true;
				}
				else
				{
					DamageInfo.bIsCritical = false;
					DamageInfo.Amount = DamageAmount;
				}
				
				bool bWasDamaged = IInterface_PRDamageable::Execute_TakeDamage(HitResult.GetActor(), DamageInfo);
				if(bWasDamaged)
				{
					// GetEffectSystem()->SpawnNiagaraEffectAtLocation(HitEffect,HitResult.Location);
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagaraEffect, HitResult.Location);
				}
			}
		}
	}
}
#pragma endregion 

#pragma region StateSystem
bool APRBaseCharacter::IsDead() const
{
	if(GetStateSystem())
	{
		return GetStateSystem()->IsDead();
	}

	return false;
}

bool APRBaseCharacter::IsInvincible() const
{
	if(GetStateSystem())
	{
		return GetStateSystem()->IsInvincible();
	}

	return false;
}

bool APRBaseCharacter::IsBlocking() const
{
	if(GetStateSystem())
	{
		return GetStateSystem()->IsBlocking();
	}

	return false;
}
#pragma endregion

#pragma region Locomotion
void APRBaseCharacter::ToggleWalk()
{
	if(GetCharacterMovement())
	{
		if(GetCharacterMovement()->MaxWalkSpeed != WalkSpeed)
		{
			// Walk
			SetWalkLocomotion();
		}
		else
		{
			// Run
			SetRunLocomotion();
		}
	}
}

void APRBaseCharacter::SetWalkLocomotion()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxAcceleration = 350.0f;
}

void APRBaseCharacter::SetRunLocomotion()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	GetCharacterMovement()->MaxAcceleration = 1000.0f;
}

float APRBaseCharacter::GetWalkSpeed() const
{
	return WalkSpeed;
}

float APRBaseCharacter::GetRunSpeed() const
{
	return RunSpeed;
}

float APRBaseCharacter::GetSprintSpeed() const
{
	return SprintSpeed;
}
#pragma endregion 

