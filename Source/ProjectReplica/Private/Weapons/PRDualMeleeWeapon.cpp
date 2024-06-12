// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PRDualMeleeWeapon.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PREffectSystemComponent.h"
#include "Effects/PRNiagaraEffect.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

APRDualMeleeWeapon::APRDualMeleeWeapon()
{
	// WeaponInfo
	WeaponType = EPRWeaponType::WeaponType_None;

	// MainWeapon
	MainWeapon->SetRelativeLocation(FVector(-10.0f, 0.0f, 0.0f));
	
	// SubWeapon
	SubWeapon = CreateDefaultSubobject<USceneComponent>(TEXT("SubWeapon"));
	SubWeapon->SetupAttachment(GetRootComponent());
	SubWeapon->SetRelativeLocation(FVector(10.0f, 0.0f, 0.0f));
	SubWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubWeaponMesh"));
	SubWeaponMesh->SetupAttachment(SubWeapon);
	SubWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);			// 무기의 외형은 콜리전을 가지지 않습니다.
	SubWeaponDrawSocketName = TEXT("SubWeapon");
	SubWeaponSheatheSocketName = TEXT("SubWeaponSheathe");
	SubWeaponDrawLocationOffset = FVector::ZeroVector;
	SubWeaponDrawRotationOffset = FRotator::ZeroRotator;
	SubWeaponSheatheLocationOffset = FVector::ZeroVector;
	SubWeaponSheatheRotationOffset = FRotator::ZeroRotator;
}

void APRDualMeleeWeapon::Draw(bool bActivateSpawnEffect)
{
	if(IsValid(GetPROwner()))
	{
		const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		AttachToComponent(GetPROwner()->GetMesh(), AttachmentTransformRules);
		MainWeapon->AttachToComponent(GetPROwner()->GetMesh(), AttachmentTransformRules, MainWeaponDrawSocketName);
		MainWeaponMesh->SetRelativeLocationAndRotation(MainWeaponDrawLocationOffset, MainWeaponDrawRotationOffset);
		SubWeapon->AttachToComponent(GetPROwner()->GetMesh(), AttachmentTransformRules, SubWeaponDrawSocketName);
		SubWeaponMesh->SetRelativeLocationAndRotation(SubWeaponDrawLocationOffset, SubWeaponDrawRotationOffset);

		// SpawnEffect를 Spawn합니다.
		if(bActivateSpawnEffect)
		{
			// MainWeaponSpawnEffect
			APRNiagaraEffect* MainWeaponSpawnNiagaraEffect = GetPROwner()->GetEffectSystem()->SpawnNiagaraEffectAtLocation(SpawnNiagaraEffect, MainWeapon->GetComponentLocation(),
																																MainWeaponMesh->GetComponentRotation(), FVector(1.0f),
																																true, true);
			if(IsValid(MainWeaponSpawnNiagaraEffect))
			{
				UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMeshComponent(MainWeaponSpawnNiagaraEffect->GetNiagaraEffect(), WeaponMeshVariableName.ToString(), MainWeaponMesh);
				
				// LinearColor
				MainWeaponSpawnNiagaraEffect->GetNiagaraEffect()->SetVariableLinearColor(EffectColorVariableName, EffectColor);
			}

			// SubWeaponSpawnEffect
			APRNiagaraEffect* SubWeaponSpawnNiagaraEffect = GetPROwner()->GetEffectSystem()->SpawnNiagaraEffectAtLocation(SpawnNiagaraEffect, SubWeapon->GetComponentLocation(),
																																SubWeaponMesh->GetComponentRotation(), FVector(1.0f),
																																true, true);
			if(IsValid(SubWeaponSpawnNiagaraEffect))
			{
				// StaticMesh
				UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMeshComponent(SubWeaponSpawnNiagaraEffect->GetNiagaraEffect(), WeaponMeshVariableName.ToString(), SubWeaponMesh);

				// LinearColor
				SubWeaponSpawnNiagaraEffect->GetNiagaraEffect()->SetVariableLinearColor(EffectColorVariableName, EffectColor);
			}
		}
		
		// 활성하되지 않았을 경우 활성화합니다.
		if(IsActivate() == false)
		{
			Activate();
		}
	}
}

void APRDualMeleeWeapon::Sheathe(bool bActivateSpawnEffect, bool bVisible)
{
	if(IsValid(GetPROwner()))
	{
		// SpawnEffect를 Spawn합니다.
		if(bActivateSpawnEffect)
		{
			// MainWeaponSpawnEffect
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

			// SubWeaponSpawnEffect
			APRNiagaraEffect* SubWeaponSpawnNiagaraEffect = GetPROwner()->GetEffectSystem()->SpawnNiagaraEffectAtLocation(SpawnNiagaraEffect, SubWeapon->GetComponentLocation(),
																															SubWeaponMesh->GetComponentRotation(), FVector(1.0f),
																															true, true);
			if(IsValid(SubWeaponSpawnNiagaraEffect))
			{
				// StaticMesh
				UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMeshComponent(SubWeaponSpawnNiagaraEffect->GetNiagaraEffect(), WeaponMeshVariableName.ToString(), SubWeaponMesh);

				// LinearColor
				SubWeaponSpawnNiagaraEffect->GetNiagaraEffect()->SetVariableLinearColor(EffectColorVariableName, EffectColor);
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
		SubWeapon->AttachToComponent(GetPROwner()->GetMesh(), AttachmentTransformRules, SubWeaponSheatheSocketName);
		SubWeaponMesh->SetRelativeLocationAndRotation(SubWeaponSheatheLocationOffset, SubWeaponSheatheRotationOffset);
	}
}
