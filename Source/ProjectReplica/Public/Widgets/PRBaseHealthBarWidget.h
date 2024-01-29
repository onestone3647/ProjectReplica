// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Blueprint/UserWidget.h"
#include "PRBaseHealthBarWidget.generated.h"

class IInterface_PRDamageable;
class UProgressBar;

/**
 * 캐릭터의 체력을 나타내는 기본 UserWidget 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRBaseHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPRBaseHealthBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	/** HealthBarBuffer를 최신화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "BaseHealthBar")
	void UpdateHealthBarBuffer();

	/** HealthBar에 적용할 퍼센트 값을 계산하여 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "BaseHealthBar")
	float GetHealthBarPercent() const;

protected:
	/** 이 위젯을 사용하는 캐릭터의 현재 체력을 나타내는 ProgressBar입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "BaseHealthBar", meta = (AllowPrivateAccess = "true", BindWidget))
	TObjectPtr<class UProgressBar> HealthBar;

	/** 현재 체력의 값이 변경되어 나타나는 HealthBar의 Buffer를 나타내는 ProgressBar입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "BaseHealthBar", meta = (AllowPrivateAccess = "true", BindWidget))
	TObjectPtr<class UProgressBar> HealthBarBuffer;
	
	/**
	 * 대미지를 입기 전의 체력과 대미지를 입은 후의 체력간의 사이를 나타내는 Buffer(임시공간) 값입니다.
	 * 체력이 바로 감소는 것이 아니라, 시각적인 효과를 부드럽게 만들기 위해 사용합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BaseHealthBar")
	float HealthBuffer;

	/** HealthBuffer가 부드럽게 감소하는 보간 속도입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BaseHealthBar")
	float HealthBufferLerpSpeed;

#pragma region DamageableActor
public:
	/** 입력받은 인자로 DamageableActor를 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "BaseHealthBar|DamageableActor")
	void InitializeDamageableActor(TScriptInterface<IInterface_PRDamageable> NewDamageableActor);
	
protected:
	/** 입력받은 오브젝트가 Damageable 인터페이스를 가지고 있는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "BaseHealthBar|DamageableActor")
	bool IsImplementsDamageableInterface(UObject* DamageableObject) const;
	
private:
	/** Damageable 인터페이스를 가지고 있는 액터입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BaseHealthBar|DamageableActor", meta = (AllowPrivateAccess = "true"))
	TScriptInterface<IInterface_PRDamageable> DamageableActor;
#pragma endregion 
};
