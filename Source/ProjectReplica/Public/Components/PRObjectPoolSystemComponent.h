// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRObjectPoolSystemComponent.generated.h"

class APRPooledObject;

/**
 * 오브젝트 풀의 설정값을 나타내는 구조체입니다. 
 */
USTRUCT(Atomic, BlueprintType)
struct FPRObjectPoolSettings : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRObjectPoolSettings()
		: PooledObjectClass(nullptr)
		, PoolSize(0)
	{}

	FPRObjectPoolSettings(TSubclassOf<APRPooledObject> NewPooledObjectClass, int32 NewPoolSize)
		: PooledObjectClass(NewPooledObjectClass)
		, PoolSize(NewPoolSize)
	{}

public:
	/** 풀에 넣을 오브젝트의 클래스 레퍼런스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSettings")
	TSubclassOf<APRPooledObject> PooledObjectClass;

	/** 풀의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSettings")
	int32 PoolSize;	
};

/**
 * 오브젝트들을 넣은 오브젝트 풀을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRObjectPool
{
	GENERATED_BODY()

public:
	FPRObjectPool()
		: Objects()
	{}

	FPRObjectPool(const TArray<TObjectPtr<class APRPooledObject>>& NewObjects)
		: Objects(NewObjects)
	{}

public:
	/** 풀에 넣을 오브젝트의 클래스 레퍼런스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRObjectPool")
	TArray<TObjectPtr<class APRPooledObject>> Objects;
};

/**
 * 동적으로 생성한 오브젝트와 해당 오브젝트를 제거하는 수명을 관리하는 TimerHandle을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRDynamicDestroyObjectList
{
	GENERATED_BODY()
	
public:
	/** 동적으로 생성한 오브젝트와 해당 오브젝트를 제거하는 수명을 관리하는 TimerHandle을 보관한 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRDynamicDestroyObjectList")
	TMap<TObjectPtr<class APRPooledObject>, FTimerHandle> Objects;
};

/**
 * 오브젝트 풀을 관리하는 ActorComponent 클래스입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTREPLICA_API UPRObjectPoolSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRObjectPoolSystemComponent();

protected:
	virtual void DestroyComponent(bool bPromoteChildren) override;

private:
	/** 월드에 Spawn한 오브젝트의 풀입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<APRPooledObject>, FPRObjectPool> ObjectPool;

	/** 활성화된 오브젝트의 Index 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<APRPooledObject>, FPRActivateIndexList> ActivateObjectIndexList;

	/**
	 * 오브젝트를 PoolSize보다 많이 동적으로 생성할 때 사용한 오브젝트의 Index의 목록입니다.
	 * 동적으로 생성하는 오브젝트의 Index에 오류가 생기지 않도록 하기 위해서 사용합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<APRPooledObject>, FPRUsedIndexList> UsedObjectIndexList;

	/** 동적으로 제거해야할 오브젝트의 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<APRPooledObject>, FPRDynamicDestroyObjectList> DynamicDestroyObjectList;

	/**
	 * 동적으로 생성한 오브젝트를 제거하는 딜레이 시간입니다.
	 * 동적으로 생성한 오브젝트가 비활성화 되었을 때, 해당 시간이 지난 후 오브젝트를 제거합니다. 
	 */ 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	float DynamicDestroyDelay;
};
