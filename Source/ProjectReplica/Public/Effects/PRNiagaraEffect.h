// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Effects/PREffect.h"
#include "PRNiagaraEffect.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

/**
 * EffectSystem이 관리하는 나이아가라 이펙트 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRNiagaraEffect : public APREffect
{
	GENERATED_BODY()

public:
	APRNiagaraEffect();

public:
	/**
	 * 인자로 받은 NiagaraSystem을 기반으로 NiagaraEffect를 초기화하는 함수입니다.
	 *
	 * @param NiagaraSystem 사용할 이펙트
	 * @param NewEffectOwner 이펙트의 소유자
	 * @param NewPoolIndex 이펙트 풀의 Index
	 * @param NewLifespan 이펙트의 수명
	 */
	UFUNCTION(BlueprintCallable, Category = "PRNiagaraEffect")
	void InitializeNiagaraEffect(UNiagaraSystem* NiagaraSystem = nullptr, AActor* NewEffectOwner = nullptr, int32 NewPoolIndex= -1, float NewLifespan = 0.0f);
	
	/** 이펙트를 활성화하는 함수입니다. */
	virtual void Activate() override;

	/** 이펙트를 비활성화하는 함수입니다. */
	virtual void Deactivate() override;

	/** FXSystemComponent를 반환하는 함수입니다. */
	virtual UFXSystemComponent* GetFXSystemComponent() const; 

	/** NiagaraEffect의 에셋을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRNiagaraEffect")
	UNiagaraSystem* GetNiagaraEffectAsset() const;
	
private:
	/** Spawn한 NiagaraEffect입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRNiagaraEffect", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraComponent> NiagaraEffect;

public:
	/** NiagaraEffect를 반환하는 함수입니다. */
	TObjectPtr<UNiagaraComponent> GetNiagaraEffect() const;
};
