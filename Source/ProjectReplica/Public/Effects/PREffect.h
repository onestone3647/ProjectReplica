// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PRPoolableInterface.h"
#include "PREffect.generated.h"

class UFXSystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectDeactivate, APREffect*, Effect);

/**
 * EffectSystem이 관리하는 이펙트 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APREffect : public AActor, public IPRPoolableInterface
{
	GENERATED_BODY()
	
public:	
	APREffect();

protected:
	virtual void BeginPlay() override;

#pragma region PooledableInterface
protected:
	/** 오브젝트가 활성화 되었는지 확인하는 함수입니다. */
	virtual bool IsActivate_Implementation() const override;
	
	/** 오브젝트를 활성화하는 함수입니다. */
	virtual void Activate_Implementation() override;

	/** 오브젝트를 비활성화하는 함수입니다. */
	virtual void Deactivate_Implementation() override;
	
	/** 오브젝트의 PoolIndex를 반환하는 함수입니다. */
	virtual int32 GetPoolIndex_Implementation() const override;

	/** 수명을 반환하는 함수입니다. */
	virtual float GetLifespan_Implementation() const override;

	/**
	 * 수명을 설정하는 함수입니다.
	 * 
	 * @param NewLifespan 설정할 수명입니다.
	 */	
	virtual void SetLifespan_Implementation(float NewLifespan) override;
#pragma endregion

public:
	/**
	 * 이펙트를 지정한 위치에 Spawn하는 함수입니다.
	 *
	 * @param Location 이펙트를 생성할 위치
	 * @param Rotation 이펙트에 적용한 회전 값
	 * @param Scale 이펙트에 적용할 크기
	 * @param bAutoActivate true일 경우 이펙트를 Spawn하자마다 이펙트를 실행합니다. false일 경우 이펙트를 실행하지 않습니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual void SpawnEffectAtLocation(FVector Location, FRotator Rotation, FVector Scale, bool bAutoActivate = true, bool bReset = false);

	/**
	 * 이펙트를 지정한 Component에 부착하여 Spawn하는 함수입니다.
	 *
	 * @param Parent 이펙트를 부착할 Component
	 * @param AttachSocketName 부착할 소켓의 이름
	 * @param Location 이펙트를 생성할 위치
	 * @param Rotation 이펙트에 적용한 회전 값
	 * @param Scale 이펙트에 적용할 크기
	 * @param LocationType 이펙트를 부착할 위치 타입
	 * @param bAutoActivate true일 경우 이펙트를 Spawn하자마다 이펙트를 실행합니다. false일 경우 이펙트를 실행하지 않습니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual void SpawnEffectAttached(USceneComponent* Parent, FName AttachSocketName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType = EAttachLocation::KeepWorldPosition, bool bAutoActivate = true, bool bReset = false);
	
	/**
	 * 이펙트를 활성화하는 함수입니다.
	 *
	 * @param bReset 처음부터 다시 재생할지 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual void ActivateEffect(bool bReset = false);

	/** 이펙트를 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual void DeactivateEffect();

	/** FXSystemComponent를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual UFXSystemComponent* GetFXSystemComponent() const; 

protected:
	/**
	 * 이펙트를 초기화하는 함수입니다.
	 *
	 * @param NewEffectOwner 이펙트의 소유자
	 * @param NewPoolIndex 이펙트 풀의 Index
	 * @param NewLifespan 이펙트의 수명
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual void InitializeEffect(AActor* NewEffectOwner = nullptr, int32 NewPoolIndex= -1, float NewLifespan = 0.0f);
	
	/** 입력받은 인자로 이펙트의 수명을 설정하는 함수입니다. */
	void SetEffectLifespan(float NewEffectLifespan);

	/** 이펙트가 비활성화될 때 실행하는 함수입니다. */
	UFUNCTION()
	void OnDeactivate();

protected:
	/** 이펙트의 활성화를 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffect")
	bool bActivate;
	
	/** 이펙트의 수명입니다. 수명이 다할 경우 이펙트를 비활성화합니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PREffect")
	float EffectLifespan;

	/** 이펙트의 수명을 관리하는 TimerHandle입니다. */
	UPROPERTY(BlueprintReadOnly, Category = "PREffect")
	FTimerHandle EffectLifespanTimerHandle;

	/** 이펙트의 소유자입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PREffect")
	TObjectPtr<AActor> EffectOwner;

	/** 풀의 Index입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PREffect")
	int32 PoolIndex;

public:
	/** EffectLifespan을 반환하는 함수입니다. */
	float GetEffectLifespan() const;
	
	/** EffectOwner를 반환하는 함수입니다. */
	FORCEINLINE AActor* GetEffectOwner() const { return EffectOwner; }

public:
	/** 이펙트가 비활성화될 때 실행하는 델리게이트입니다. */
	FOnEffectDeactivate OnEffectDeactivateDelegate;
};
