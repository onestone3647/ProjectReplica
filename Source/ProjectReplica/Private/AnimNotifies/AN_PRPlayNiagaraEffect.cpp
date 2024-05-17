// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRPlayNiagaraEffect.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PREffectSystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Effects/PRNiagaraEffect.h"

UFXSystemComponent* UAN_PRPlayNiagaraEffect::SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UFXSystemComponent* ReturnComp = nullptr;

	// Template이 유효한지 확인합니다.
	if(Template)
	{
		// Template이 루프되는지 확인하고, 루프되면 Spawn하지 않습니다.
		if(Template->IsLooping())
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
				APRNiagaraEffect* SpawnNiagaraEffect = nullptr;
				// Attached가 true이면 특정 소켓에 연결된 위치에 Effect를 Spawn합니다.
				if(Attached)
				{
					SpawnNiagaraEffect = EffectSystem->SpawnNiagaraEffectAttached(Template, MeshComp, SocketName, LocationOffset, RotationOffset, Scale, true);
				}
				else
				{
					// 특정 위치에 Effect를 Spawn합니다.
					const FTransform MeshTransform = MeshComp->GetSocketTransform(SocketName);
					SpawnNiagaraEffect = EffectSystem->SpawnNiagaraEffectAtLocation(Template, MeshTransform.TransformPosition(LocationOffset), (MeshTransform.GetRotation() * RotationOffsetQuat).Rotator(), Scale, true);
				}

				if(IsValid(SpawnNiagaraEffect))
				{
					ReturnComp = SpawnNiagaraEffect->GetFXSystemComponent();
					if(ReturnComp)
					{
						ReturnComp->SetUsingAbsoluteScale(bAbsoluteScale);
						ReturnComp->SetRelativeScale3D_Direct(Scale);

						return ReturnComp;
					}
				}
			}
		}

		// EffectSystem이 없거나 Effect가 존재하지 않을 경우 일반적인 방법으로 Effect를 Spawn합니다.
		if(Attached)
		{
			ReturnComp = UNiagaraFunctionLibrary::SpawnSystemAttached(Template, MeshComp, SocketName, LocationOffset, RotationOffset, EAttachLocation::KeepRelativeOffset, true);
		}
		else
		{
			const FTransform MeshTransform = MeshComp->GetSocketTransform(SocketName);
			ReturnComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(MeshComp->GetWorld(), Template, MeshTransform.TransformPosition(LocationOffset), (MeshTransform.GetRotation() * RotationOffsetQuat).Rotator(), FVector(1.0f),true);
		}

		if(ReturnComp != nullptr)
		{
			ReturnComp->SetUsingAbsoluteScale(bAbsoluteScale);
			ReturnComp->SetRelativeScale3D_Direct(Scale);
		}
	}
	
	return ReturnComp;
}
