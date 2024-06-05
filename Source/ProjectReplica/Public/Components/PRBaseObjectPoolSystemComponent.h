// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "Interfaces/PRPoolableInterface.h"
#include "PRBaseObjectPoolSystemComponent.generated.h"

#pragma region Struct
/**
 * 클래스별로 활성화된 오브젝트들의 Index를 보관하는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRActivateObjectIndexList
{
	GENERATED_BODY()

public:
	FPRActivateObjectIndexList()
		: List()
	{}

	FPRActivateObjectIndexList(const TMap<TSubclassOf<AActor>, FPRActivateIndexList>& NewList)
		: List(NewList)
	{}

public:
	/** 클래스 레퍼런스와 활성화된 Index를 보관하는 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AcitvateObjectIndexList")
	TMap<TSubclassOf<AActor>, FPRActivateIndexList> List;

public:
	/**
	 * 주어진 오브젝트에 해당하는 Indexes를 반환하는 함수입니다.
	 *
	 * @param ObjectToFind Indexes를 찾을 오브젝트입니다.
	 * @return Indexes를 찾을 경우 Indexes를 반환합니다. 못찾았을 경우 nullptr을 반환합니다.
	 */
	TSet<int32>* GetIndexesForObject(AActor* ObjectToFind)
	{
		if(!IsValid(ObjectToFind))
		{
			return nullptr;
		}

		TSubclassOf<AActor> ActorClass = ObjectToFind->GetClass();
		FPRActivateIndexList* ActivateIndexList = List.Find(ActorClass);
		if(ActivateIndexList)
		{
			return &ActivateIndexList->Indexes;
		}
		
		return nullptr;
	}
};

/**
 * 클래스별로 이전에 사용된 오브젝트들의 Index를 보관하는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRUsedObjectIndexList
{
	GENERATED_BODY()

public:
	FPRUsedObjectIndexList()
		: List()
	{}

	FPRUsedObjectIndexList(const TMap<TSubclassOf<AActor>, FPRUsedIndexList>& NewList)
		: List(NewList)
	{}

public:
	/** 클래스 레퍼런스와 해당 클래스의 이전에 사용된 Index를 보관하는 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UsedObjectIndexList")
	TMap<TSubclassOf<AActor>, FPRUsedIndexList> List;
};

/**
 * 동적으로 생성한 오브젝트와 해당 오브젝트를 제거하는 TimerHandle을 관리하는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRDynamicDestroyObject
{
	GENERATED_BODY()

public:
	FPRDynamicDestroyObject()
		: TimerHandles()
	{}

	FPRDynamicDestroyObject(const TMap<TObjectPtr<AActor>, FTimerHandle>& NewTimerHandles)
		: TimerHandles(NewTimerHandles)
	{}

public:
	/** 오브젝트와 해당 오브젝트를 제거하는 TimerHandle을 보관하는 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicDestroyObject")
	TMap<TObjectPtr<AActor>, FTimerHandle> TimerHandles;
};

/**
 * 동적으로 생성한 오브젝트 목록을 클래스별로 보관하는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRDynamicDestroyObjectList
{
	GENERATED_BODY()

public:
	FPRDynamicDestroyObjectList()
		: List()
	{}

	FPRDynamicDestroyObjectList(const TMap<TSubclassOf<AActor>, FPRDynamicDestroyObject>& NewList)
		: List(NewList)
	{}
	
public:
	/** 클래스 레퍼런스와 동적으로 생성한 오브젝트와 해당 오브젝트를 제거하는 TimerHandle을 보관한 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicDestroyObjectPool")
	TMap<TSubclassOf<AActor>, FPRDynamicDestroyObject> List;

public:
	/**
	 * 주어진 오브젝트에 해당하는 TimerHandle을 반환하는 함수입니다.
	 *
	 * @param ObjectToFind TimerHandle을 찾을 오브젝트입니다.
	 * @return TimerHandle을 찾았을 경우 TimerHandle을 반환합니다. 못 찾았을 경우 nullptr을 반환합니다.
	 */
	FTimerHandle* FindTimerHandleForObject(AActor* ObjectToFind)
	{
		if(!IsValid(ObjectToFind))
		{
			return nullptr;
		}

		for(auto& ListPair : List)
		{
			FPRDynamicDestroyObject& DestroyList = ListPair.Value;
			FTimerHandle* FoundTimerHandle = DestroyList.TimerHandles.Find(ObjectToFind);
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

	/** 모든 ObjectPool을 제거하는 함수입니다. */
	UFUNCTION(Blueprintable, Category = "PRBaseObjectPoolSystem")
	virtual void ClearAllObjectPool();

	// /**
	//  * 주어진 ObjectPool을 제거하는 함수입니다.
	//  *
	//  * @param NewObjectPool 제거할 ObjectPool입니다.
	//  */
	// UFUNCTION(Blueprintable, Category = "PRBaseObjectPoolSystem")
	// virtual void ClearObjectPool(FPRObjectPool& NewObjectPool);
	
	/**
	 * 주어진 DynamicDestroyObjectList를 제거하는 함수입니다.
	 * 
	 * @param NewDynamicDestroyObjectList 제거할 DynamicDestroyObjectList입니다.
	 */
	UFUNCTION(Blueprintable, Category = "PRBaseObjectPoolSystem")
	virtual void ClearDynamicDestroyObjectList(FPRDynamicDestroyObjectList& NewDynamicDestroyObjectList);
	
	// /**
	//  * 주어진 오브젝트 클래스에 해당하는 ObjectPool에서 비활성화된 오브젝트를 좌표와 회전 값을 적용한후 활성화하는 함수입니다.
	//  * 주어진 오브젝트 클래스에 해당하는 ObjectPool이 없을 경우, 동적으로 ObjectPool을 생성하고 비활성화된 오브젝트를 활성화하고 좌표와 회전 값을 적용합니다.
	//  * 비활성화된 오브젝트가 없을 경우, 동적으로 오브젝트를 생성하여 활성화하고 좌표와 회전 값을 적용합니다.
	//  *
	//  * @param PooledObjectClass ObjectPool에서 찾을 오브젝트의 클래스입니다.
	//  * @param NewLocation 적용할 오브젝트의 좌표입니다.
	//  * @param NewRotation 적용할 오브젝틔 회전 값입니다.
	//  * @return ObjectPool에서 찾아 활성화한 오브젝트를 반환합니다.
	//  */
	// UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	// virtual AActor* ActivatePooledObjectFromClass(TSubclassOf<AActor> PooledObjectClass, FVector NewLocation = FVector::ZeroVector, FRotator NewRotation = FRotator::ZeroRotator);
	//
	// /**
	//  * 주어진 비활성화된 오브젝트가 ObjectPool에 존재하면 활성화하고 좌표와 회전 값을 적용하는 함수입니다.
	//  * 주어진 오브젝트가 이미 활성화된 상태일 경우 nullptr을 반환합니다.
	//  *
	//  * @param PooledObject 활성화할 비활성화된 오브젝트입니다.
	//  * @param NewLocation 적용할 오브젝트의 좌표입니다.
	//  * @param NewRotation 적용할 오브젝틔 회전 값입니다.
	//  * @return 활성화한 오브젝트 오브젝트를 반환합니다.
	//  */
	// UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	// virtual AActor* ActivatePooledObject(AActor* PooledObject, FVector NewLocation = FVector::ZeroVector, FRotator NewRotation = FRotator::ZeroRotator);
	//
	// /**
	//  * 주어진 오브젝트 클래스에 해당하는 ObjectPool에서 활성화할 수 있는 오브젝트를 반환하는 함수입니다.
	//  * 주어진 오브젝트 클래스에서 해당하는 ObjectPool이 없을 경우, 동적으로 ObjectPool을 생성하고 비활성화된 오브젝트를 반환합니다.
 // 	 * ObjectPool에서 활성화할 수 있는 오브젝트가 없을 경우, 동적으로 오브젝트를 생성하여 반환합니다.
	//  *
	//  * @param PooledObjectClass ObjectPool에서 찾을 오브젝트의 클래스입니다.
	//  * @return 활성화할 수 있는 오브젝트를 반환합니다.
	//  */	
	// UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	// virtual AActor* GetActivateablePooledObject(TSubclassOf<AActor> PooledObjectClass);
	//
	// /**
	//  * 주어진 객체가 현재 활성화되어 있는지 확인하는 함수입니다.
	//  * 
	//  * @param PooledObject 확인할 객체입니다.
	//  * @return 객체가 유효하고, 풀링 가능하며, 현재 활성화된 경우 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	//  */
	// UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	// virtual bool IsActivatePooledObject(AActor* PooledObject) const;
	//
	// /**
	//  * 주어진 오브젝트 클래스의 ObjectPool이 생성되었는지 확인하는 함수입니다. 
	//  * 
	//  * @param PooledObjectClass 확인할 오브젝트 클래스입니다.
	//  * @return ObjectPool이 생성되었으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	//  */
	// UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	// virtual bool IsCreateObjectPool(TSubclassOf<AActor> PooledObjectClass) const;
	//
	// /**
	//  * 주어진 오브젝트 클래스의 ActivateObjectIndexList가 생성되었는지 확인하는 함수입니다.
	//  * 
	//  * @param PooledObjectClass 확인할 오브젝트 클래스입니다.
	//  * @return ActivateObjectIndexList가 생성되었으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	//  */
	// UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	// virtual bool IsCreateActivateObjectIndexList(TSubclassOf<AActor> PooledObjectClass) const;
	//
	// /**
	//  * 주어진 오브젝트 클래스의 UsedObjectIndexList가 생성되었는지 확인하는 함수입니다. 
	//  * 
	//  * @param PooledObjectClass 확인할 오브젝트 클래스입니다.
	//  * @return UsedObjectIndexList가 생성되었으면 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	//  */
	// UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	// virtual bool IsCreateUsedObjectIndexList(TSubclassOf<AActor> PooledObjectClass) const;

	/**
	 * 주어진 객체가 유효한 풀링 가능한 객체인지 확인하는 함수입니다.
	 *
	 * @param PoolableObject 확인할 객체입니다.
	 * @return 객체가 유효하고 PRPoolableInterface를 구현하는 경우 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	bool IsPoolableObject(UObject* PoolableObject) const;

	/**
	 * 주어진 오브젝트의 클래스가 유효한 풀링 가능한 오브젝트 클래스인지 확인하는 함수입니다.
	 *
	 * @param PoolableObjectClass 확인할 오브젝트 클래스입니다.
	 * @return 오브젝트의 클래스가 유효하고 PRPoolableInterface를 구현하는 경우 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	bool IsPoolableObjectClass(TSubclassOf<UObject> PoolableObjectClass) const;

	/**
	 * 사용 가능한 Index를 찾아 반환하는 함수입니다.
	 * 
	 * @param UsedIndexes 이미 사용 중인 Index 목록입니다.
	 * @return 사용 가능한 Index를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	int32 FindAvailableIndex(const TSet<int32>& UsedIndexes);

protected:
	/**
	 * 동적으로 생성한 오브젝트를 제거하는 딜레이 시간입니다.
	 * 동적으로 생성한 오브젝트가 비활성화 되었을 때, 해당 시간이 지난 후 오브젝트를 제거합니다. 
	 */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseObjectPoolSystem")
	float DynamicDestroyDelay;

	/** 동적으로 생성하는 ObjectPool의 PoolSize입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseObjectPoolSystem", meta = (ClampMin = "1"))
	int32 DynamicPoolSize;
};
