// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "PRBaseObjectPoolSystemComponent.h"
#include "PRObjectPoolSystemComponent.generated.h"

class APRPooledObject;

#pragma region Struct
/**
 * 오브젝트를 보관하는 풀을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRPool
{
	GENERATED_BODY()

public:
	FPRPool()
		: PooledObjects()
	{}

	FPRPool(const TArray<TObjectPtr<APRPooledObject>>& NewPooledObjects)
		: PooledObjects(NewPooledObjects)
	{}

public:
	/** 풀에 보관된 오브젝트들의 Array입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRPool")
	TArray<TObjectPtr<APRPooledObject>> PooledObjects;
};

/**
 * 여러 오브젝트 풀을 보관하는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRObjectPool
{
	GENERATED_BODY()

public:
	FPRObjectPool()
		: Pool()
	{}

	FPRObjectPool(const TMap<TSubclassOf<APRPooledObject>, FPRPool>& NewPool)
		: Pool(NewPool)
	{}

public:
	/** 오브젝트 클래스와 해당 풀의 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRObjectPool")
	TMap<TSubclassOf<APRPooledObject>, FPRPool> Pool;
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

	FPRObjectPoolSettings(TSubclassOf<APRPooledObject> NewPooledObjectClass, int32 NewPoolSize)
		: PooledObjectClass(NewPooledObjectClass)
		, PoolSize(NewPoolSize)
	{}

public:
	/** Pool에 넣을 오브젝트의 클래스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSettings")
	TSubclassOf<APRPooledObject> PooledObjectClass;

	/** Pool의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSettings")
	int32 PoolSize;	
};
#pragma endregion

/**
 * ObjectPool을 관리하는 ActorComponent 클래스입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTREPLICA_API UPRObjectPoolSystemComponent : public UPRBaseObjectPoolSystemComponent
{
	GENERATED_BODY()

public:
	UPRObjectPoolSystemComponent();

#pragma region PRBaseObjectPoolSystem
public:
	/** 기존의 ObjectPool을 제거하고, 새로 ObjectPool을 생성하여 초기화하는 함수입니다. */
	virtual void InitializeObjectPool() override;

	/** 모든 ObjectPool을 제거하는 함수입니다. */
	virtual void ClearAllObjectPool() override;
#pragma endregion

public:
	/**
	 * 주어진 오브젝트 클래스에 해당하는 ObjectPool에서 비활성화된 오브젝트를 좌표와 회전 값을 적용한후 활성화하는 함수입니다.
	 * 주어진 오브젝트 클래스에 해당하는 ObjectPool이 없을 경우, 동적으로 ObjectPool을 생성하고 비활성화된 오브젝트를 활성화하고 좌표와 회전 값을 적용합니다.
	 * 비활성화된 오브젝트가 없을 경우, 동적으로 오브젝트를 생성하여 활성화하고 좌표와 회전 값을 적용합니다.
	 *
	 * @param PooledObjectClass ObjectPool에서 찾을 오브젝트의 클래스입니다.
	 * @param NewLocation 적용할 오브젝트의 좌표입니다.
	 * @param NewRotation 적용할 오브젝틔 회전 값입니다.
	 * @return ObjectPool에서 찾아 활성화한 오브젝트를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	APRPooledObject* ActivatePooledObjectFromClass(TSubclassOf<APRPooledObject> PooledObjectClass, FVector NewLocation = FVector::ZeroVector, FRotator NewRotation = FRotator::ZeroRotator);

	/**
	 * 주어진 비활성화된 오브젝트가 ObjectPool에 존재하면 활성화하고 좌표와 회전 값을 적용하는 함수입니다.
	 * 주어진 오브젝트가 이미 활성화된 상태일 경우 nullptr을 반환합니다.
	 *
	 * @param PooledObject 활성화할 비활성화된 오브젝트입니다.
	 * @param NewLocation 적용할 오브젝트의 좌표입니다.
	 * @param NewRotation 적용할 오브젝틔 회전 값입니다.
	 * @return 활성화한 오브젝트 오브젝트를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	APRPooledObject* ActivatePooledObject(APRPooledObject* PooledObject, FVector NewLocation = FVector::ZeroVector, FRotator NewRotation = FRotator::ZeroRotator);
	
	/**
	 * 주어진 오브젝트 클래스에 해당하는 ObjectPool에서 활성화할 수 있는 오브젝트를 반환하는 함수입니다.
	 * 주어진 오브젝트 클래스에서 해당하는 ObjectPool이 없을 경우, 동적으로 ObjectPool을 생성하고 비활성화된 오브젝트를 반환합니다.
	  * ObjectPool에서 활성화할 수 있는 오브젝트가 없을 경우, 동적으로 오브젝트를 생성하여 반환합니다.
	 *
	 * @param PooledObjectClass ObjectPool에서 찾을 오브젝트의 클래스입니다.
	 * @return 활성화할 수 있는 오브젝트를 반환합니다.
	 */	
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	APRPooledObject* GetActivateablePooledObject(TSubclassOf<APRPooledObject> PooledObjectClass);

	/**
	 * 주어진 객체가 활성화되어 있는지 확인하는 함수입니다.
	 * 
	 * @param PooledObject 확인할 객체입니다.
	 * @return 객체가 유효하고, 풀링 가능하며, 현재 활성화된 경우 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	bool IsActivatePooledObject(APRPooledObject* PooledObject) const;

	/**
	 * 주어진 오브젝트 클래스의 ObjectPool이 생성되었는지 확인하는 함수입니다. 
	 * @param PooledObjectClass 확인할 오브젝트 클래스입니다.
	 * @return ObjectPool이 생성되었으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	bool IsCreateObjectPool(TSubclassOf<APRPooledObject> PooledObjectClass) const;

	/**
	 * 주어진 오브젝트 클래스의 ActivateObjectIndexList가 생성되어 있는지 확인하는 함수입니다.
	 * 
	 * @param PooledObjectClass 확인할 오브젝트 클래스입니다.
	 * @return ActivateObjectIndexList가 생성되었으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	virtual bool IsCreateActivateObjectIndexList(TSubclassOf<APRPooledObject> PooledObjectClass) const;

	/**
	 * 주어진 오브젝트 클래스의 UsedObjectIndexList가 생성되어 있는지 확인하는 함수입니다. 
	 * 
	 * @param PooledObjectClass 확인할 오브젝트 클래스입니다.
	 * @return UsedObjectIndexList가 생성되었으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	bool IsCreateUsedObjectIndexList(TSubclassOf<APRPooledObject> PooledObjectClass) const;

	/**
	 * 주어진 오브젝트가 동적으로 생성된 오브젝트인지 확인하는 함수입니다.
	 *
	 * @param PooledObject 확인할 오브젝트입니다.
	 * @return PooledObject가 동적으로 생성되었다면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(Blueprintable, Category = "PRObjectPoolSystem")
	bool IsDynamicPooledObject(APRPooledObject* PooledObject) const;

private:
	/**
	 * 주어진 ObjectPool을 제거하는 함수입니다.
	 *
	 * @param NewObjectPool 제거할 ObjectPool입니다.
	 */
	UFUNCTION(Blueprintable, Category = "PRObjectPoolSystem")
	void ClearObjectPool(FPRObjectPool& NewObjectPool);
	
	/**
	 * 주어진 오브젝트 클래스를 월드에 Spawn하는 함수입니다.
	 * 
	 * @param ObjectClass 월드에 Spawn할 오브젝트의 클래스입니다.
	 * @return 월드가 유효하고 주어진 오브젝트 클래스가 PRPoolableInterface를 구현하는 경우 월드에 Spawn한 오브젝트를 반환합니다. 그렇지 않으면 nullptr을 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	APRPooledObject* SpawnObjectInWorld(TSubclassOf<APRPooledObject> ObjectClass);

	/**
	 * 주어진 오브젝트 클래스를 월드에 동적으로 Spawn하는 함수입니다.
	 * 동적으로 Spawn한 오브젝트는 비활성화된 후 일정시간이 지나면 제거됩니다.
	 * 
	 * @param ObjectClass 월드에 동적으로 Spawn할 오브젝트의 클래스입니다.
	 * @return 월드가 유효하고 주어진 오브젝트 클래스가 PoolableInterface를 구현하는 경우 월드에 동적으로 Spawn한 오브젝트를 반환합니다. 그렇지 않으면 nullptr을 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	APRPooledObject* SpawnDynamicObjectInWorld(TSubclassOf<APRPooledObject> ObjectClass);
	
	/**
	 * 주어진 오브젝트 클래스를 바탕으로 Object를 월드에 Spawn한 후 초기화하는 함수입니다.
	 * 
	 * @param ObjectClass 월드에 Spawn할 오브젝트의 클래스입니다.
	 * @param Index 월드에 Spawn한 오브젝트가 ObjectPool에서 사용하는 Index 값입니다.
	 * @return 월드에 Spawn한 후 초기화한 오브젝트입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	APRPooledObject* SpawnAndInitializeObject(TSubclassOf<APRPooledObject> ObjectClass, int32 Index = 0);

	/**
	 * 주어진 ObjectPool의 설정 값을 바탕으로 ObjectPool을 생성하는 함수입니다.
	 *
	 * @param ObjectPoolSettings ObjectPool을 생성할 설정 값입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	void CreateObjectPool(const FPRObjectPoolSettings& ObjectPoolSettings);

	/**
	 * 주어진 오브젝트 클래스의 ActivateObjectIndexList를 생성하는 함수입니다.
	 *
	 * @param ObjectClass ActivateObjectIndexList를 생성할 오브젝트 클래스입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	void CreateActivateObjectIndexList(TSubclassOf<APRPooledObject> ObjectClass);

	/**
	 * 주어진 오브젝트 클래스의 UsedObjectIndexList를 생성하는 함수입니다.
	 * 
	 * @param ObjectClass UsedObjectIndexList를 생성할 오브젝트 클래스입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	void CreateUsedObjectIndexList(TSubclassOf<APRPooledObject> ObjectClass);

	/**
	 * 주어진 오브젝트가 비활성화될 때 실행하는 함수입니다.
	 * 비활성화된 오브젝트의 Index를 ActivateObjectIndexList에서 제거합니다.
	 * 
	 * @param PooledObject 비활성화된 오브젝트입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	void OnPooledObjectDeactivate(APRPooledObject* PooledObject);

	/**
	 * 주어진 동적으로 생성한 오브젝트가 비활성화될 때 실행하는 함수입니다.
	 * 
	 * @param PooledObject 비활성화된 동적으로 생성한 오브젝트입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	void OnDynamicObjectDeactivate(APRPooledObject* PooledObject);

	/**
	 * 주어진 동적으로 생성한 오브젝트를 제거하는 함수입니다.
	 * 
	 * @param PooledObject 제거할 오브젝트입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRObjectPoolSystem")
	void OnDynamicObjectDestroy(APRPooledObject* PooledObject);

private:
	/** ObjectPool의 설정 값을 가진 데이터 테이블입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> ObjectPoolSettingsDataTable;

	/** ObjectPool입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	FPRObjectPool ObjectPool;

	/** 활성화된 오브젝트의 Index 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	FPRActivateObjectIndexList ActivateObjectIndexList;

	/**
	 * 이전에 사용된 오브젝트들의 Index 목록입니다.
	 * 동적으로 생성하는 오브젝트의 Index에 오류가 생기지 않도록 합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	FPRUsedObjectIndexList UsedObjectIndexList;

	/** 동적으로 제거할 오브젝트의 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	FPRDynamicDestroyObjectList DynamicDestroyObjectList;
};
