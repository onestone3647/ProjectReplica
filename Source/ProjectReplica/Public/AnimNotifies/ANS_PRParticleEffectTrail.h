// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState_TimedParticleEffect.h"
#include "ANS_PRParticleEffectTrail.generated.h"

/**
 * 캐릭터의 EffectSystem에서 가져온 ParticleEffect Trail을 가져와 Spawn하는 AnimNotifyState 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRParticleEffectTrail : public UAnimNotifyState_TimedParticleEffect
{
	GENERATED_BODY()
	
};
