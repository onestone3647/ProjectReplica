// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PRPooledObject.h"

APRPooledObject::APRPooledObject()
{
	PrimaryActorTick.bCanEverTick = false;

	bActivate = false;
	ObjectLifespan = 0.0f;
	ObjectOwner = nullptr;
	PoolIndex = INDEX_NONE;
}

void APRPooledObject::BeginPlay()
{
	Super::BeginPlay();
}

#pragma region PooledableInterface
bool APRPooledObject::IsActivate_Implementation() const
{
	return bActivate;
}

void APRPooledObject::Activate_Implementation()
{
	bActivate = true;
	SetActorHiddenInGame(!bActivate);
	SetActorTickEnabled(bActivate);

	// 오브젝트의 수명을 설정합니다. 오브젝트의 수명이 끝나면 오브젝트를 비활성화합니다.
	IPRPoolableInterface::Execute_SetLifespan(this, ObjectLifespan);
}

void APRPooledObject::Deactivate_Implementation()
{
	bActivate = false;
	SetActorHiddenInGame(!bActivate);
	SetActorTickEnabled(bActivate);

	// 오브젝트에 설정된 모든 타이머를 초기화합니다.
	GetWorldTimerManager().ClearAllTimersForObject(this);

	// 델리게이트를 호출합니다.
	OnPooledObjectDeactivateDelegate.Broadcast(this);
}

int32 APRPooledObject::GetPoolIndex_Implementation() const
{
	return PoolIndex;
}

void APRPooledObject::SetLifespan_Implementation(float NewLifespan)
{
	ObjectLifespan = NewLifespan;
	if(NewLifespan > 0.0f)
	{
		// 수명이 0보다 클 경우, 즉 새로운 수명이 설정된 경우 타이머를 설정합니다.
		GetWorldTimerManager().SetTimer(LifespanTimerHandle, this, &APRPooledObject::OnDeactivate, NewLifespan);
	}
	else
	{
		// 수명이 0보다 작거나 같을 경우, 즉 오브젝트의 수명이 무한대인 경우 이전에 설정된 타이머를 지워 제한된 수명을 가지지 않게 합니다.
		GetWorldTimerManager().ClearTimer(LifespanTimerHandle);
	}
}
#pragma endregion

void APRPooledObject::InitializeObject_Implementation(AActor* NewObjectOwner, int32 NewPoolIndex)
{
	// 오브젝트를 비활성화 상태로 설정합니다.
	bActivate = false;
	SetActorHiddenInGame(!bActivate);

	ObjectOwner = NewObjectOwner;
	PoolIndex = NewPoolIndex;
	
	// 오브젝트에 설정된 모든 타이머를 초기화합니다.
	GetWorldTimerManager().ClearAllTimersForObject(this);

	// 델리게이트에 바인딩된 함수를 제거합니다.
	OnPooledObjectDeactivateDelegate.Clear();
}

void APRPooledObject::ActivateAndSetLocation(const FVector& NewLocation)
{
	IPRPoolableInterface::Execute_Activate(this);
	SetActorLocation(NewLocation);
}

void APRPooledObject::OnDeactivate()
{
	IPRPoolableInterface::Execute_Deactivate(this);
}

