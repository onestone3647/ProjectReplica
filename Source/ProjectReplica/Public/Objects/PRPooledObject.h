// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PRPoolableInterface.h"
#include "PRPooledObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPooledObjectDeactivate, APRPooledObject*, PooledObject);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDynamicPooledObjectDeactivate, APRPooledObject*, PooledObject);

/**
 * 오브젝트 풀링에 사용하는 오브젝트 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRPooledObject : public AActor, public IPRPoolableInterface
{
	GENERATED_BODY()
	
public:	
	APRPooledObject();

protected:
	virtual void BeginPlay() override;

#pragma region PooledableInterface
public:
	/** 오브젝트가 활성화 되었는지 확인하는 함수입니다. */
	virtual bool IsActivate_Implementation() const override;
	
	/** 오브젝트를 활성화하는 함수입니다. */
	virtual void Activate_Implementation() override;

	/** 오브젝트를 비활성화하는 함수입니다. */
	virtual void Deactivate_Implementation() override;
	
	/** 오브젝트의 PoolIndex를 반환하는 함수입니다. */
	virtual int32 GetPoolIndex_Implementation() const override;

	/**
	 * 수명을 설정하는 함수입니다.
	 * 
	 * @param NewLifespan 설정할 수명입니다.
	 */
	virtual void SetLifespan_Implementation(float NewLifespan) override;
#pragma endregion
	
public:
	/** 오브젝트를 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PRPooledObject")
	void InitializeObject(AActor* NewObjectOwner = nullptr, int32 NewPoolIndex = -1);
	virtual void InitializeObject_Implementation(AActor* NewObjectOwner = nullptr, int32 NewPoolIndex = -1);

	/** 오브젝트를 활성화하고 오브젝트의 위치를 설정하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRPooledObject")
	void ActivateAndSetLocation(const FVector& NewLocation);

protected:
	/** 오브젝트가 비활성화될 때 실행하는 함수입니다. */
	UFUNCTION()
	void OnDeactivate();

protected:
	/** 오브젝트의 활성화를 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRPooledObject")
	bool bActivate;

	/**
	 * 오브젝트의 수명입니다. 수명이 다할 경우 오브젝트는 비활성화됩니다.
	 *
	 * 5.3.2버전 버그
	 * 변수명을 Lifespan으로 하면 Initial Life Span 변수로 값이 옮겨짐
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRPooledObject")
	float ObjectLifespan;

	/** 오브젝트의 수명을 관리하는 TimerHandle입니다. */
	UPROPERTY(BlueprintReadOnly, Category = "PREffect")
	FTimerHandle LifespanTimerHandle;

	/** 오브젝트의 소유자입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRPooledObject")
	TObjectPtr<AActor> ObjectOwner;

	/** Pool의 Index입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRPooledObject")
	int32 PoolIndex;

public:
	/** ObjectOwner를 반환하는 함수입니다. */
	FORCEINLINE AActor* GetObjectOwner() const { return ObjectOwner; }

public:
	/** 오브젝트가 비활성화될 때 실행하는 델리게이트입니다. */
	FOnPooledObjectDeactivate OnPooledObjectDeactivateDelegate;

	// /** 동적으로 생성한 오브젝트가 비활성화될 때 실행하는 델리게이트입니다. */
	// FOnDynamicPooledObjectDeactivate OnDynamicPooledObjectDeactivate;
};
