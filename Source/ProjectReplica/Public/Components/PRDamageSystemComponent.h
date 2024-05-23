// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRDamageSystemComponent.generated.h"

class UPRStatSystemComponent;
class UPRStateSystemComponent;
class APRDamageAmount;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBlockedDelegate, bool, CanBeParried);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageResponseDelegate, EPRDamageResponse, DamageResponse);

/**
 * 대미지의 처리를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRCanBeDamaged : uint8
{
	CanBeDamaged_BlockDamage		UMETA(DisplayName = "BlockDamage"),			// 대미지를 막음
	CanBeDamaged_DoDamage			UMETA(DisplayName = "DoDamage"),			// 대미지를 받음
	CanBeDamaged_NoDamage			UMETA(DisplayName = "NoDamage")				// 대미지를 받지 않음
};

/**
 * 대미지를 관리하는 ActorComponent 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRDamageSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRDamageSystemComponent();

public:
	virtual void InitializeComponent() override;

public:
	/** PROwner의 StatSystem을 바인딩하는 함수입니다. */
	void BindStatSystem(TObjectPtr<UPRStatSystemComponent> NewStatSystem);

	/** PROwner의 StateSystem을 바인딩하는 함수입니다. */
	void BindStateSystem(TObjectPtr<UPRStateSystemComponent> NewStateSystem);
	
	/**
	 * 회복량만큼 체력을 회복하고 회복한 후의 체력을 반환하는 함수입니다.
	 *
	 * @param Amount 회복량
	 * @return 회복한 후의 체력
	 */
	UFUNCTION(BlueprintCallable, Category = "DamageSystem")
	float Heal(float Amount);

	/**
	 * 대미지 정보에 해당하는 대미지를 받는 함수입니다.
	 *
	 * @param DamageInfo 받는 대미지의 정보
	 * @return 대미지를 받을 경우 true, 대미지를 받지 않을 경우 false를 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "DamageSystem")
	bool TakeDamage(FPRDamageInfo DamageInfo);

private:
	/**
	 * 받는 대미지의 정보에 따라 방어하여 대미지를 받지 않을지, 대미지를 받을지, 대미지를 받지 않을지 판별하는 함수입니다.
	 *
	 * @param bShouldDamageInvincible 무적 상태에도 대미지를 입힐지 여부
	 * @param bCanBeBlocked 방어 가능 여부
	 * @return 대미지 처리 여부
	 */
	EPRCanBeDamaged CanBeDamaged(const bool& bShouldDamageInvincible, const bool& bCanBeBlocked);
	
private:
	/** 캐릭터의 능력치를 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DamageSystem", meta = (AllowPrivateAccess = "true"))
	// class UPRStatSystemComponent* StatSystem;
	TWeakObjectPtr<class UPRStatSystemComponent> StatSystem;

	/** 캐릭터의 상태(State)를 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DamageSystem", meta = (AllowPrivateAccess = "true"))
	// class UPRStateSystemComponent* StateSystem;
	TWeakObjectPtr<class UPRStateSystemComponent> StateSystem;

	/** 대미지를 표시하는 액터의 클래스 레퍼런스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "임시", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class APRDamageAmount> DamageAmount;
	
public:
	/** 사망했을 때 호출하는 델리게이트입니다. */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "DamageSystem")
	FOnDeathDelegate OnDeathDelegate;

	/** 방어했을 때 호출하는 델리게이트입니다. */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "DamageSystem")
	FOnBlockedDelegate OnBlockedDelegate;

	/** 대미지에 반응할 때 호출하는 델리게이트입니다. */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "DamageSystem")
	FOnDamageResponseDelegate OnDamageResponseDelegate;
};
