// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PRBaseWeapon.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PREffectSystemComponent.h"
#include "Effects/PRNiagaraEffect.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

APRBaseWeapon::APRBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// WeaponInfo
	WeaponType = EPRWeaponType::WeaponType_None;
	WeaponStat = FPRWeaponStat();
	bActivate = false;
	PROwner = nullptr;

	// SpawnEffect
	SpawnNiagaraEffect = nullptr;
	EffectColor = FLinearColor(20.0f, 15.0f, 200.0f, 1.0f);
	WeaponMeshVariableName = TEXT("WeaponMesh");
	EffectColorVariableName = TEXT("EffectColor");

	// MainWeapon
	MainWeapon = CreateDefaultSubobject<USceneComponent>(TEXT("MainWeapon"));
	MainWeapon->SetupAttachment(Root);
	MainWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainWeaponMesh"));
	MainWeaponMesh->SetupAttachment(MainWeapon);
	MainWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);			// 무기의 외형은 콜리전을 가지지 않습니다.
	MainWeaponDrawSocketName = TEXT("MainWeapon");
	MainWeaponSheatheSocketName = TEXT("MainWeaponSheathe");
	MainWeaponDrawLocationOffset = FVector::ZeroVector;
	MainWeaponDrawRotationOffset = FRotator::ZeroRotator;
	MainWeaponSheatheLocationOffset = FVector::ZeroVector;
	MainWeaponSheatheRotationOffset = FRotator::ZeroRotator;
}

void APRBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void APRBaseWeapon::InitializeWeapon(APRBaseCharacter* NewPROwner, FPRWeaponStat NewWeaponStat)
{
	if(IsValid(NewPROwner))
	{
		PROwner = NewPROwner;
		WeaponStat = NewWeaponStat;
	}
}

void APRBaseWeapon::Activate()
{
	bActivate = true;
	SetActorHiddenInGame(!bActivate);
}

void APRBaseWeapon::Deactivate()
{
	bActivate = false;
	SetActorHiddenInGame(!bActivate);
}

bool APRBaseWeapon::IsActivate() const
{
	return bActivate;
}

void APRBaseWeapon::Draw(bool bActivateSpawnEffect)
{
	// 무기를 DrawSocket에 부착하고 활성화되지 않았을 경우 활성화합니다.
	if(IsValid(GetPROwner()))
	{
		const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		AttachToComponent(GetPROwner()->GetMesh(), AttachmentTransformRules);
		MainWeapon->AttachToComponent(GetPROwner()->GetMesh(), AttachmentTransformRules, MainWeaponDrawSocketName);
		MainWeaponMesh->SetRelativeLocationAndRotation(MainWeaponDrawLocationOffset, MainWeaponDrawRotationOffset);

		// SpawnEffect를 Spawn합니다.
		if(bActivateSpawnEffect)
		{
			APRNiagaraEffect* MainWeaponSpawnNiagaraEffect = GetPROwner()->GetEffectSystem()->SpawnNiagaraEffectAtLocation(SpawnNiagaraEffect, MainWeapon->GetComponentLocation(),
																																MainWeaponMesh->GetComponentRotation(), FVector(1.0f),
																																true, true);
			
			if(IsValid(MainWeaponSpawnNiagaraEffect))
			{
				// StaticMesh
				UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMeshComponent(MainWeaponSpawnNiagaraEffect->GetNiagaraEffect(), WeaponMeshVariableName.ToString(), MainWeaponMesh);

				// LinearColor
				MainWeaponSpawnNiagaraEffect->GetNiagaraEffect()->SetVariableLinearColor(EffectColorVariableName, EffectColor);
			}
		}
		
		if(IsActivate() == false)
		{
			Activate();
		}
	}
}

void APRBaseWeapon::Sheathe(bool bActivateSpawnEffect, bool bVisible)
{
	if(IsValid(GetPROwner()))
	{
		// SpawnEffect를 Spawn합니다.
		if(bActivateSpawnEffect)
		{
			APRNiagaraEffect* MainWeaponSpawnNiagaraEffect = GetPROwner()->GetEffectSystem()->SpawnNiagaraEffectAtLocation(SpawnNiagaraEffect, MainWeapon->GetComponentLocation(),
																																MainWeaponMesh->GetComponentRotation(), FVector(1.0f),
																																true, true);
			if(IsValid(MainWeaponSpawnNiagaraEffect))
			{
				// StaticMesh
				MainWeaponSpawnNiagaraEffect->GetNiagaraEffect()->SetVariableStaticMesh(WeaponMeshVariableName, MainWeaponMesh->GetStaticMesh());

				// LinearColor
				MainWeaponSpawnNiagaraEffect->GetNiagaraEffect()->SetVariableLinearColor(EffectColorVariableName, EffectColor);
			}
		}
		
		// 무기를 숨길 경우 비활성화합니다.
		if(!bVisible)
		{
			Deactivate();
		}
		
		const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		AttachToComponent(GetPROwner()->GetMesh(), AttachmentTransformRules);
		MainWeapon->AttachToComponent(GetPROwner()->GetMesh(), AttachmentTransformRules, MainWeaponSheatheSocketName);
		MainWeaponMesh->SetRelativeLocationAndRotation(MainWeaponSheatheLocationOffset, MainWeaponSheatheRotationOffset);
	}
}

APRBaseCharacter* APRBaseWeapon::GetPROwner() const
{
	return PROwner;
}

