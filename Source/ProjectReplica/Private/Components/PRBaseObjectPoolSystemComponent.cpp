// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRBaseObjectPoolSystemComponent.h"
#include "Interfaces/PRPoolableInterface.h"


UPRBaseObjectPoolSystemComponent::UPRBaseObjectPoolSystemComponent()
{
}

void UPRBaseObjectPoolSystemComponent::DestroyComponent(bool bPromoteChildren)
{
	// ObjectPool을 제거합니다.
	ClearObjectPool();
	
	Super::DestroyComponent(bPromoteChildren);
}

void UPRBaseObjectPoolSystemComponent::InitializeObjectPool()
{
	// 자식 클래스에서 오버라이딩하여 사용합니다.
}

void UPRBaseObjectPoolSystemComponent::ClearObjectPool()
{
	// 자식 클래스에서 오버라이딩하여 사용합니다.
}

UObject* UPRBaseObjectPoolSystemComponent::ActivatePooledObjectFromClass(TSubclassOf<UObject> PooledObjectClass, FVector NewLocation, FRotator NewRotation)
{
	return nullptr;
}

UObject* UPRBaseObjectPoolSystemComponent::ActivatePooledObject(UObject* PooledObject, FVector NewLocation,	FRotator NewRotation)
{
	return nullptr;
}

UObject* UPRBaseObjectPoolSystemComponent::GetActivateablePooledObject(TSubclassOf<UObject> PooledObjectClass)
{
	return nullptr;
}

bool UPRBaseObjectPoolSystemComponent::IsActivatePooledObject(UObject* PooledObject) const
{
	// 자식 클래스에서 오버라이딩하여 사용합니다.

	return false;
}

bool UPRBaseObjectPoolSystemComponent::IsCreateObjectPool(TSubclassOf<UObject> PooledObjectClass) const
{
	// 자식 클래스에서 오버라이딩하여 사용합니다.

	return false;
}

bool UPRBaseObjectPoolSystemComponent::IsCreateActivateObjectIndexList(TSubclassOf<UObject> PooledObjectClass) const
{
	// 자식 클래스에서 오버라이딩하여 사용합니다.

	return false;
}

bool UPRBaseObjectPoolSystemComponent::IsCreateUsedObjectIndexList(TSubclassOf<UObject> PooledObjectClass) const
{
	// 자식 클래스에서 오버라이딩하여 사용합니다.

	return false;
}
