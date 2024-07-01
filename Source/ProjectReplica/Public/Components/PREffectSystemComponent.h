// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "PRBaseObjectPoolSystemComponent.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystem.h"
#include "Effects/PRNiagaraEffect.h"
#include "Effects/PRParticleEffect.h"
#include "PREffectSystemComponent.generated.h"


#pragma region Structs
/**
 * NiagaraEffect를 보관하는 Pool을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRNiagaraEffectPool
{
	GENERATED_BODY()

public:
	FPRNiagaraEffectPool()
		: PooledEffects()
	{}

	FPRNiagaraEffectPool(const TArray<TObjectPtr<APRNiagaraEffect>>& NewPooledEffects)
		: PooledEffects(NewPooledEffects)
	{}

public:
	/** Pool에 보관된 NiagaraEffect들의 Array입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRNiagaraSystemPool")
	TArray<TObjectPtr<APRNiagaraEffect>> PooledEffects;
};

/**
 * 여러 NiagaraEffect Pool을 보관하는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRNiagaraEffectObjectPool
{
	GENERATED_BODY()

public:
	FPRNiagaraEffectObjectPool()
		: Pool()
	{}

	FPRNiagaraEffectObjectPool(const TMap<TObjectPtr<UNiagaraSystem>, FPRNiagaraEffectPool>& NewPool)
		: Pool(NewPool)
	{}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRNiagaraEffectObjectPool")
	TMap<TObjectPtr<UNiagaraSystem>, FPRNiagaraEffectPool> Pool;
};

/**
 * NiagaraEffectPool의 설정 값을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRNiagaraEffectPoolSettings : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRNiagaraEffectPoolSettings()
		: NiagaraSystem(nullptr)
		, PoolSize(0)
		, EffectLifespan(0.0f)
	{}

	FPRNiagaraEffectPoolSettings(TObjectPtr<UNiagaraSystem> NewNiagaraSystem, int32 NewPoolSize, float NewEffectLifespan)
		: NiagaraSystem(NewNiagaraSystem)
		, PoolSize(NewPoolSize)
		, EffectLifespan(NewEffectLifespan)
	{}

public:
	/** Pool에 넣을 NiagaraSystem입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRNiagaraEffectPoolSettings")
	TObjectPtr<UNiagaraSystem> NiagaraSystem;

	/** Pool의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRNiagaraEffectPoolSettings")
	int32 PoolSize;

	/** 이펙트의 수명입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRNiagaraEffectPoolSettings")
	float EffectLifespan;

public:
	/**
	 * 주어진 NiagaraEffectPoolSettings와 같은지 확인하는 ==연산자 오버로딩입니다.
	 * 
	 * @param TargetNiagaraEffectPoolSettings 비교하는 NiagaraEffectPoolSettings와 같은지 확인할 NiagaraEffectPoolSettings입니다.
	 * @return 주어진 NiagaraEffectPoolSettings와 같을 경우 true를 반환합니다. 그렇지 않을 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator==(const FPRNiagaraEffectPoolSettings& TargetNiagaraEffectPoolSettings) const
	{
		return this->NiagaraSystem == TargetNiagaraEffectPoolSettings.NiagaraSystem
				&& this->PoolSize == TargetNiagaraEffectPoolSettings.PoolSize
				&& this->EffectLifespan == TargetNiagaraEffectPoolSettings.EffectLifespan;
	}

	/**
	 * 주어진 NiagaraEffectPoolSettings와 같지 않은지 확인하는 !=연산자 오버로딩입니다.
	 * 
	 * @param TargetNiagaraEffectPoolSettings 비교하는 NiagaraEffectPoolSettings와 같지 않은지 확인할 NiagaraEffectPoolSettings입니다.
	 * @return 주어진 NiagaraEffectPoolSettings와 같지 않을 경우 true를 반환합니다. 그렇지 않을 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator!=(const FPRNiagaraEffectPoolSettings& TargetNiagaraEffectPoolSettings) const
	{
		return this->NiagaraSystem != TargetNiagaraEffectPoolSettings.NiagaraSystem
				|| this->PoolSize != TargetNiagaraEffectPoolSettings.PoolSize
				|| this->EffectLifespan != TargetNiagaraEffectPoolSettings.EffectLifespan;
	}
};

/**
 * NiagaraSystem별로 활성화된 NiagaraEffect들의 Index를 보관하는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRActivateNiagaraEffectIndexList
{
	GENERATED_BODY()

public:
	FPRActivateNiagaraEffectIndexList()
		: List()
	{}

	FPRActivateNiagaraEffectIndexList(const TMap<TObjectPtr<UNiagaraSystem>, FPRActivateIndexList>& NewList)
		: List(NewList)
	{}

public:
	/** NiagaraSystem과 활성화된 Index를 보관하는 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRActivateNiagaraEffectIndexList")
	TMap<TObjectPtr<UNiagaraSystem>, FPRActivateIndexList> List;

public:
	/**
	 * 주어진 NiagaraSystem에 해당하는 Indexes를 반환하는 함수입니다.
	 *
	 * @param NiagaraSystemToFind Indexes를 찾을 NiagaraSystem입니다.
	 * @return Indexes를 찾을 경우 Indexes를 반환합니다. 못찾았을 경우 nullptr을 반환합니다.
	 */
	TSet<int32>* GetIndexesForNiagaraSystem(const UNiagaraSystem& NiagaraSystemToFind)
	{
		if(!IsValid(&NiagaraSystemToFind))
		{
			return nullptr;
		}

		FPRActivateIndexList* ActivateIndexList = List.Find(NiagaraSystemToFind);
		if(ActivateIndexList)
		{
			return &ActivateIndexList->Indexes;
		}
		
		return nullptr;
	}
};

/**
 * NiagaraSystem별로 사용된 NiagaraEffect들의 Index를 보관하는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRUsedNiagaraEffectIndexList
{
	GENERATED_BODY()

public:
	FPRUsedNiagaraEffectIndexList()
		: List()
	{}

	FPRUsedNiagaraEffectIndexList(const TMap<TObjectPtr<UNiagaraSystem>, FPRUsedIndexList>& NewList)
		: List(NewList)
	{}

public:
	/** NiagaraSystem과 해당 NiagaraSystem의 이전에 사용된 Index를 보관하는 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRUsedNiagaraEffectIndexList")
	TMap<TObjectPtr<UNiagaraSystem>, FPRUsedIndexList> List;
};

/**
 * 동적으로 생성한 NiagaraEffect 목록을 NiagaraSystem별로 보관하는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRDynamicDestroyNiagaraEffectList
{
	GENERATED_BODY()

public:
	FPRDynamicDestroyNiagaraEffectList()
		: List()
	{}

	FPRDynamicDestroyNiagaraEffectList(const TMap<TObjectPtr<UNiagaraSystem>, FPRDynamicDestroyObject>& NewList)
		: List(NewList)
	{}
	
public:
	/** NiagaraSystem과 동적으로 생성한 NiagaraEffect와 해당 NiagaraEffect를 제거하는 TimerHandle을 보관한 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRDynamicDestroyNiagaraEffectList")
	TMap<TObjectPtr<UNiagaraSystem>, FPRDynamicDestroyObject> List;

public:
	/**
	 * 주어진 NiagaraEffect에 해당하는 TimerHandle을 반환하는 함수입니다.
	 *
	 * @param NiagaraEffectToFind TimerHandle을 찾을 NiagaraEffect입니다.
	 * @return TimerHandle을 찾았을 경우 TimerHandle을 반환합니다. 못 찾았을 경우 nullptr을 반환합니다.
	 */
	FTimerHandle* FindTimerHandleForNiagaraEffect(APRNiagaraEffect& NiagaraEffectToFind)
	{
		if(!IsValid(&NiagaraEffectToFind))
		{
			return nullptr;
		}

		FPRDynamicDestroyObject* DestroyObjects = List.Find(NiagaraEffectToFind.GetNiagaraEffectAsset());
		if(DestroyObjects)
		{
			FTimerHandle* FoundTimerHandle = DestroyObjects->TimerHandles.Find(NiagaraEffectToFind);
			if(FoundTimerHandle)
			{
				return FoundTimerHandle;
			}
		}

		return nullptr;
	}
};

/**
 * ParticleEffect를 보관하는 Pool을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRParticleEffectPool
{
	GENERATED_BODY()

public:
	FPRParticleEffectPool()
		: PooledEffects()
	{}

	FPRParticleEffectPool(const TArray<TObjectPtr<APRParticleEffect>>& NewPooledEffects)
		: PooledEffects(NewPooledEffects)
	{}

public:
	/** Pool에 보관된 ParticleEffect들의 Array입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRParticleSystemPool")
	TArray<TObjectPtr<APRParticleEffect>> PooledEffects;
};

/**
 * 여러 ParticleEffect Pool을 보관하는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRParticleEffectObjectPool
{
	GENERATED_BODY()

public:
	FPRParticleEffectObjectPool()
		: Pool()
	{}

	FPRParticleEffectObjectPool(const TMap<TObjectPtr<UParticleSystem>, FPRParticleEffectPool>& NewPool)
		: Pool(NewPool)
	{}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRParticleEffectObjectPool")
	TMap<TObjectPtr<UParticleSystem>, FPRParticleEffectPool> Pool;
};

/**
 * ParticleEffectPool의 설정 값을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRParticleEffectPoolSettings : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRParticleEffectPoolSettings()
		: ParticleSystem(nullptr)
		, PoolSize(0)
		, EffectLifespan(0.0f)
	{}

	FPRParticleEffectPoolSettings(TObjectPtr<UParticleSystem> NewParticleSystem, int32 NewPoolSize, float NewEffectLifespan)
		: ParticleSystem(NewParticleSystem)
		, PoolSize(NewPoolSize)
		, EffectLifespan(NewEffectLifespan)
	{}

public:
	/** Pool에 넣을 ParticleSystem입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRParticleEffectPoolSettings")
	TObjectPtr<UParticleSystem> ParticleSystem;

	/** Pool의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRParticleEffectPoolSettings")
	int32 PoolSize;

	/** 이펙트의 수명입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRParticleEffectPoolSettings")
	float EffectLifespan;

public:
	/**
	 * 주어진 ParticleEffectPoolSettings와 같은지 확인하는 ==연산자 오버로딩입니다.
	 * 
	 * @param TargetParticleEffectPoolSettings 비교하는 ParticleEffectPoolSettings와 같은지 확인할 ParticleEffectPoolSettings입니다.
	 * @return 주어진 ParticleEffectPoolSettings와 같을 경우 true를 반환합니다. 그렇지 않을 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator==(const FPRParticleEffectPoolSettings& TargetParticleEffectPoolSettings) const
	{
		return this->ParticleSystem == TargetParticleEffectPoolSettings.ParticleSystem
				&& this->PoolSize == TargetParticleEffectPoolSettings.PoolSize
				&& this->EffectLifespan == TargetParticleEffectPoolSettings.EffectLifespan;
	}

	/**
	 * 주어진 ParticleEffectPoolSettings와 같지 않은지 확인하는 !=연산자 오버로딩입니다.
	 * 
	 * @param TargetParticleEffectPoolSettings 비교하는 ParticleEffectPoolSettings와 같지 않은지 확인할 ParticleEffectPoolSettings입니다.
	 * @return 주어진 ParticleEffectPoolSettings와 같지 않을 경우 true를 반환합니다. 그렇지 않을 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator!=(const FPRParticleEffectPoolSettings& TargetParticleEffectPoolSettings) const
	{
		return this->ParticleSystem != TargetParticleEffectPoolSettings.ParticleSystem
				|| this->PoolSize != TargetParticleEffectPoolSettings.PoolSize
				|| this->EffectLifespan != TargetParticleEffectPoolSettings.EffectLifespan;
	}
};

/**
 * ParticleSystem별로 활성화된 ParticleEffect들의 Index를 보관하는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRActivateParticleEffectIndexList
{
	GENERATED_BODY()

public:
	FPRActivateParticleEffectIndexList()
		: List()
	{}

	FPRActivateParticleEffectIndexList(const TMap<TObjectPtr<UParticleSystem>, FPRActivateIndexList>& NewList)
		: List(NewList)
	{}

public:
	/** ParticleSystem과 활성화된 Index를 보관하는 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRActivateParticleEffectIndexList")
	TMap<TObjectPtr<UParticleSystem>, FPRActivateIndexList> List;

public:
	/**
	 * 주어진 ParticleSystem에 해당하는 Indexes를 반환하는 함수입니다.
	 *
	 * @param ParticleSystemToFind Indexes를 찾을 ParticleSystem입니다.
	 * @return Indexes를 찾을 경우 Indexes를 반환합니다. 못찾았을 경우 nullptr을 반환합니다.
	 */
	TSet<int32>* GetIndexesForParticleSystem(const UParticleSystem& ParticleSystemToFind)
	{
		if(!IsValid(&ParticleSystemToFind))
		{
			return nullptr;
		}

		FPRActivateIndexList* ActivateIndexList = List.Find(ParticleSystemToFind);
		if(ActivateIndexList)
		{
			return &ActivateIndexList->Indexes;
		}
		
		return nullptr;
	}
};

/**
 * ParticleSystem별로 사용된 ParticleEffect들의 Index를 보관하는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRUsedParticleEffectIndexList
{
	GENERATED_BODY()

public:
	FPRUsedParticleEffectIndexList()
		: List()
	{}

	FPRUsedParticleEffectIndexList(const TMap<TObjectPtr<UParticleSystem>, FPRUsedIndexList>& NewList)
		: List(NewList)
	{}

public:
	/** ParticleSystem과 해당 ParticleSystem의 이전에 사용된 Index를 보관하는 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRUsedParticleEffectIndexList")
	TMap<TObjectPtr<UParticleSystem>, FPRUsedIndexList> List;
};

/**
 * 동적으로 생성한 ParticleEffect 목록을 ParticleSystem별로 보관하는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRDynamicDestroyParticleEffectList
{
	GENERATED_BODY()

public:
	FPRDynamicDestroyParticleEffectList()
		: List()
	{}

	FPRDynamicDestroyParticleEffectList(const TMap<TObjectPtr<UParticleSystem>, FPRDynamicDestroyObject>& NewList)
		: List(NewList)
	{}
	
public:
	/** ParticleSystem과 동적으로 생성한 ParticleEffect와 해당 ParticleEffect를 제거하는 TimerHandle을 보관한 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRDynamicDestroyParticleEffectList")
	TMap<TObjectPtr<UParticleSystem>, FPRDynamicDestroyObject> List;

public:
	/**
	 * 주어진 ParticleEffect에 해당하는 TimerHandle을 반환하는 함수입니다.
	 *
	 * @param ParticleEffectToFind TimerHandle을 찾을 ParticleEffect입니다.
	 * @return TimerHandle을 찾았을 경우 TimerHandle을 반환합니다. 못 찾았을 경우 nullptr을 반환합니다.
	 */
	FTimerHandle* FindTimerHandleForParticleEffect(const APRParticleEffect& ParticleEffectToFind)
	{
		if(!IsValid(&ParticleEffectToFind))
		{
			return nullptr;
		}

		FPRDynamicDestroyObject* DestroyObjects = List.Find(ParticleEffectToFind.GetParticleEffectAsset());
		if(DestroyObjects)
		{
			FTimerHandle* FoundTimerHandle = DestroyObjects->TimerHandles.Find(ParticleEffectToFind);
			if(FoundTimerHandle)
			{
				return FoundTimerHandle;
			}
		}

		return nullptr;
	}
};
#pragma endregion

/**
 * Owner가 사용하는 이펙트를 관리하는 ActorComponent 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPREffectSystemComponent : public UPRBaseObjectPoolSystemComponent
{
	GENERATED_BODY()

public:
	UPREffectSystemComponent();

#pragma region PRBaseObjectPoolSystem
public:
	/** 기존의 ObjectPool을 제거하고, 새로 ObjectPool을 생성하여 초기화하는 함수입니다. */
	virtual void InitializeObjectPool() override;

	/** 모든 ObjectPool을 제거하는 함수입니다. */
	virtual void ClearAllObjectPool() override;
#pragma endregion

#pragma region NiagaraSystem
public:
	/** 기존의 NiagaraPool을 제거하고, 새로 NiagaraPool을 생성하여 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraSystem")
	void InitializeNiagaraPool();

	/** 모든 NiagaraPool을 제거하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraSystem")
	void ClearAllNiagaraPool();

	/**
	 * NiagaraEffect를 지정한 위치에 Spawn하는 함수입니다.
	 *
	 * @param SpawnEffect Spawn할 NiagaraEffect
	 * @param Location NiagaraEffect를 생성할 위치
	 * @param Rotation NiagaraEffect에 적용한 회전 값
	 * @param Scale NiagaraEffect에 적용할 크기
	 * @param bEffectAutoActivate true일 경우 NiagaraEffect를 Spawn하자마다 NiagaraEffect를 실행합니다. false일 경우 NiagaraEffect를 실행하지 않습니다.
	 * @param bReset 처음부터 다시 재생할지 여부
	 * @return 지정한 위치에 Spawn한 NiagaraEffect입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraEffect")
	APRNiagaraEffect* SpawnNiagaraEffectAtLocation(UNiagaraSystem* SpawnEffect, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.0f), bool bEffectAutoActivate = true, bool bReset = false);
	
	/**
	 * NiagaraEffect를 지정한 Component에 부착하여 Spawn하는 함수입니다.
	 *
	 * @param SpawnEffect Spawn할 NiagaraEffect
	 * @param Parent NiagaraEffect를 부착할 Component
	 * @param AttachSocketName 부착할 소켓의 이름
	 * @param Location NiagaraEffect를 생성할 위치
	 * @param Rotation NiagaraEffect에 적용한 회전 값
	 * @param Scale NiagaraEffect에 적용할 크기
	 * @param bEffectAutoActivate true일 경우 NiagaraEffect를 Spawn하자마다 NiagaraEffect를 실행합니다. false일 경우 NiagaraEffect를 실행하지 않습니다.
	 * @param bReset 처음부터 다시 재생할지 여부
	 * @return 지정한 Component에 부착하여 Spawn한 NiagaraEffect입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraEffect")
	APRNiagaraEffect* SpawnNiagaraEffectAttached(UNiagaraSystem* SpawnEffect, USceneComponent* Parent, FName AttachSocketName, FVector Location, FRotator Rotation, FVector Scale = FVector(1.0f), bool bEffectAutoActivate = true, bool bReset = false);

	/**
	 * 주어진 NiagaraSystem에 해당하는 활성화할 수 있는 NiagaraEffect를 반환하는 함수입니다.
	 *
	 * @param NiagaraSystem 활성화할 수 있는 NiagaraEffect를 찾을 NiagaraSystem입니다.
	 * @return 활성화할 수 있는 NiagaraEffect입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraEffect")
	APRNiagaraEffect* GetActivateableNiagaraEffect(UNiagaraSystem* NiagaraSystem);
	
	/**
	 * 주어진 NiagaraEffect가 활성화되어 있는지 확인하는 함수입니다.
	 * 
	 * @param NiagaraEffect 확인할 NiagaraEffect입니다.
	 * @return NiagaraEffect가 활성화되어 있으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraSystem")
	bool IsActivateNiagaraEffect(APRNiagaraEffect* NiagaraEffect) const;

	/**
	 * 주어진 NiagaraSystem에 해당하는 NiagaraPool이 생성되어 있는지 확인하는 함수입니다.
	 * 
	 * @param NiagaraSystem 확인할 NiagaraSystem입니다.
	 * @return NiagaraPool이 생성되어 있으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraSystem")
	bool IsCreateNiagaraPool(UNiagaraSystem* NiagaraSystem) const;	

	/**
	 * 주어진 NiagaraSystem에 해당하는 ActivateNiagaraIndexList가 생성되어 있는지 확인하는 함수입니다.
	 * 
	 * @param NiagaraSystem 확인할 NiagaraSystem입니다.
	 * @return ActivateNiagaraIndexList가 생성되어 있으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraSystem")
	bool IsCreateActivateNiagaraIndexList(UNiagaraSystem* NiagaraSystem) const;

	/**
	 * 주어진 NiagaraSystem에 해당하는 UsedNiagaraIndexList가 생성되어 있는지 확인하는 함수입니다.
	 * 
	 * @param NiagaraSystem 확인할 NiagaraSystem입니다.
	 * @return UsedNiagaraIndexList가 생성되어 있으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraSystem")
	bool IsCreateUsedNiagaraIndexList(UNiagaraSystem* NiagaraSystem) const;

	/**
	 * 주어진 NiagaraEffect가 동적으로 생성되었는지 확인하는 함수입니다.
	 * 
	 * @param NiagaraEffect 확인할 NiagaraEffect입니다.
	 * @return NiagaraEffect가 동적으로 생성되었으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraSystem")
	bool IsDynamicNiagaraEffect(APRNiagaraEffect* NiagaraEffect) const;

private:
	/**
	 * 주어진 NiagaraPool을 제거하는 함수입니다.
	 * 
	 * @param TargetNiagaraPool 제거할 NiagaraPool입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem|NiagaraSystem")
	void ClearNiagaraPool(FPRNiagaraEffectObjectPool& TargetNiagaraPool);
	
	/**
	 * 주어진 NiagaraPool의 설정 값을 바탕으로 NiagaraPool을 생성하는 함수입니다.
	 *
	 * @param NiagaraPoolSettings NiagaraPool을 생성할 설정 값입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem|NiagaraSystem")
	void CreateNiagaraPool(const FPRNiagaraEffectPoolSettings& NiagaraPoolSettings);

	/**
	 * 주어진 NiagaraSystem의 ActivateNiagaraIndexList를 생성하는 함수입니다.
	 *
	 * @param NiagaraSystem ActivateNiagaraIndexList를 생성할 NiagaraSystem입니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem|NiagaraSystem")
	void CreateActivateNiagaraIndexList(UNiagaraSystem* NiagaraSystem);

	/**
	 * 주어진 NiagaraSystem의 UsedNiagaraIndexList를 생성하는 함수입니다.
	 *
	 * @param NiagaraSystem UsedNiagaraIndexList를 생성할 NiagaraSystem입니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem|NiagaraSystem")
	void CreateUsedNiagaraIndexList(UNiagaraSystem* NiagaraSystem);	

	/**
	 * 주어진 NiagaraSystem을 월드에 PRNiagaraEffect로 Spawn하는 함수입니다.
	 *
	 * @param NiagaraSystem 월드에 Spawn할 NiagaraSystem입니다.
	 * @param PoolIndex 월드에 Spawn한 NiagaraSystem이 NiagaraPool에서 사용하는 Index 값입니다. 
	 * @param Lifespan PRNiagaraEffect의 수명입니다.
	 * @return 월드에 Spawn PRNiagaraEffect입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem|NiagaraSystem")
	APRNiagaraEffect* SpawnNiagaraEffectInWorld(UNiagaraSystem* NiagaraSystem, int32 PoolIndex, float Lifespan);

	/**
	 * 주어진 NiagaraSystem을 월드에 동적으로 Spawn하는 함수입니다.
	 * 동적으로 Spawn한 NiagaraSystem은 비활성화된 후 일정시간이 지나면 제거됩니다.
	 * 
	 * @param NiagaraSystem 월드에 동적으로 Spawn할 NiagaraSystem입니다.
	 * @return 월드에 동적으로 Spawn한 NiagaraEffect입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraEffect")
	APRNiagaraEffect* SpawnDynamicNiagaraEffectInWorld(UNiagaraSystem* NiagaraSystem);	

	/**
	 * 주어진 NiagaraSystem에 해당하는 NiagaraEffect를 초기화한 후 반환하는 함수입니다.
	 * 
	 * @param SpawnEffect 초기화할 NiagaraEffect의 NiagaraSystem입니다.
	 * @return 초기화된 NiagaraEffect입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem|NiagaraSystem")
	APRNiagaraEffect* InitializeNiagaraEffect(UNiagaraSystem* SpawnEffect);

	/**
	 * 주어진 동적으로 생성한 NiagaraEffect를 제거하는 함수입니다.
	 * 
	 * @param TargetDynamicDestroyNiagaraEffectList 제거할 동적으로 생성한 NiagaraEffect의 목록입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem|NiagaraSystem")
	void ClearDynamicDestroyNiagaraList(FPRDynamicDestroyNiagaraEffectList& TargetDynamicDestroyNiagaraEffectList);

	/**
	 * 주어진 NiagaraSystem에 해당하는 NiagaraEffect의 설정 값을 데이터 테이블에서 가져오는 함수입니다.
	 *
	 * @param NiagaraSystem 데이터 테이블에서 설정 값을 가져올 NiagaraSystem입니다.
	 * @return 데이터 테이블에 해당하는 NiagaraEffect의 설정 값이 있으면 반환합니다. 그렇지 않으면 기본 값을 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraEffect")
	FPRNiagaraEffectPoolSettings GetNiagaraEffectPoolSettingsFromDataTable(UNiagaraSystem* NiagaraSystem) const; 

	/**
	 * 주어진 NiagaraEffect가 비활성화될 때 실행하는 함수입니다.
	 *
	 * @param TargetEffect 비활성화되는 Effect입니다.
	 */
	UFUNCTION()
	void OnNiagaraEffectDeactivate(APREffect* TargetEffect);

	/**
	 * 주어진 동적으로 생성한 NiagaraEffect가 비활성화될 때 실행하는 함수입니다.
	 *
	 * @param TargetEffect 비활성화되는 Effect입니다.
	 */
	UFUNCTION()
	void OnDynamicNiagaraEffectDeactivate(APREffect* TargetEffect);

	/**
	 * 동적으로 생성한 NiagaraEffect를 제거하는 함수입니다.
	 *
	 * @param TargetNiagaraEffect 제거하는 동적으로 생성된 NiagaraEffect입니다.
	 */
	UFUNCTION()
	void OnDynamicNiagaraEffectDestroy(APRNiagaraEffect* TargetNiagaraEffect);
	
private:
	/** NiagaraObjectPool의 설정 값을 가진 데이터 테이블입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PREffectSystem|NiagaraSystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> NiagaraPoolSettingsDataTable;
	
	/** NiagaraSystem ObjectPool입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|NiagaraSystem", meta = (AllowPrivateAccess = "true"))
	FPRNiagaraEffectObjectPool NiagaraPool;

	/** 활성화된 NiagaraSystem의 Index 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|NiagaraSystem", meta = (AllowPrivateAccess = "true"))
	FPRActivateNiagaraEffectIndexList ActivateNiagaraIndexList;

	/**
	 * 이전에 사용된 NiagaraSystem들의 Index 목록입니다.
	 * 동적으로 생성하는 NiagaraSystem의 Index에 오류가 생기지 않도록 합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|NiagaraSystem", meta = (AllowPrivateAccess = "true"))
	FPRUsedNiagaraEffectIndexList UsedNiagaraIndexList;

	/** 동적으로 제거할 NiagaraSystem의 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|NiagaraSystem", meta = (AllowPrivateAccess = "true"))
	FPRDynamicDestroyNiagaraEffectList DynamicDestroyNiagaraList;
#pragma endregion

#pragma region ParticleSystem
public:
	/** 기존의 ParticlePool을 제거하고, 새로 ParticlePool을 생성하여 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleSystem")
	void InitializeParticlePool();

	/** 모든 ParticlePool을 제거하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleSystem")
	void ClearAllParticlePool();

	/**
	 * ParticleEffect를 지정한 위치에 Spawn하는 함수입니다.
	 *
	 * @param SpawnEffect Spawn할 ParticleEffect
	 * @param Location ParticleEffect를 생성할 위치
	 * @param Rotation ParticleEffect에 적용한 회전 값
	 * @param Scale ParticleEffect에 적용할 크기
	 * @param bEffectAutoActivate true일 경우 ParticleEffect를 Spawn하자마다 ParticleEffect를 실행합니다. false일 경우 ParticleEffect를 실행하지 않습니다.
	 * @param bReset 처음부터 다시 재생할지 여부
	 * @return 지정한 위치에 Spawn한 ParticleEffect입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	APRParticleEffect* SpawnParticleEffectAtLocation(UParticleSystem* SpawnEffect, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.0f), bool bEffectAutoActivate = true, bool bReset = false);
	
	/**
	 * ParticleEffect를 지정한 Component에 부착하여 Spawn하는 함수입니다.
	 *
	 * @param SpawnEffect Spawn할 ParticleEffect
	 * @param Parent ParticleEffect를 부착할 Component
	 * @param AttachSocketName 부착할 소켓의 이름
	 * @param Location ParticleEffect를 생성할 위치
	 * @param Rotation ParticleEffect에 적용한 회전 값
	 * @param Scale ParticleEffect에 적용할 크기
	 * @param bEffectAutoActivate true일 경우 ParticleEffect를 Spawn하자마다 ParticleEffect를 실행합니다. false일 경우 ParticleEffect를 실행하지 않습니다.
	 * @param bReset 처음부터 다시 재생할지 여부
	 * @return 지정한 Component에 부착하여 Spawn한 ParticleEffect입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	APRParticleEffect* SpawnParticleEffectAttached(UParticleSystem* SpawnEffect, USceneComponent* Parent, FName AttachSocketName, FVector Location, FRotator Rotation, FVector Scale = FVector(1.0f), bool bEffectAutoActivate = true, bool bReset = false);

	/**
	 * 주어진 ParticleSystem에 해당하는 활성화할 수 있는 ParticleEffect를 반환하는 함수입니다.
	 *
	 * @param ParticleSystem 활성화할 수 있는 ParticleEffect를 찾을 ParticleSystem입니다.
	 * @return 활성화할 수 있는 ParticleEffect입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	APRParticleEffect* GetActivateableParticleEffect(UParticleSystem* ParticleSystem);
	
	/**
	 * 주어진 ParticleEffect가 활성화되어 있는지 확인하는 함수입니다.
	 * 
	 * @param ParticleEffect 확인할 ParticleEffect입니다.
	 * @return ParticleEffect가 활성화되어 있으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleSystem")
	bool IsActivateParticleEffect(APRParticleEffect* ParticleEffect) const;

	/**
	 * 주어진 ParticleSystem에 해당하는 ParticlePool이 생성되어 있는지 확인하는 함수입니다.
	 * 
	 * @param ParticleSystem 확인할 ParticleSystem입니다.
	 * @return ParticlePool이 생성되어 있으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleSystem")
	bool IsCreateParticlePool(UParticleSystem* ParticleSystem) const;	

	/**
	 * 주어진 ParticleSystem에 해당하는 ActivateParticleIndexList가 생성되어 있는지 확인하는 함수입니다.
	 * 
	 * @param ParticleSystem 확인할 ParticleSystem입니다.
	 * @return ActivateParticleIndexList가 생성되어 있으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleSystem")
	bool IsCreateActivateParticleIndexList(UParticleSystem* ParticleSystem) const;

	/**
	 * 주어진 ParticleSystem에 해당하는 UsedParticleIndexList가 생성되어 있는지 확인하는 함수입니다.
	 * 
	 * @param ParticleSystem 확인할 ParticleSystem입니다.
	 * @return UsedParticleIndexList가 생성되어 있으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleSystem")
	bool IsCreateUsedParticleIndexList(UParticleSystem* ParticleSystem) const;

	/**
	 * 주어진 ParticleEffect가 동적으로 생성되었는지 확인하는 함수입니다.
	 * 
	 * @param ParticleEffect 확인할 ParticleEffect입니다.
	 * @return ParticleEffect가 동적으로 생성되었으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleSystem")
	bool IsDynamicParticleEffect(APRParticleEffect* ParticleEffect) const;

private:
	/**
	 * 주어진 ParticlePool을 제거하는 함수입니다.
	 * 
	 * @param TargetParticlePool 제거할 ParticlePool입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem|ParticleSystem")
	void ClearParticlePool(FPRParticleEffectObjectPool& TargetParticlePool);
	
	/**
	 * 주어진 ParticlePool의 설정 값을 바탕으로 ParticlePool을 생성하는 함수입니다.
	 *
	 * @param ParticlePoolSettings ParticlePool을 생성할 설정 값입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem|ParticleSystem")
	void CreateParticlePool(const FPRParticleEffectPoolSettings& ParticlePoolSettings);

	/**
	 * 주어진 ParticleSystem의 ActivateParticleIndexList를 생성하는 함수입니다.
	 *
	 * @param ParticleSystem ActivateParticleIndexList를 생성할 ParticleSystem입니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem|ParticleSystem")
	void CreateActivateParticleIndexList(UParticleSystem* ParticleSystem);

	/**
	 * 주어진 ParticleSystem의 UsedParticleIndexList를 생성하는 함수입니다.
	 *
	 * @param ParticleSystem UsedParticleIndexList를 생성할 ParticleSystem입니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem|ParticleSystem")
	void CreateUsedParticleIndexList(UParticleSystem* ParticleSystem);	

	/**
	 * 주어진 ParticleSystem을 월드에 PRParticleEffect로 Spawn하는 함수입니다.
	 *
	 * @param ParticleSystem 월드에 Spawn할 ParticleSystem입니다.
	 * @param PoolIndex 월드에 Spawn한 ParticleSystem이 ParticlePool에서 사용하는 Index 값입니다. 
	 * @param Lifespan PRParticleEffect의 수명입니다.
	 * @return 월드에 Spawn PRParticleEffect입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem|ParticleSystem")
	APRParticleEffect* SpawnParticleEffectInWorld(UParticleSystem* ParticleSystem, int32 PoolIndex, float Lifespan);

	/**
	 * 주어진 ParticleSystem을 월드에 동적으로 Spawn하는 함수입니다.
	 * 동적으로 Spawn한 ParticleSystem은 비활성화된 후 일정시간이 지나면 제거됩니다.
	 * 
	 * @param ParticleSystem 월드에 동적으로 Spawn할 ParticleSystem입니다.
	 * @return 월드에 동적으로 Spawn한 ParticleEffect입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	APRParticleEffect* SpawnDynamicParticleEffectInWorld(UParticleSystem* ParticleSystem);	

	/**
	 * 주어진 ParticleSystem에 해당하는 ParticleEffect를 초기화한 후 반환하는 함수입니다.
	 * 
	 * @param SpawnEffect 초기화할 ParticleEffect의 ParticleSystem입니다.
	 * @return 초기화된 ParticleEffect입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem|ParticleSystem")
	APRParticleEffect* InitializeParticleEffect(UParticleSystem* SpawnEffect);

	/**
	 * 주어진 동적으로 생성한 ParticleEffect를 제거하는 함수입니다.
	 * 
	 * @param TargetDynamicDestroyParticleEffectList 제거할 동적으로 생성한 ParticleEffect의 목록입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem|ParticleSystem")
	void ClearDynamicDestroyParticleList(FPRDynamicDestroyParticleEffectList& TargetDynamicDestroyParticleEffectList);

	/**
	 * 주어진 ParticleSystem에 해당하는 ParticleEffect의 설정 값을 데이터 테이블에서 가져오는 함수입니다.
	 *
	 * @param ParticleSystem 데이터 테이블에서 설정 값을 가져올 ParticleSystem입니다.
	 * @return 데이터 테이블에 해당하는 ParticleEffect의 설정 값이 있으면 반환합니다. 그렇지 않으면 기본 값을 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	FPRParticleEffectPoolSettings GetParticleEffectPoolSettingsFromDataTable(UParticleSystem* ParticleSystem) const; 

	/**
	 * 주어진 ParticleEffect가 비활성화될 때 실행하는 함수입니다.
	 *
	 * @param TargetEffect 비활성화되는 Effect입니다.
	 */
	UFUNCTION()
	void OnParticleEffectDeactivate(APREffect* TargetEffect);

	/**
	 * 주어진 동적으로 생성한 ParticleEffect가 비활성화될 때 실행하는 함수입니다.
	 *
	 * @param TargetEffect 비활성화되는 Effect입니다.
	 */
	UFUNCTION()
	void OnDynamicParticleEffectDeactivate(APREffect* TargetEffect);

	/**
	 * 동적으로 생성한 ParticleEffect를 제거하는 함수입니다.
	 *
	 * @param TargetParticleEffect 제거하는 동적으로 생성된 ParticleEffect입니다.
	 */
	UFUNCTION()
	void OnDynamicParticleEffectDestroy(APRParticleEffect* TargetParticleEffect);
	
private:
	/** ParticleObjectPool의 설정 값을 가진 데이터 테이블입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PREffectSystem|ParticleSystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> ParticlePoolSettingsDataTable;
	
	/** ParticleSystem ObjectPool입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|ParticleSystem", meta = (AllowPrivateAccess = "true"))
	FPRParticleEffectObjectPool ParticlePool;

	/** 활성화된 ParticleSystem의 Index 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|ParticleSystem", meta = (AllowPrivateAccess = "true"))
	FPRActivateParticleEffectIndexList ActivateParticleIndexList;

	/**
	 * 이전에 사용된 ParticleSystem들의 Index 목록입니다.
	 * 동적으로 생성하는 ParticleSystem의 Index에 오류가 생기지 않도록 합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|ParticleSystem", meta = (AllowPrivateAccess = "true"))
	FPRUsedParticleEffectIndexList UsedParticleIndexList;

	/** 동적으로 제거할 ParticleSystem의 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|ParticleSystem", meta = (AllowPrivateAccess = "true"))
	FPRDynamicDestroyParticleEffectList DynamicDestroyParticleList;
#pragma endregion 
};
