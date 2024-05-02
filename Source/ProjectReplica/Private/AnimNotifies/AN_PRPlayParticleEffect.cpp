// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRPlayParticleEffect.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PREffectSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Effects/PRParticleEffect.h"
#include "Kismet/GameplayStatics.h"

UParticleSystemComponent* UAN_PRPlayParticleEffect::SpawnParticleSystem(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UParticleSystemComponent* ReturnComp = nullptr;

	// PSTemplate이 유효한지 확인합니다.
	if(PSTemplate)
	{
		// PSTemplate이 루프되는지 확인하고, 루프되면 Spawn하지 않습니다.
		if(PSTemplate->IsLooping())
		{
			return ReturnComp;
		}

		// EffectSystem에 Effect가 존재할 경우 EffectSystem에서 Effect를 가져와 Spawn합니다.
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner))
		{
			UPREffectSystemComponent* EffectSystem = PROwner->GetEffectSystem();
			if(EffectSystem)
			{
				APRParticleEffect* SpawnParticleEffect = nullptr;
				// Attached가 true이면 특정 소켓에 연결된 위치에 Effect를 Spawn합니다.
				if(Attached)
				{
					SpawnParticleEffect = EffectSystem->SpawnParticleEffectAttached(PSTemplate, MeshComp, SocketName, LocationOffset, RotationOffset, Scale, true);
				}
				else
				{
					// 특정 위치에 Effect를 Spawn합니다.
					const FTransform MeshTransform = MeshComp->GetSocketTransform(SocketName);
					SpawnParticleEffect = EffectSystem->SpawnParticleEffectAtLocation(PSTemplate, MeshTransform.TransformPosition(LocationOffset), (MeshTransform.GetRotation() * FQuat(RotationOffset)).Rotator(), Scale, true);
				}

				return ReturnComp;
			}
		}

		// EffectSystem이 없거나 Effect가 존재하지 않을 경우 일반적인 방법으로 Effect를 Spawn합니다.
		if(Attached)
		{
			ReturnComp = UGameplayStatics::SpawnEmitterAttached(PSTemplate, MeshComp, SocketName, LocationOffset, RotationOffset, EAttachLocation::KeepRelativeOffset, true);
		}
		else
		{
			const FTransform MeshTransform = MeshComp->GetSocketTransform(SocketName);
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(MeshTransform.TransformPosition(LocationOffset));
			SpawnTransform.SetRotation(MeshTransform.GetRotation() * FQuat(RotationOffset));
			SpawnTransform.SetScale3D(Scale);
			ReturnComp = UGameplayStatics::SpawnEmitterAtLocation(MeshComp->GetWorld(), PSTemplate, SpawnTransform);
		}
	}
	
	return ReturnComp;
}
