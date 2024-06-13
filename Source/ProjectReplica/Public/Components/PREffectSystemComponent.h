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

#pragma region BeforeStruct
// /**
//  * NiagaraEffectPool의 설정 값을 나타내는 구조체입니다.
//  */
// USTRUCT(Atomic, BlueprintType)
// struct FPRNiagaraEffectSettings : public FTableRowBase
// {
// 	GENERATED_BODY()
//
// public:
// 	FPRNiagaraEffectSettings()
// 		: NiagaraSystem(nullptr)
// 		, PoolSize(0)
// 		, Lifespan(0.0f)
// 	{}
//
// 	FPRNiagaraEffectSettings(TObjectPtr<UNiagaraSystem> NewNiagaraSystem, int32 NewPoolSize, float NewLifespan)
// 		: NiagaraSystem(NewNiagaraSystem)
// 		, PoolSize(NewPoolSize)
// 		, Lifespan(NewLifespan)
// 	{}
//
// public:
// 	/** 생성할 NiagaraSystem입니다. */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRNiagaraEffectSettings")
// 	TObjectPtr<UNiagaraSystem> NiagaraSystem;
//
// 	/** Pool에 넣을 초기에 생성할 NiagaraEffect의 수입니다. */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRNiagaraEffectSettings")
// 	int32 PoolSize;
//
// 	/** 이펙트의 수명입니다. */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRNiagaraEffectSettings")
// 	float Lifespan;
//
// public:
// 	/**
// 	 * 인자로 받은 NiagaraEffectSettings와 같은지 판별하는 ==연산자 오버로딩입니다.
// 	 * 
// 	 * @param NewNiagaraEffectSettings 비교하는 NiagaraEffectSettings와 같은지 판별할 NiagaraEffectSettings입니다.
// 	 * @return 인자로 받은 NiagaraEffectSettings와 같을 경우 true를 다를 경우 false를 반환합니다.
// 	 */
// 	FORCEINLINE bool operator==(const FPRNiagaraEffectSettings& NewNiagaraEffectSettings) const
// 	{
// 		return this->NiagaraSystem == NewNiagaraEffectSettings.NiagaraSystem
// 				&& this->PoolSize == NewNiagaraEffectSettings.PoolSize
// 				&& this->Lifespan == NewNiagaraEffectSettings.Lifespan;
// 	}
//
// 	/**
// 	 * 인자로 받은 NiagaraEffectSettings와 다른지 판별하는 !=연산자 오버로딩입니다.
// 	 * 
// 	 * @param NewNiagaraEffectSettings 비교하는 NiagaraEffectSettings와 같은지 판별할 NiagaraEffectSettings입니다.
// 	 * @return 인자로 받은 NiagaraEffectSettings와 다를 경우 true를 같을 경우 false를 반환합니다.
// 	 */
// 	FORCEINLINE bool operator!=(const FPRNiagaraEffectSettings& NewNiagaraEffectSettings) const
// 	{
// 		return this->NiagaraSystem != NewNiagaraEffectSettings.NiagaraSystem
// 				&& this->PoolSize != NewNiagaraEffectSettings.PoolSize
// 				&& this->Lifespan != NewNiagaraEffectSettings.Lifespan;
// 	}
// };
//
// // /**
// //  * NiagaraEffect의 풀입니다.
// //  */
// // USTRUCT(Atomic, BlueprintType)
// // struct FPRNiagaraEffectPool
// // {
// // 	GENERATED_BODY()
// //
// // public:
// // 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRNiagaraEffectPool")
// // 	TArray<TObjectPtr<APRNiagaraEffect>> Effects;
// // };
//
// /**
//  * 동적으로 생성한 NiagaraEffect 및 해당 NiagaraEffect를 제거할 때 사용하는 TimerHandle을 저장한 구조체입니다. 
//  */
// USTRUCT(Atomic, BlueprintType)
// struct FPRDynamicNiagaraEffectPool
// {
// 	GENERATED_BODY()
//
// public:
// 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DynamicNiagaraEffectPool")
// 	TMap<TObjectPtr<APRNiagaraEffect>, FTimerHandle> Effects;
// };
//
// /**
//  * ParticleEffectPool의 설정 값을 나타내는 구조체입니다.
//  */
// USTRUCT(Atomic, BlueprintType)
// struct FPRParticleEffectSettings : public FTableRowBase
// {
// 	GENERATED_BODY()
//
// public:
// 	FPRParticleEffectSettings()
// 		: ParticleSystem(nullptr)
// 		, PoolSize(0)
// 		, Lifespan(0.0f)
// 	{}
//
// 	FPRParticleEffectSettings(TObjectPtr<UParticleSystem> NewParticleSystem, int32 NewPoolSize, float NewLifespan)
// 		: ParticleSystem(NewParticleSystem)
// 		, PoolSize(NewPoolSize)
// 		, Lifespan(NewLifespan)
// 	{}
//
// public:
// 	/** 생성할 ParticleSystem입니다. */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRParticleEffectSettings")
// 	TObjectPtr<UParticleSystem> ParticleSystem;
//
// 	/** Pool에 넣을 초기에 생성할 ParticleEffect의 수입니다. */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRParticleEffectSettings")
// 	int32 PoolSize;
//
// 	/** 이펙트의 수명입니다. */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRParticleEffectSettings")
// 	float Lifespan;
//
// public:
// 	/**
// 	 * 인자로 받은 ParticleEffectSettings와 같은지 판별하는 ==연산자 오버로딩입니다.
// 	 * 
// 	 * @param NewParticleEffectSettings 비교하는 ParticleEffectSettings와 같은지 판별할 ParticleEffectSettings입니다.
// 	 * @return 인자로 받은 ParticleEffectSettings와 같을 경우 true를 다를 경우 false를 반환합니다.
// 	 */
// 	FORCEINLINE bool operator==(const FPRParticleEffectSettings& NewParticleEffectSettings) const
// 	{
// 		return this->ParticleSystem == NewParticleEffectSettings.ParticleSystem
// 				&& this->PoolSize == NewParticleEffectSettings.PoolSize
// 				&& this->Lifespan == NewParticleEffectSettings.Lifespan;
// 	}
//
// 	/**
// 	 * 인자로 받은 ParticleEffectSettings와 다른지 판별하는 !=연산자 오버로딩입니다.
// 	 * 
// 	 * @param NewParticleEffectSettings 비교하는 ParticleEffectSettings와 같은지 판별할 ParticleEffectSettings입니다.
// 	 * @return 인자로 받은 ParticleEffectSettings와 다를 경우 true를 같을 경우 false를 반환합니다.
// 	 */
// 	FORCEINLINE bool operator!=(const FPRParticleEffectSettings& NewParticleEffectSettings) const
// 	{
// 		return this->ParticleSystem != NewParticleEffectSettings.ParticleSystem
// 				&& this->PoolSize != NewParticleEffectSettings.PoolSize
// 				&& this->Lifespan != NewParticleEffectSettings.Lifespan;
// 	}
// };
//
// /**
//  * ParticleEffect의 풀입니다.
//  */
// // USTRUCT(Atomic, BlueprintType)
// // struct FPRParticleEffectPool
// // {
// // 	GENERATED_BODY()
// //
// // public:
// // 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRParticleEffectPool")
// // 	TArray<TObjectPtr<APRParticleEffect>> Effects;
// // };
//
// /**
//  * 동적으로 생성한 ParticleEffect 및 해당 ParticleEffect를 제거할 때 사용하는 TimerHandle을 저장한 구조체입니다. 
//  */
// USTRUCT(Atomic, BlueprintType)
// struct FPRDynamicParticleEffectPool
// {
// 	GENERATED_BODY()
//
// public:
// 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DynamicParticleEffectPool")
// 	TMap<TObjectPtr<APRParticleEffect>, FTimerHandle> Effects;
// };
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
	APRNiagaraEffect* SpawnNiagaraEffectAttached(UNiagaraSystem* SpawnEffect, USceneComponent* Parent, FName AttachSocketName, FVector Location, FRotator Rotation, FVector Scale, bool bEffectAutoActivate = true, bool bReset = false);

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

	/** 인자로 받은 NiagaraEffect가 비활성화될 때 실행하는 함수입니다. */
	UFUNCTION()
	void OnNiagaraEffectDeactivate(APREffect* Effect);

	/** 동적으로 생성한 NiagaraEffect가 비활성화될 때 실행하는 함수입니다. */
	UFUNCTION()
	void OnDynamicNiagaraEffectDeactivate(APREffect* Effect);

	/** 동적으로 생성한 NiagaraEffect를 제거하는 함수입니다. */
	UFUNCTION()
	void DynamicNiagaraEffectDestroy(APRNiagaraEffect* NiagaraEffect);
	
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
	void InitializeParticlePool();

	void ClearAllParticlePool();
	
private:
	/** ParticlePool의 설정 값을 가진 데이터 테이블입니다. */
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














	


#pragma region NiagaraEffect
public:
	/**
	 * 모든 NiagaraEffectPool, DynamicNiagaraEffectPool,
	 * ActivateNiagaraEffectIndexList, UsedNiagaraEffectIndexList를 비우는 함수입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraEffect")
	void EmptyAllNiagaraEffectPool();

	/** 인자로 받은 NiagaraEffect가 활성화되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraEffect")
	bool IsActivateNiagaraEffect(APRNiagaraEffect* NiagaraEffect) const;

	/** 인자로 받은 NiagaraEffect의 Pool이 생성되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraEffect")
	bool IsCreateNiagaraEffectPool(UNiagaraSystem* NiagaraSystem) const;
	
private:
	
	/** 인자로 받은 NiagaraEffectSettings를 기반으로 NiagaraEffectPool을 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraEffect")
	void CreateNiagaraEffectPool(FPRNiagaraEffectSettings NiagaraEffectSettings);

	/** 인자로 받은 NiagaraSystem의 ActivateNiagaraEffectIndexList를 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraEffect")
	void CreateActivateNiagaraEffectIndexList(UNiagaraSystem* NiagaraSystem);

	/** 인자로 받은 NiagaraSystem의 UsedNiagaraEffectIndexList를 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraEffect")
	void CreateUsedNiagaraEffectIndexList(UNiagaraSystem* NiagaraSystem);
	
	/** 인자로 받은 NiagaraSystem를 월드에 APRNiagaraEffect로 Spawn하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraEffect")
	APRNiagaraEffect* SpawnNiagaraEffectInWorld(UNiagaraSystem* NiagaraSystem, int32 PoolIndex = -1, float Lifespan = 0.0f);

	/** 인자로 받은 NiagaraEffect의 설정 값을 데이터 테이블에서 가져오는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraEffect")
	FPRNiagaraEffectSettings GetNiagaraEffectSettingsFromDataTable(UNiagaraSystem* NiagaraSystem) const; 
	
private:
	/** NiagaraEffectPool의 설정 값을 가진 데이터 테이블입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PREffectSystem|NiagaraEffect", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> NiagaraEffectSettingsDataTable;	
	
	/** 월드에 Spawn한 NiagaraEffect의 Pool입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|NiagaraEffect", meta = (AllowPrivateAccess = "true"))
	TMap<TObjectPtr<UNiagaraSystem>, FPRNiagaraEffectPool> NiagaraEffectPool;

	/** 월드에 동적 Spawn한 NiagaraEffect의 Pool입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|NiagaraEffect", meta = (AllowPrivateAccess = "true"))
	TMap<TObjectPtr<UNiagaraSystem>, FPRDynamicNiagaraEffectPool> DynamicNiagaraEffectPool;

	/** 활성화된 NiagaraEffect의 Index 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|NiagaraEffect", meta = (AllowPrivateAccess = "true"))
	TMap<TObjectPtr<UNiagaraSystem>, FPRActivateIndexList> ActivateNiagaraEffectIndexList;

	/**
	 * 동적으로 생성된 NiagaraEffect의 Index를 추적하기 위한 Index 목록입니다.
	 * 동적으로 생성한 NiagaraEffect의 Index를 부여할 때 해당 Index를 여기에 저장합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|NiagaraEffect", meta = (AllowPrivateAccess = "true"))
	TMap<TObjectPtr<UNiagaraSystem>, FPRUsedIndexList> UsedNiagaraEffectIndexList;
#pragma endregion 
	
#pragma region ParticleEffect
public:
	/**
	 * 모든 ParticleEffectPool, DynamicParticleEffectPool,
	 * ActivateParticleEffectIndexList, UsedParticleEffectIndexList를 비우는 함수입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	void EmptyAllParticleEffectPool();

	/** 인자로 받은 ParticleEffect가 활성화되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	bool IsActivateParticleEffect(APRParticleEffect* ParticleEffect) const;

	/** 인자로 받은 ParticleEffect의 Pool이 생성되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	bool IsCreateParticleEffectPool(UParticleSystem* ParticleSystem) const;

	/** 인자로 받은 ParticleEffect가 동적으로 생성한 ParticleEffect인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	bool IsDynamicParticleEffect(APRParticleEffect* ParticleEffect) const;

	/**
	 * 이펙트를 지정한 위치에 Spawn하는 함수입니다.
	 *
	 * @param SpawnEffect Spawn할 이펙트
	 * @param Location 이펙트를 생성할 위치
	 * @param Rotation 이펙트에 적용한 회전 값
	 * @param Scale 이펙트에 적용할 크기
	 * @param bEffectAutoActivate true일 경우 이펙트를 Spawn하자마다 이펙트를 실행합니다. false일 경우 이펙트를 실행하지 않습니다.
	 * @param bReset 처음부터 다시 재생할지 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	APRParticleEffect* SpawnParticleEffectAtLocation(UParticleSystem* SpawnEffect, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.0f), bool bEffectAutoActivate = true, bool bReset = false);

	
	/**
	 * 이펙트를 지정한 Component에 부착하여 Spawn하는 함수입니다.
	 *
	 * @param SpawnEffect Spawn할 이펙트
	 * @param Parent 이펙트를 부착할 Component
	 * @param AttachSocketName 부착할 소켓의 이름
	 * @param Location 이펙트를 생성할 위치
	 * @param Rotation 이펙트에 적용한 회전 값
	 * @param Scale 이펙트에 적용할 크기
	 * @param bEffectAutoActivate true일 경우 이펙트를 Spawn하자마다 이펙트를 실행합니다. false일 경우 이펙트를 실행하지 않습니다.
	 * @param bReset 처음부터 다시 재생할지 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	APRParticleEffect* SpawnParticleEffectAttached(UParticleSystem* SpawnEffect, USceneComponent* Parent, FName AttachSocketName, FVector Location, FRotator Rotation, FVector Scale, bool bEffectAutoActivate = true, bool bReset = false);

private:
	/** 인자에 해당하는 활성화할 수 있는 ParticleSystem을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	APRParticleEffect* GetActivateableParticleEffect(UParticleSystem* ParticleSystem);
	
	/** 인자로 받은 ParticleEffectSettings를 기반으로 ParticleEffectPool을 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	void CreateParticleEffectPool(FPRParticleEffectSettings ParticleEffectSettings);

	/** 인자로 받은 ParticleSystem의 ActivateParticleEffectIndexList를 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	void CreateActivateParticleEffectIndexList(UParticleSystem* ParticleSystem);

	/** 인자로 받은 ParticleSystem의 UsedParticleEffectIndexList를 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	void CreateUsedParticleEffectIndexList(UParticleSystem* ParticleSystem);
	
	/** 인자로 받은 ParticleSystem를 월드에 APRParticleEffect로 Spawn하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	APRParticleEffect* SpawnParticleEffectInWorld(UParticleSystem* ParticleSystem, int32 PoolIndex = -1, float Lifespan = 0.0f);
	
	/** 인자로 받은 ParticleSystem를 월드에 APRParticleEffect로 동적 Spawn하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|NiagaraEffect")
	APRParticleEffect* SpawnDynamicParticleEffectInWorld(UParticleSystem* ParticleSystem);	

	/** 인자로 받은 ParticleEffect가 비활성화될 때 실행하는 함수입니다. */
	UFUNCTION()
	void OnParticleEffectDeactivate(APREffect* Effect);

	/** 동적으로 생성한 ParticleEffect가 비활성화될 때 실행하는 함수입니다. */
	UFUNCTION()
	void OnDynamicParticleEffectDeactivate(APREffect* Effect);

	/** 동적으로 생성한 ParticleEffect를 제거하는 함수입니다. */
	UFUNCTION()
	void DynamicParticleEffectDestroy(APRParticleEffect* ParticleEffect);

	/** 인자로 받은 ParticleEffect의 설정 값을 데이터 테이블에서 가져오는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffectSystem|ParticleEffect")
	FPRParticleEffectSettings GetParticleEffectSettingsFromDataTable(UParticleSystem* ParticleSystem) const; 
	
private:
	/** ParticleEffectPool의 설정 값을 가진 데이터 테이블입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PREffectSystem|ParticleEffect", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> ParticleEffectSettingsDataTable;	
	
	/** 월드에 Spawn한 ParticleEffect의 Pool입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|ParticleEffect", meta = (AllowPrivateAccess = "true"))
	TMap<TObjectPtr<UParticleSystem>, FPRParticleEffectPool> ParticleEffectPool;

	/** 월드에 동적 Spawn한 ParticleEffect의 Pool입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|ParticleEffect", meta = (AllowPrivateAccess = "true"))
	TMap<TObjectPtr<UParticleSystem>, FPRDynamicParticleEffectPool> DynamicParticleEffectPool;

	/** 활성화된 ParticleEffect의 Index 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|ParticleEffect", meta = (AllowPrivateAccess = "true"))
	TMap<TObjectPtr<UParticleSystem>, FPRActivateIndexList> ActivateParticleEffectIndexList;

	/**
	 * 동적으로 생성된 ParticleEffect의 Index를 추적하기 위한 Index 목록입니다.
	 * 동적으로 생성한 ParticleEffect의 Index를 부여할 때 해당 Index를 여기에 저장합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffectSystem|ParticleEffect", meta = (AllowPrivateAccess = "true"))
	TMap<TObjectPtr<UParticleSystem>, FPRUsedIndexList> UsedParticleEffectIndexList;
#pragma endregion 
};
