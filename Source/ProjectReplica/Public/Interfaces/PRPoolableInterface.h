// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PRPoolableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPRPoolableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 오브젝트 풀에 관련된 Interface 클래스입니다.
 */
class PROJECTREPLICA_API IPRPoolableInterface
{
	GENERATED_BODY()

public:
	/** 활성화 되었는지 확인하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Poolable")
	bool IsActivate() const;
	virtual bool IsActivate_Implementation() const = 0;
	
	/** 활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Poolable")
	void Activate();
	virtual void Activate_Implementation() = 0;

	/** 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Poolable")
	void Deactivate();
	virtual void Deactivate_Implementation() = 0;
	
	/** PoolIndex를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Poolable")
	int32 GetPoolIndex() const;
	virtual int32 GetPoolIndex_Implementation() const = 0;

	/** 수명을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Poolable")
	float GetLifespan() const;
	virtual float GetLifespan_Implementation() const = 0;

	/**
	 * 수명을 설정하는 함수입니다.
	 * 
	 * @param NewLifespan 설정할 수명입니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Poolable")
	void SetLifespan(float NewLifespan);
	virtual void SetLifespan_Implementation(float NewLifespan) = 0;
};
