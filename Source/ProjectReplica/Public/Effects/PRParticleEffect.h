// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Effects/PREffect.h"
#include "PRParticleEffect.generated.h"

class UParticleSystem;
class UParticleSystemComponent;

/**
 * EffectSystem이 관리하는 파티클 이펙트 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRParticleEffect : public APREffect
{
	GENERATED_BODY()

public:
	APRParticleEffect();

public:
	/**
	 * 인자로 받은 ParticleSystem을 기반으로 ParticleEffect를 초기화하는 함수입니다.
	 *
	 * @param ParticleSystem 사용할 이펙트
	 * @param NewEffectOwner 이펙트의 소유자
	 * @param NewPoolIndex 이펙트 풀의 Index
	 * @param NewLifespan 이펙트의 수명
	 */
	UFUNCTION(BlueprintCallable, Category = "PRParticleEffect")
	void InitializeParticleEffect(UParticleSystem* ParticleSystem = nullptr, AActor* NewEffectOwner = nullptr, int32 NewPoolIndex= -1, float NewLifespan = 0.0f);
	
	/** 이펙트를 활성화하는 함수입니다. */
	virtual void Activate() override;

	/** 이펙트를 비활성화하는 함수입니다. */
	virtual void Deactivate() override;

	/** FXSystemComponent를 반환하는 함수입니다. */
	virtual UFXSystemComponent* GetFXSystemComponent() const; 

	/** ParticleEffect의 에셋을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRParticleEffect")
	UParticleSystem* GetParticleEffectAsset() const;
	
private:
	/** Spawn한 ParticleEffect입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRParticleEffect", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystemComponent> ParticleEffect;

public:
	/** ParticleEffect를 반환하는 함수입니다. */
	TObjectPtr<UParticleSystemComponent> GetParticleEffect() const;
};
