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

	FPRActivateObjectIndexList(const TMap<TSubclassOf<UObject>, FPRActivateIndexList>& NewList)
		: List(NewList)
	{}

public:
	/** 클래스 레퍼런스와 활성화된 Index를 보관하는 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAcitvateObjectIndexList")
	TMap<TSubclassOf<UObject>, FPRActivateIndexList> List;

public:
	/**
	 * 주어진 오브젝트에 해당하는 Indexes를 반환하는 함수입니다.
	 *
	 * @param ObjectToFind Indexes를 찾을 오브젝트입니다.
	 * @return Indexes를 찾을 경우 Indexes를 반환합니다. 못찾았을 경우 nullptr을 반환합니다.
	 */
	TSet<int32>* GetIndexesForObject(UObject* ObjectToFind)
	{
		if(!IsValid(ObjectToFind))
		{
			return nullptr;
		}

		TSubclassOf<UObject> ActorClass = ObjectToFind->GetClass();
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

	FPRUsedObjectIndexList(const TMap<TSubclassOf<UObject>, FPRUsedIndexList>& NewList)
		: List(NewList)
	{}

public:
	/** 클래스 레퍼런스와 해당 클래스의 이전에 사용된 Index를 보관하는 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRUsedObjectIndexList")
	TMap<TSubclassOf<UObject>, FPRUsedIndexList> List;
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

	FPRDynamicDestroyObject(const TMap<TObjectPtr<UObject>, FTimerHandle>& NewTimerHandles)
		: TimerHandles(NewTimerHandles)
	{}

public:
	/** 오브젝트와 해당 오브젝트를 제거하는 TimerHandle을 보관하는 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRDynamicDestroyObject")
	TMap<TObjectPtr<UObject>, FTimerHandle> TimerHandles;
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

	FPRDynamicDestroyObjectList(const TMap<TSubclassOf<UObject>, FPRDynamicDestroyObject>& NewList)
		: List(NewList)
	{}
	
public:
	/** 클래스 레퍼런스와 동적으로 생성한 오브젝트와 해당 오브젝트를 제거하는 TimerHandle을 보관한 Map입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRDynamicDestroyObjectPool")
	TMap<TSubclassOf<UObject>, FPRDynamicDestroyObject> List;

public:
	/**
	 * 주어진 오브젝트에 해당하는 TimerHandle을 반환하는 함수입니다.
	 *
	 * @param ObjectToFind TimerHandle을 찾을 오브젝트입니다.
	 * @return TimerHandle을 찾았을 경우 TimerHandle을 반환합니다. 못 찾았을 경우 nullptr을 반환합니다.
	 */
	FTimerHandle* FindTimerHandleForObject(UObject& ObjectToFind)
	{
		if(!IsValid(&ObjectToFind))
		{
			return nullptr;
		}

		FPRDynamicDestroyObject* DestroyObjects = List.Find(ObjectToFind.GetClass());
		if(DestroyObjects)
		{
			FTimerHandle* FoundTimerHandle = DestroyObjects->TimerHandles.Find(ObjectToFind);
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
	 * 주어진 객체를 활성화하는 함수입니다.
	 * 
	 * @param PoolableObject 활성화할 객체입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	void ActivateObject(UObject* PoolableObject);

	/**
	 * 주어진 객체를 비활성화하는 함수입니다.
	 * 
	 * @param PoolableObject 비활성화할 객체입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	void DeactivateObject(UObject* PoolableObject);	

	/**
 	 * 주어진 객체의 PoolIndex를 반환하는 함수입니다.
 	 * 
 	 * @param PoolableObject PoolIndex를 반환할 객체입니다.
 	 * @return 주어진 객체가 유효하고 풀링 가능한 객체일 경우 객체의 PoolIndex를 반환합니다.
 	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	int32 GetPoolIndex(UObject* PoolableObject) const;

	/**
	  * 주어진 객체의 Lifespan을 반환하는 함수입니다.
	  * 
	  * @param PoolableObject Lifespan을 반환할 객체입니다.
	  * @return 주어진 객체가 유효하고 풀링 가능한 객체일 경우 객체의 Lifespan을 반환합니다.
	  */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	float GetLifespan(UObject* PoolableObject) const;

	/**
	  * 주어진 객체의 Lifespan을 설정하는 함수입니다.
	  * 
	  * @param PoolableObject Lifespan을 설정할 객체입니다.
	  */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	void SetLifespan(UObject* PoolableObject, float NewLifespan);

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
	 * 주어진 객체가 활성화 되었는지 확인하는 함수입니다.
	 * 
	 * @param PoolableObject 확인할 객체입니다.
	 * @return 주어진 객체가 활성화 되었을 경우 true를 반환합니다. 그렇지 않으면 false를 반환합니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseObjectPoolSystem")
	bool IsActivateObject(UObject* PoolableObject) const;
	
	/**
	 * 동적으로 생성한 오브젝트을 제거하는 함수입니다.
	 * 
	 * @param TargetDynamicDestroyObjectList 제거할 동적으로생성한 오브젝트의 목록입니다.
	 */
	UFUNCTION(Blueprintable, Category = "PRBaseObjectPoolSystem")
	virtual void ClearDynamicDestroyObjectList(FPRDynamicDestroyObjectList& TargetDynamicDestroyObjectList);
	
protected:
	/**
	 * 동적으로 생성한 오브젝트의 수명입니다.
	 * 동적으로 생성한 오브젝트가 비활성화 되었을 때, 해당 시간이 지난 후 오브젝트를 제거합니다. 
	 */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseObjectPoolSystem")
	float DynamicLifespan;

	/** 동적으로 생성하는 ObjectPool의 PoolSize입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseObjectPoolSystem", meta = (ClampMin = "1"))
	int32 DynamicPoolSize;

#pragma region Template
protected:
	/**
	 * Template를 사용한 동적으로 생성한 오브젝트들을 제거하는 함수입니다.
	 *
	 * @param List 동적으로 생성한 오브젝트의 목록입니다.
	 */
	template <typename KeyType>
	void ClearDynamicDestroyObjects(TMap<KeyType, FPRDynamicDestroyObject>& List)
	{
		for(auto& ListEntry : List)
		{
			FPRDynamicDestroyObject& DynamicDestroyObject = ListEntry.Value;
			if(&DynamicDestroyObject)
			{
				// DynamicDestroyObject의 모든 타이머를 해제하고 오브젝트를 제거합니다.
				for(auto& TimerEntry : DynamicDestroyObject.TimerHandles)
				{
					if(IsValid(TimerEntry.Key))
					{
						// 타이머를 해제합니다.
						GetWorld()->GetTimerManager().ClearTimer(TimerEntry.Value);

						// 오브젝트를 제거합니다.
						TimerEntry.Key->ConditionalBeginDestroy();		// 오브젝트를 안전하게 제거하는 함수입니다. 가비지 컬렉션 대상이 되기 전에 수동으로 메모리에서 해제합니다.
						TimerEntry.Key = nullptr;
					}
				}

				DynamicDestroyObject.TimerHandles.Empty();
			}
		}

		List.Empty();
	}
#pragma endregion 
};
