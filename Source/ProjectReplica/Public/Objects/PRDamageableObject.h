// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interface_PRDamageable.h"
#include "PRDamageableObject.generated.h"

/**
 * 대미지를 받으면 파괴되는 오브젝트 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRDamageableObject : public AActor, public IInterface_PRDamageable
{
	GENERATED_BODY()
	
public:	
	APRDamageableObject();

protected:
	virtual void BeginPlay() override;

protected:
	/** 이 오브젝트의 RootComponent로 지정할 SceneComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRDamageableObject")
	TObjectPtr<class USceneComponent> Root;


#pragma region Interface_Damageable
public:
	/**
	 * 대미지를 받는 함수입니다.
	 *
	 * @param DamageInfo 대미지의 정보
	 * @return 대미지 적용 여부
	 */
	virtual bool TakeDamage_Implementation(FPRDamageInfo DamageInfo) override;
#pragma endregion
};
