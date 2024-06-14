// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRObjectPoolSystemComponent.h"

#include "Interfaces/PRPoolableInterface.h"
#include "Objects/PRPooledObject.h"

UPRObjectPoolSystemComponent::UPRObjectPoolSystemComponent()
{
	ObjectPoolSettingsDataTable = nullptr;
	ObjectPool = FPRObjectPool();
	ActivateObjectIndexList = FPRActivateObjectIndexList();
	UsedObjectIndexList = FPRUsedObjectIndexList();
	DynamicDestroyObjectList = FPRDynamicDestroyObjectList();
}

#pragma region PRBaseObjectPoolSystem
void UPRObjectPoolSystemComponent::InitializeObjectPool()
{
	ClearAllObjectPool();

	// ObjectPoolSettings 데이터 테이블을 기반으로 ObjectPool을 생성합니다.
	if(ObjectPoolSettingsDataTable)
	{
		TArray<FName> RowNames = ObjectPoolSettingsDataTable->GetRowNames();
		for(const FName& RowName : RowNames)
		{
			FPRObjectPoolSettings* ObjectPoolSettings = ObjectPoolSettingsDataTable->FindRow<FPRObjectPoolSettings>(RowName, FString(""));
			if(ObjectPoolSettings)
			{
				CreateObjectPool(*ObjectPoolSettings);
			}
		}
	}
}

void UPRObjectPoolSystemComponent::ClearAllObjectPool()
{
	ActivateObjectIndexList.List.Empty();
	UsedObjectIndexList.List.Empty();
	ClearDynamicDestroyObjectList(DynamicDestroyObjectList);
	ClearObjectPool(ObjectPool);
}
#pragma endregion

APRPooledObject* UPRObjectPoolSystemComponent::ActivatePooledObjectFromClass(TSubclassOf<APRPooledObject> PooledObjectClass, FVector NewLocation, FRotator NewRotation)
{
	// 활성화할 오브젝트입니다.
	APRPooledObject* ActivateablePooledObject = GetActivateablePooledObject(PooledObjectClass);
	if(IsValid(ActivateablePooledObject))
	{
		return ActivatePooledObject(ActivateablePooledObject, NewLocation, NewRotation);
	}
	
	return nullptr;
}

APRPooledObject* UPRObjectPoolSystemComponent::ActivatePooledObject(APRPooledObject* PooledObject, FVector NewLocation, FRotator NewRotation)
{
	// 유효한 풀링 가능한 객체가 아니거나, 이미 활성화되었거나, ObjectPool이 생성되지 않았다면 nullptr을 반환합니다.
	if(!IsPoolableObject(PooledObject)
		|| IsActivatePooledObject(PooledObject)
		|| !IsCreateObjectPool(PooledObject->GetClass()))
	{
		return nullptr;
	}
	
	// 동적으로 생성한 오브젝트일 경우 DynamicObjectDestroyTimer가 작동 중이라면 DynamicObjectDestroyTimer를 정지합니다.
	FTimerHandle* DynamicObjectDestroyTimer = DynamicDestroyObjectList.FindTimerHandleForObject(*PooledObject);
	if(DynamicObjectDestroyTimer)
	{
		GetWorld()->GetTimerManager().ClearTimer(*DynamicObjectDestroyTimer);
	}

	// 오브젝트를 Spawn할 위치와 회전 값을 적용하고 활성화합니다.
	PooledObject->SetActorLocationAndRotation(NewLocation, NewRotation);
	ActivateObject(PooledObject);
	
	// 해당 오브젝트 클래스를 처음 활성화하는 경우 ActivateObjectIndexList를 생성합니다.
	if(!IsCreateActivateObjectIndexList(PooledObject->GetClass()))
	{
		CreateActivateObjectIndexList(PooledObject->GetClass());
	}

	// 활성화된 오브젝트의 Index를 ActivateObjectIndexList에 저장합니다.
	const int32 PoolIndex = GetPoolIndex(PooledObject);
	ActivateObjectIndexList.GetIndexesForObject(PooledObject)->Add(PoolIndex);
	
	return PooledObject;
}

APRPooledObject* UPRObjectPoolSystemComponent::GetActivateablePooledObject(TSubclassOf<APRPooledObject> PooledObjectClass)
{
	// 오브젝트 클래스가 유효하지 않을 경우 nullptr을 반환합니다.
	if(!PooledObjectClass)
	{
		return nullptr;
	}
	
	// 해당 오브젝트 클래스의 ObjectPool이 생성되었는지 확인하고, 없으면 생성합니다.
	if(!IsCreateObjectPool(PooledObjectClass))
	{
		FPRObjectPoolSettings ObjectPoolSettings = FPRObjectPoolSettings(PooledObjectClass, DynamicPoolSize);
		CreateObjectPool(ObjectPoolSettings);
	}

	// ObjectPool에서 해당 오브젝트 클래스의 Pool을 얻습니다.
	FPRPool* PoolEntry = ObjectPool.Pool.Find(PooledObjectClass);
	if(PoolEntry == nullptr)
	{
		// 지정된 오브젝트 클래스가 없으면 nullptr을 반환합니다.
		return nullptr;
	}
	
	// 활성화할 오브젝트입니다.
	APRPooledObject* ActivateablePooledObject = nullptr;

	// PoolEntry에서 활성화되지 않은 오브젝트를 찾습니다.
	for(const auto& PooledObject : PoolEntry->PooledObjects)
	{
		if(!IsActivatePooledObject(PooledObject))
		{
			ActivateablePooledObject = PooledObject;
		
			break;
		}
	}

	// PoolEntry의 모든 오브젝트가 활성화되었을 경우 새로운 오브젝트를 생성합니다.
	if(!ActivateablePooledObject)
	{
		ActivateablePooledObject = SpawnDynamicObjectInWorld(PooledObjectClass);
	}

	// 동적으로 생성된 오브젝트일 경우 DynamicDestroyTimer를 정지합니다.
	if(IsDynamicPooledObject(ActivateablePooledObject))
	{
		FPRDynamicDestroyObject* DynamicDestroyObject = DynamicDestroyObjectList.List.Find(ActivateablePooledObject->GetClass());
		if(DynamicDestroyObject)
		{
			FTimerHandle* DynamicDestroyTimer = DynamicDestroyObject->TimerHandles.Find(ActivateablePooledObject);
			if(DynamicDestroyTimer)
			{
				GetWorld()->GetTimerManager().ClearTimer(*DynamicDestroyTimer);
			}
		}
	}
	
	return ActivateablePooledObject;
}

bool UPRObjectPoolSystemComponent::IsActivatePooledObject(APRPooledObject* PooledObject) const
{
	// 유효하지 않는 오브젝트이거나 풀링 가능한 객체가 아니면 false를 반환합니다.
	if(!IsValid(PooledObject) || !IsPoolableObject(PooledObject))
	{
		return false;
	}

	// 객체의 클래스에 해당하는 활성화된 Index 목록을 찾습니다.
	const FPRActivateIndexList* IndexList = ActivateObjectIndexList.List.Find(PooledObject->GetClass());
	if(IndexList)
	{
		// 객체의 PoolIndex를 가져오고 객체가 활성화된 상태인지 확인합니다.
		const int32 PooledIndex = GetPoolIndex(PooledObject);
		const bool bIsPooledObjectActivated = IsActivateObject(PooledObject);

		// Index 목록에 해당 Index가 포함되어 있고, 객체가 활성화된 상태이면 true를 반환합니다.
		return IndexList->Indexes.Contains(PooledIndex) && bIsPooledObjectActivated;
	}

	// 위 조건을 모두 만족하지 않으면 false를 반환합니다.
	return false;
}

bool UPRObjectPoolSystemComponent::IsCreateObjectPool(TSubclassOf<APRPooledObject> PooledObjectClass) const
{
	return ObjectPool.Pool.Contains(PooledObjectClass);
}

bool UPRObjectPoolSystemComponent::IsCreateActivateObjectIndexList(TSubclassOf<APRPooledObject> PooledObjectClass) const
{
	return ActivateObjectIndexList.List.Contains(PooledObjectClass);
}

bool UPRObjectPoolSystemComponent::IsCreateUsedObjectIndexList(TSubclassOf<APRPooledObject> PooledObjectClass) const
{
	return UsedObjectIndexList.List.Contains(PooledObjectClass);
}

bool UPRObjectPoolSystemComponent::IsDynamicPooledObject(APRPooledObject* PooledObject) const
{
	// 주어진 객체가 유효한 풀링 가능한 객체인지 확인합니다.
	if(!IsPoolableObject(PooledObject))
	{
		return false;
	}

	const FPRDynamicDestroyObject* DynamicDestroyObject = DynamicDestroyObjectList.List.Find(PooledObject->GetClass());
	if(DynamicDestroyObject)
	{
		return DynamicDestroyObject->TimerHandles.Contains(PooledObject);
	}

	return false;
}

void UPRObjectPoolSystemComponent::ClearObjectPool(FPRObjectPool& NewObjectPool)
{
	// ObjectPool의 모든 클래스에 대해 반복합니다.
	for(auto& PoolEntry : NewObjectPool.Pool)
	{
		FPRPool& Pool = PoolEntry.Value;
		for(auto& PooledObject : Pool.PooledObjects)
		{
			if(IsValid(PooledObject))
			{
				// 오브젝트를 제거합니다.
				PooledObject->ConditionalBeginDestroy();		// 오브젝트를 안전하게 제거하는 함수입니다. 가비지 컬렉션 대상이 되기 전에 수동으로 메모리에서 해제합니다.
				PooledObject = nullptr;
			}
		}

		Pool.PooledObjects.Empty();
	}
	
	NewObjectPool.Pool.Empty();
}

APRPooledObject* UPRObjectPoolSystemComponent::SpawnObjectInWorld(TSubclassOf<APRPooledObject> ObjectClass)
{
	// 월드가 유효하지 않거나 ObjectClass가 풀링 가능한 클래스가 아닐 경우 nullptr을 반환합니다.
	if(!IsValid(GetWorld()) || !IsPoolableObjectClass(ObjectClass))
	{
		return nullptr;
	}

	APRPooledObject* SpawnObject = GetWorld()->SpawnActor<APRPooledObject>(ObjectClass);
	if(IsValid(SpawnObject))
	{
		return SpawnObject;
	}
	
	return nullptr;
}

APRPooledObject* UPRObjectPoolSystemComponent::SpawnDynamicObjectInWorld(TSubclassOf<APRPooledObject> PooledObjectClass)
{
	if(!PooledObjectClass)
	{
		return nullptr;
	}
	
	APRPooledObject* DynamicObject = nullptr;

	// Critical Section 시작
	FCriticalSection CriticalSection;
	CriticalSection.Lock();

	// 해당 오브젝트 클래스의 UsedObjectIndexList가 생성되었는지 확인하고, 없으면 생성합니다.
	if(!IsCreateUsedObjectIndexList(PooledObjectClass))
	{
		CreateUsedObjectIndexList(PooledObjectClass);
	}

	// UsedObjectIndexList에서 해당 오브젝트 클래스의 UsedIndexList를 얻습니다.
	FPRUsedIndexList* UsedIndexList = UsedObjectIndexList.List.Find(PooledObjectClass);
	if(UsedIndexList == nullptr)
	{
		// 지정된 오브젝트 클래스가 없습니다.
		return nullptr;
	}

	// 사용 가능한 Index를 구합니다.
	const int32 NewIndex = FindAvailableIndex(UsedIndexList->Indexes);
	// 사용 가능한 Index를 UsedIndexList에 추가합니다.
	UsedIndexList->Indexes.Add(NewIndex);

	// Critical Section 끝
	CriticalSection.Unlock();

	// 새로운 오브젝트를 생성하고 사용 가능한 Index로 초기화합니다.
	DynamicObject = SpawnAndInitializeObject(PooledObjectClass, NewIndex);
		
	// OnDynamicObjectDeactivate 함수를 바인딩합니다.
	DynamicObject->OnPooledObjectDeactivateDelegate.AddDynamic(this, &UPRObjectPoolSystemComponent::OnDynamicObjectDeactivate);

	// ObjectPool에서 해당 Object의 Pool을 얻습니다.
	FPRPool* PoolEntry = ObjectPool.Pool.Find(PooledObjectClass);
	if(!PoolEntry)
	{
		// Pool이 없을 경우 생성한 Object를 제거하고 nullptr을 반환합니다.
		DynamicObject->ConditionalBeginDestroy();
		
		return nullptr;
	}
	
	// 새로 생성한 오브젝트를 PoolEntry에 추가합니다.
	PoolEntry->PooledObjects.Emplace(DynamicObject);

	return DynamicObject;
}

APRPooledObject* UPRObjectPoolSystemComponent::SpawnAndInitializeObject(TSubclassOf<APRPooledObject> ObjectClass, int32 Index)
{
	APRPooledObject* SpawnObject = SpawnObjectInWorld(ObjectClass);
	if(IsValid(SpawnObject))
	{
		// 생성한 오브젝트를 초기화하고 OnPooledObjectDeactivate 함수를 바인딩합니다.
		SpawnObject->InitializeObject(GetOwner(), Index);
		SpawnObject->OnPooledObjectDeactivateDelegate.AddDynamic(this, &UPRObjectPoolSystemComponent::OnPooledObjectDeactivate);
	}
	
	return SpawnObject;
}

void UPRObjectPoolSystemComponent::CreateObjectPool(const FPRObjectPoolSettings& ObjectPoolSettings)
{
	if(GetWorld()
		&& ObjectPoolSettings.PooledObjectClass
		&& IsPoolableObjectClass(ObjectPoolSettings.PooledObjectClass))
	{
		// 오브젝트를 생성하여 ObjectPool에 추가할 NewPool에 추가합니다.
		FPRPool NewPool;
		
		for(int32 Index = 0; Index < ObjectPoolSettings.PoolSize; Index++)
		{
			// 오브젝트를 월드에 Spawn하고 초기화합니다.
			APRPooledObject* SpawnObject = SpawnAndInitializeObject(ObjectPoolSettings.PooledObjectClass, Index);
			if(IsValid(SpawnObject))
			{
				NewPool.PooledObjects.Emplace(SpawnObject);
			}
		}

		// 초기화된 NewPool을 ObjectPool에 추가하고 ActivateObjectIndexList를 생성합니다.
		ObjectPool.Pool.Emplace(ObjectPoolSettings.PooledObjectClass, NewPool);
		CreateActivateObjectIndexList(ObjectPoolSettings.PooledObjectClass);
	}
}

void UPRObjectPoolSystemComponent::CreateActivateObjectIndexList(TSubclassOf<APRPooledObject> ObjectClass)
{
	if(IsPoolableObjectClass(ObjectClass))
	{
		ActivateObjectIndexList.List.Emplace(ObjectClass);
	}
}

void UPRObjectPoolSystemComponent::CreateUsedObjectIndexList(TSubclassOf<APRPooledObject> ObjectClass)
{
	if(IsPoolableObjectClass(ObjectClass))
	{
		FPRPool* PoolEntry = ObjectPool.Pool.Find(ObjectClass);
		if(PoolEntry)
		{
			FPRUsedIndexList UsedIndexList;
			for(const auto& PooledObject : PoolEntry->PooledObjects)
			{
				if(IsValid(PooledObject))
				{
					UsedIndexList.Indexes.Add(GetPoolIndex(PooledObject));
				}
			}
			
			UsedObjectIndexList.List.Emplace(ObjectClass, UsedIndexList);	
		}
	}
}

void UPRObjectPoolSystemComponent::OnPooledObjectDeactivate(APRPooledObject* PooledObject)
{
	// 유효하지 않는 오브젝트이거나 풀링 가능한 객체가 아니면 반환합니다.
	if(!IsValid(PooledObject) || !IsPoolableObject(PooledObject))
	{
		return;
	}

	// 오브젝트가 활성화된 상태라면 비활성화합니다.
	if(IsActivatePooledObject(PooledObject))
	{
		FPRActivateIndexList* ActivateIndexList = ActivateObjectIndexList.List.Find(PooledObject->GetClass());
		if(ActivateIndexList)
		{
			// 비활성화된 오브젝트의 Index를 제거합니다.
			ActivateIndexList->Indexes.Remove(GetPoolIndex(PooledObject));
		}
	}
}

void UPRObjectPoolSystemComponent::OnDynamicObjectDeactivate(APRPooledObject* PooledObject)
{
	if(!IsValid(PooledObject) || !IsPoolableObject(PooledObject))
	{
		return;
	}
	
	if(DynamicLifespan > 0.0f)
	{
		// 동적 수명이 끝난 후 오브젝트를 제거하도록 타이머를 설정합니다.
		FTimerHandle DynamicLifespanTimerHandle;
		FTimerDelegate DynamicLifespanDelegate = FTimerDelegate::CreateUObject(this, &UPRObjectPoolSystemComponent::OnDynamicObjectDestroy, PooledObject);
		GetWorld()->GetTimerManager().SetTimer(DynamicLifespanTimerHandle, DynamicLifespanDelegate, DynamicLifespan, false);

		// TimerHandle을 추가합니다.
		FPRDynamicDestroyObject* DynamicDestroyObject = DynamicDestroyObjectList.List.Find(PooledObject->GetClass());
		if(DynamicDestroyObject)
		{
			DynamicDestroyObject->TimerHandles.Emplace(PooledObject, DynamicLifespanTimerHandle);
		}
		else
		{
			FPRDynamicDestroyObject NewDynamicDestroyObject;
			NewDynamicDestroyObject.TimerHandles.Emplace(PooledObject, DynamicLifespanTimerHandle);
			DynamicDestroyObjectList.List.Emplace(PooledObject->GetClass(), NewDynamicDestroyObject);
		}
	}
	else
	{
		// 동적 수명이 없을 경우 타이머를 실행하지 않고 바로 오브젝트를 제거합니다.
		OnDynamicObjectDestroy(PooledObject);		
	}
}

void UPRObjectPoolSystemComponent::OnDynamicObjectDestroy(APRPooledObject* PooledObject)
{
	// DynamicObjectDestroyTimer를 제거합니다.
	FPRDynamicDestroyObject* DynamicDestroyObject = DynamicDestroyObjectList.List.Find(PooledObject->GetClass());
	if(DynamicDestroyObject)
	{
		FTimerHandle* TimerHandle = DynamicDestroyObject->TimerHandles.Find(PooledObject);
		GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
		DynamicDestroyObject->TimerHandles.Remove(PooledObject);
	}

	// 오브젝트 클래스의 UsedObjectIndex를 얻습니다.
	FPRUsedIndexList* UsedIndexList = UsedObjectIndexList.List.Find(PooledObject->GetClass());
	if(UsedIndexList)
	{
		// 사용 중인 Index를 제거합니다.
		UsedIndexList->Indexes.Remove(GetPoolIndex(PooledObject));
	}

	// 오브젝트 클래스의 ObjectPool이 생성되었는지 확인합니다.
	if(IsCreateObjectPool(PooledObject->GetClass()))
	{
		// ObjectPool에서 오브젝트를 제거합니다.
		FPRPool* PoolEntry = ObjectPool.Pool.Find(PooledObject->GetClass());
		if(PoolEntry)
		{
			PoolEntry->PooledObjects.Remove(PooledObject);
		}
	}
		
	PooledObject->ConditionalBeginDestroy();
}
