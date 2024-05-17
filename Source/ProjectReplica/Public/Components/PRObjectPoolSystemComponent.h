// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRObjectPoolSystemComponent.generated.h"

class APRPooledObject;

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
 * 오브젝트들을 넣은 ObjectPool을 나타내는 구조체입니다.
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
	FPRDynamicDestroyObjectList()
		: Objects()
	{}

	FPRDynamicDestroyObjectList(const TMap<TObjectPtr<class APRPooledObject>, FTimerHandle>& NewObjects)
		: Objects(NewObjects)
	{}
	
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

public:
	/** ObjectPool을 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	void InitializeObjectPool();

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
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	APRPooledObject* ActivatePooledObjectFromClass(TSubclassOf<APRPooledObject> PooledObjectClass, FVector NewLocation = FVector::ZeroVector, FRotator NewRotation = FRotator::ZeroRotator);

	/**
	 * 인자로 받은 비활성화된 오브젝트가 ObjectPool에 존재하면 활성화하고 좌표와 회전 값을 적용하는 함수입니다.
	 * 인자로 받은 오브젝트가 이미 활성화된 상태일 경우 nullptr을 반환합니다.
	 * 
	 * @param PooledObject 활성화할 비활성화된 오브젝트
	 * @param NewLocation 적용할 오브젝트의 좌표
	 * @param NewRotation 적용할 오브젝틔 회전 값
	 * @return 활성화한 오브젝트
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	APRPooledObject* ActivatePooledObject(APRPooledObject* PooledObject, FVector NewLocation = FVector::ZeroVector, FRotator NewRotation = FRotator::ZeroRotator);
	
	/**
	 * 인자로 받은 오브젝트 클래스에 해당하는 ObjectPool에서 활성화할 수 있는 오브젝트를 반환하는 함수입니다.
	 * 인자로 받은 오브젝트 클래스에서 해당하는 ObjectPool이 없을 경우, 동적으로 ObjectPool을 생성하고 비활성화된 오브젝트를 반환합니다.
 	 * ObjectPool에서 활성화할 수 있는 오브젝트가 없을 경우, 동적으로 오브젝트를 생성하여 반환합니다.
	 *
	 * @param PooledObjectClass ObjectPool에서 찾을 오브젝트의 클래스
	 * @return 활성화할 수 있는 오브젝트
	 */	
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	APRPooledObject* GetActivateablePooledObject(TSubclassOf<APRPooledObject> PooledObjectClass);

	/**
	 * 인자로 받은 오브젝트가 ObjectPool에서 활성화 되었는지 판별하는 함수입니다.
	 * 
	 * @param PooledObject 판별할 오브젝트
	 * @return 활성화되었으면 true, 활성화되지 않거나 ObjectPool이 존재하지 않을 경우 false를 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	bool IsActivatePooledObject(APRPooledObject* PooledObject) const;

	/**
	 * 인자로 받은 오브젝트 클래스의 ObjectPool이 생성되었는지 판별하는 함수입니다. 
	 * 
	 * @param PooledObjectClass 판별할 오브젝트 클래스
	 * @return 생성되었으면 true, 생성되지 않았을 경우 false를 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	bool IsCreateObjectPool(TSubclassOf<APRPooledObject> PooledObjectClass) const;

	/**
	 * 인자로 받은 오브젝트 클래스의 ActivateObjectIndexList가 생성되었는지 판별하는 함수입니다.
	 * 
	 * @param PooledObjectClass 판별할 오브젝트 클래스
	 * @return 생성되었으면 true, 생성되지 않았을 경우 false를 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	bool IsCreateActivateObjectIndexList(TSubclassOf<APRPooledObject> PooledObjectClass) const;

	/**
	 * 인자로 받은 오브젝트 클래스의 UsedObjectIndexList가 생성되었는지 판별하는 함수입니다. 
	 * 
	 * @param PooledObjectClass 판별할 오브젝트 클래스
	 * @return 생성되었으면 true, 생성되지 않았을 경우 false를 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	bool IsCreateUsedObjectIndexList(TSubclassOf<APRPooledObject> PooledObjectClass) const;
	
private:
	/**
	 * 인자로 받은 클래스의 오브젝트를 월드에 Spawn하는 함수입니다.
	 * 
	 * @param ObjectClass 월드에 Spawn할 오브젝트의 클래스
	 * @return 월드에 Spawn한 오브젝트
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	APRPooledObject* SpawnObjectInWorld(TSubclassOf<APRPooledObject> ObjectClass);

	/**
	 * 
	 * @param ObjectClass 월드에 Spawn할 오브젝트의 클래스
	 * @param Index 월드에 Spawn한 오브젝트의 ObjectPool에서 사용하는 Index 값
	 * @return 월드에 Spawn한 오브젝트
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	APRPooledObject* SpawnAndInitializeObject(TSubclassOf<APRPooledObject> ObjectClass, int32 Index = 0);

	/**
	 * 사용 가능한 Index를 찾아 반환하는 함수입니다.
	 * 
	 * @param UsedIndexes 이미 사용 중인 Index 목록
	 * @return 사용 가능한 Index
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	int32 FindAvailableIndex(const TSet<int32>& UsedIndexes);
	
	/** ObjectPool을 비우는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	void ClearObjectPool();

	/** 인자로 받은 ObjectPool의 설정 값을 바탕으로 ObjectPool을 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	void CreateObjectPool(FPRObjectPoolSettings ObjectPoolSettings);

	/** 인자로 받은 오브젝트 클래스의 ActivateObjectIndexList를 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	void CreateActivateObjectIndexList(TSubclassOf<APRPooledObject> ObjectClass);	

	/**
	 * 인자로 받은 오브젝트가 비활성화될 때 실행하는 함수입니다.
	 * 비활성화된 오브젝트의 Index를 ActivateObjectIndexList에서 제거합니다.
	 * 
	 * @param PooledObject 비활성화된 오브젝트
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	void OnPooledObjectDeactivate(APRPooledObject* PooledObject);

	/**
	 * 인자로 받은 동적으로 생성한 오브젝트가 비활성화될 때 실행하는 함수입니다.
	 * 
	 * @param PooledObject 비활성화된 동적으로 생성한 오브젝트
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	void OnDynamicObjectDeactivate(APRPooledObject* PooledObject);

	/**
	 * 인자로 받은 동적으로 생성한 오브젝트를 제거하는 함수입니다.
	 * 
	 * @param PooledObject 제거할 오브젝트
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	void OnDynamicObjectDestroy(APRPooledObject* PooledObject);
	
protected:
	/** ObjectPool의 설정 값을 가진 데이터 테이블입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRObjectPoolSystem")
	TObjectPtr<UDataTable> ObjectPoolSettingsDataTable;
	
	/** 월드에 Spawn한 오브젝트의 풀입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem")
	TMap<TSubclassOf<APRPooledObject>, FPRObjectPool> ObjectPool;

	/** 활성화된 오브젝트의 Index 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem")
	TMap<TSubclassOf<APRPooledObject>, FPRActivateIndexList> ActivateObjectIndexList;

	/**
	 * 오브젝트를 PoolSize보다 많이 동적으로 생성할 때 사용한 오브젝트의 Index의 목록입니다.
	 * 동적으로 생성하는 오브젝트의 Index에 오류가 생기지 않도록 하기 위해서 사용합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem")
	TMap<TSubclassOf<APRPooledObject>, FPRUsedIndexList> UsedObjectIndexList;

	/** 동적으로 제거해야할 오브젝트의 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem")
	TMap<TSubclassOf<APRPooledObject>, FPRDynamicDestroyObjectList> DynamicDestroyObjectList;

	/**
	 * 동적으로 생성한 오브젝트를 제거하는 딜레이 시간입니다.
	 * 동적으로 생성한 오브젝트가 비활성화 되었을 때, 해당 시간이 지난 후 오브젝트를 제거합니다. 
	 */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem")
	float DynamicDestroyDelay;

	/** 동적으로 생성하는 ObjectPool의 PoolSize입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem", meta = (ClampMin = "1"))
	int32 DynamicPoolSize;
};
