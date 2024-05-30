// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRBaseObjectPoolSystemComponent.generated.h"

#pragma region Struct
/**
 * 오브젝트를 클래스 이름으로 구분하여 보관하는 풀을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRPool
{
	GENERATED_BODY()

public:
	FPRPool()
		: PoolByName()
	{}

	FPRPool(const TMap<FName, FPRObjectArray>& NewPoolByName)
		: PoolByName(NewPoolByName)
	{}

public:
	/** 클래스 이름과 오브젝트 배열을 보관하는 맵입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool")
	TMap<FName, FPRObjectArray> PoolByName;
};

/**
 * 오브젝트들을 보관하는 풀을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRObjectPool
{
	GENERATED_BODY()

public:
	FPRObjectPool()
		: Pool()
	{}

	FPRObjectPool(const TMap<TSubclassOf<UObject>, FPRPool>& NewPool)
		: Pool(NewPool)
	{}

public:
	/** 풀에 넣을 오브젝트의 클래스 레퍼런스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectPool")
	TMap<TSubclassOf<UObject>, FPRPool> Pool;
};

/**
 * 활성화된 오브젝트들의 인덱스를 클래스 이름으로 구분하여 보관하는 목록을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRActivateObjectList
{
	GENERATED_BODY()

public:
	FPRActivateObjectList()
		: ActivateIndexesListByName()
	{}

	FPRActivateObjectList(const TMap<FName, FPRActivateIndexList>& NewActivateIndexesListByName)
		: ActivateIndexesListByName(NewActivateIndexesListByName)
	{}

public:
	/** 클래스 이름과 활성화된 인덱스를 보관한 맵입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActivateObjectList")
	TMap<FName, FPRActivateIndexList> ActivateIndexesListByName;
};

/**
 * 활성화된 오브젝트들의 인덱스를 보관한 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRActivateObjectIndexList
{
	GENERATED_BODY()

public:
	FPRActivateObjectIndexList()
		: List()
	{}

	FPRActivateObjectIndexList(const TMap<TSubclassOf<UObject>, FPRActivateObjectList>& NewList)
		: List(NewList)
	{}

public:
	/** 클래스 레퍼런스와 활성화된 인덱스를 보관한 맵입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AcitvateObjectIndexList")
	TMap<TSubclassOf<UObject>, FPRActivateObjectList> List;
};

/**
 * 이전에 사용된 오브젝트들의 인덱스를 클래스 이름으로 구분하여 보관하는 목록을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRUsedObjectList
{
	GENERATED_BODY()

public:
	FPRUsedObjectList()
		: UsedIndexesListByName()
	{}

	FPRUsedObjectList(const TMap<FName, FPRUsedIndexList>& NewUsedIndexesListByName)
		: UsedIndexesListByName(NewUsedIndexesListByName)
	{}

public:
	/** 클래스 이름과 이전에 사용된 인덱스를 보관한 맵입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UsedObjectList")
	TMap<FName, FPRUsedIndexList> UsedIndexesListByName;
};

/**
 * 이전에 사용된 오브젝트들의 인덱스를 보관한 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRUsedObjectIndexList
{
	GENERATED_BODY()

public:
	FPRUsedObjectIndexList()
		: List()
	{}

	FPRUsedObjectIndexList(const TMap<TSubclassOf<UObject>, FPRUsedIndexList>& NewList)
		: List(NewList)
	{}

public:
	/** 클래스 레퍼런스와 이전에 사용된 인덱스를 보관한 맵입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UsedObjectIndexList")
	TMap<TSubclassOf<UObject>, FPRUsedIndexList> List;
};

/**
 * 동적으로 생성한 오브젝트와 해당 오브젝트를 제거하는 수명을 관리하는 타이머 핸들을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRDynamicDestroyObject
{
	GENERATED_BODY()

public:
	FPRDynamicDestroyObject()
		: Timers()
	{}

	FPRDynamicDestroyObject(const TMap<TObjectPtr<UObject>, FTimerHandle>& NewTimers)
		: Timers(NewTimers)
	{}

public:
	/** 오브젝트와 해당 오브젝트를 제거하는 타이머 핸들을 보관하는 맵입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicDestroyObject")
	TMap<TObjectPtr<UObject>, FTimerHandle> Timers;
};

/**
 * 동적으로 생성한 오브젝트와 해당 오브젝트를 제거하는 수명을 관리하는 타이머 핸들을 보관한 풀을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRDynamicDestroyPool
{
	GENERATED_BODY()

public:
	FPRDynamicDestroyPool()
		: DestroyPoolByName()
	{}

	FPRDynamicDestroyPool(const TMap<FName, FPRDynamicDestroyObject>& NewDestroyPoolByName)
		: DestroyPoolByName(NewDestroyPoolByName)
	{}
	
public:
	/** 클래스 이름과 해당 오브젝트를 제거하는 수명을 관리하는 타이머 핸들을 보관한 맵입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicDestroyPool")
	TMap<FName, FPRDynamicDestroyObject> DestroyPoolByName;
};

/**
 * 동적으로 생성한 오브젝트와 해당 오브젝트를 제거하는 수명을 관리하는 타이머 핸들을 보관한 풀을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRDynamicDestroyObjectPool
{
	GENERATED_BODY()

public:
	FPRDynamicDestroyObjectPool()
		: DynamicDestroyPool()
	{}

	FPRDynamicDestroyObjectPool(const TMap<TSubclassOf<UObject>, FPRDynamicDestroyPool>& NewDynamicDestroyPool)
		: DynamicDestroyPool(NewDynamicDestroyPool)
	{}
	
public:
	/** 클래스 레퍼런스와 해당 오브젝트를 제거하는 수명을 관리하는 타이머 핸들을 보관한 맵입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicDestroyObjectPool")
	TMap<TSubclassOf<UObject>, FPRDynamicDestroyPool> DynamicDestroyPool;
};

/**
 * ObjectPool의 설정 값을 나타내는 구조체입니다. 
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

	FPRObjectPoolSettings(TSubclassOf<UObject> NewPooledObjectClass, int32 NewPoolSize)
		: PooledObjectClass(NewPooledObjectClass)
		, PoolSize(NewPoolSize)
	{}

public:
	/** 풀에 넣을 오브젝트의 클래스 레퍼런스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSettings")
	TSubclassOf<UObject> PooledObjectClass;

	/** 풀의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSettings")
	int32 PoolSize;	
};
#pragma endregion

/**
 * 오브젝트 풀을 관리하는 기본 ActorComponent 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRBaseObjectPoolSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRBaseObjectPoolSystemComponent();

public:
	virtual void DestroyComponent(bool bPromoteChildren) override;

public:
	/** 기존의 ObjectPool을 제거하고, 새로 ObjectPool을 생성하여 초기화하는 함수입니다. */
	UFUNCTION(Blueprintable, Category = "PRBaseObjectPoolSystem")
	virtual void InitializeObjectPool();

	/** ObjectPool을 제거하는 함수입니다. */
	UFUNCTION(Blueprintable, Category = "PRBaseObjectPoolSystem")
	virtual void ClearObjectPool();

protected:
	/**
	 * 인자로 받은 오브젝트 클래스에 해당하는 ObjectPool에서 비활성화된 오브젝트를 활성화하고 좌표와 회전 값을 적용하는 함수입니다.
	 * 인자로 받은 오브젝트 클래스에 해당하는 ObjectPool이 없을 경우, 동적으로 ObjectPool을 생성하고 비활성화된 오브젝트를 활성화하고 좌표와 회전 값을 적용합니다.
	 * 비활성화된 오브젝트가 없을 경우, 동적으로 오브젝트를 생성하여 활성화하고 좌표와 회전 값을 적용합니다.
	 *
	 * @param PooledObjectClass ObjectPool에서 찾을 오브젝트의 클래스
	 * @param NewLocation 적용할 오브젝트의 좌표
	 * @param NewRotation 적용할 오브젝틔 회전 값
	 * @return ObjectPool에서 찾아 활성화한 오브젝트
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	virtual UObject* ActivatePooledObjectFromClass(TSubclassOf<UObject> PooledObjectClass, FVector NewLocation = FVector::ZeroVector, FRotator NewRotation = FRotator::ZeroRotator);

	/**
	 * 인자로 받은 비활성화된 오브젝트가 ObjectPool에 존재하면 활성화하고 좌표와 회전 값을 적용하는 함수입니다.
	 * 인자로 받은 오브젝트가 이미 활성화된 상태일 경우 nullptr을 반환합니다.
	 *
	 * @param PooledObject 활성화할 비활성화된 오브젝트
	 * @param NewLocation 적용할 오브젝트의 좌표
	 * @param NewRotation 적용할 오브젝틔 회전 값
	 * @return 활성화한 오브젝트
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	virtual UObject* ActivatePooledObject(UObject* PooledObject, FVector NewLocation = FVector::ZeroVector, FRotator NewRotation = FRotator::ZeroRotator);
	
	/**
	 * 인자로 받은 오브젝트 클래스에 해당하는 ObjectPool에서 활성화할 수 있는 오브젝트를 반환하는 함수입니다.
	 * 인자로 받은 오브젝트 클래스에서 해당하는 ObjectPool이 없을 경우, 동적으로 ObjectPool을 생성하고 비활성화된 오브젝트를 반환합니다.
 	 * ObjectPool에서 활성화할 수 있는 오브젝트가 없을 경우, 동적으로 오브젝트를 생성하여 반환합니다.
	 *
	 * @param PooledObjectClass ObjectPool에서 찾을 오브젝트의 클래스
	 * @return 활성화할 수 있는 오브젝트
	 */	
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	virtual UObject* GetActivateablePooledObject(TSubclassOf<UObject> PooledObjectClass);

	/**
	 * 인자로 받은 오브젝트가 ObjectPool에서 활성화 되었는지 판별하는 함수입니다.
	 * 
	 * @param PooledObject 판별할 오브젝트
	 * @return 활성화되었으면 true, 활성화되지 않거나 ObjectPool이 존재하지 않을 경우 false를 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	virtual bool IsActivatePooledObject(UObject* PooledObject) const;

	/**
	 * 인자로 받은 오브젝트 클래스의 ObjectPool이 생성되었는지 판별하는 함수입니다. 
	 * 
	 * @param PooledObjectClass 판별할 오브젝트 클래스
	 * @return 생성되었으면 true, 생성되지 않았을 경우 false를 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	virtual bool IsCreateObjectPool(TSubclassOf<UObject> PooledObjectClass) const;

	/**
	 * 인자로 받은 오브젝트 클래스의 ActivateObjectIndexList가 생성되었는지 판별하는 함수입니다.
	 * 
	 * @param PooledObjectClass 판별할 오브젝트 클래스
	 * @return 생성되었으면 true, 생성되지 않았을 경우 false를 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	virtual bool IsCreateActivateObjectIndexList(TSubclassOf<UObject> PooledObjectClass) const;

	/**
	 * 인자로 받은 오브젝트 클래스의 UsedObjectIndexList가 생성되었는지 판별하는 함수입니다. 
	 * 
	 * @param PooledObjectClass 판별할 오브젝트 클래스
	 * @return 생성되었으면 true, 생성되지 않았을 경우 false를 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	virtual bool IsCreateUsedObjectIndexList(TSubclassOf<UObject> PooledObjectClass) const;
};
