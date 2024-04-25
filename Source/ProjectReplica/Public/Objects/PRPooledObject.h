// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/Actor.h"
#include "PRPooledObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPooledObjectDeactivate, APRPooledObject*, PooledObject);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDynamicPooledObjectDeactivate, APRPooledObject*, PooledObject);

/**
 * 오브젝트 풀링에 사용하는 오브젝트 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRPooledObject : public AActor
{
	GENERATED_BODY()
	
public:	
	APRPooledObject();

protected:
	virtual void BeginPlay() override;

public:
	/** 오브젝트를 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PRPooledObject")
	void InitializeObject(AActor* NewObjectOwner = nullptr, int32 NewPoolIndex = -1);
	virtual void InitializeObject_Implementation(AActor* NewObjectOwner = nullptr, int32 NewPoolIndex = -1);
	
	/** 오브젝트가 활성화 되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRPooledObject")
	virtual bool IsActivate() const;
	
	/** 오브젝트를 활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PRPooledObject")
	void Activate();
	virtual void Activate_Implementation();

	/** 오브젝트를 활성화하고 오브젝트의 위치를 설정하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRPooledObject")
	void ActivateAndSetLocation(const FVector& NewLocation);

	/** 오브젝트를 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PRPooledObject")
	void Deactivate();
	virtual void Deactivate_Implementation();

protected:
	/** 입력받은 인자로 오브젝트의 수명을 설정하는 함수입니다. */
	void SetLifespan(float NewLifespan);

protected:
	/** 오브젝트의 활성화를 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRPooledObject")
	bool bActivate;

	/** 오브젝트의 수명입니다. 수명이 다할 경우 오브젝트는 비활성화됩니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRPooledObject")
	float Lifespan;

	/** 오브젝트의 수명을 관리하는 TimerHandle입니다. */
	UPROPERTY(BlueprintReadOnly, Category = "PREffect")
	FTimerHandle LifespanTimerHandle;

	/** 오브젝트의 소유자입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRPooledObject")
	TObjectPtr<AActor> ObjectOwner;

	/** 풀의 Index입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRPooledObject")
	int32 PoolIndex;

public:
	/** ObjectOwner를 반환하는 함수입니다. */
	FORCEINLINE AActor* GetObjectOwner() const { return ObjectOwner; }

	/** PoolIndex를 반환하는 함수입니다. */
	int32 GetPoolIndex() const;

public:
	/** 오브젝트가 비활성화될 때 실행하는 델리게이트입니다. */
	FOnPooledObjectDeactivate OnPooledObjectDeactivateDelegate;

	// /** 동적으로 생성한 오브젝트가 비활성화될 때 실행하는 델리게이트입니다. */
	// FOnDynamicPooledObjectDeactivate OnDynamicPooledObjectDeactivate;
};
