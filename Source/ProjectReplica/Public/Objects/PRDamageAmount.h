// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "PRPooledObject.h"
#include "PRDamageAmount.generated.h"

class UPRDamageAmountWidget;
class UWidgetComponent;

/**
 * 대미지를 나타내는 Actor 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRDamageAmount : public APRPooledObject
{
	GENERATED_BODY()
	
public:	
	APRDamageAmount();

protected:
	virtual void BeginPlay() override;

public:
	/**
	 * 초기화하는 함수입니다.
	 *
	 * @param SpawnLocation Spawn할 위치
	 * @param DamageAmount 대미지의 양
	 * @param bIsCritical 일반 대미지인지, 치명타 대미지인지 판별하는 인자
	 * @param ElementType 대미지의 속성
	 */
	UFUNCTION(BlueprintCallable, Category = "DamageAmount")
	void Initialize(FVector SpawnLocation, float DamageAmount, bool bIsCritical, EPRElementType ElementType);

protected:
	/** FadeOut 위젯 애니메이션이 끝났을 때 실행하는 함수입니다. */
	UFUNCTION()
	void OnFadeOutWidgetAnimFinished();

private:
	/** DamageAmount 위젯을 생성하는 함수입니다. */
	class UPRDamageAmountWidget* CreateDamageAmountWidget();

protected:
	/** 이 오브젝트의 RootComponent로 지정할 SceneComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DamageAmount")
	TObjectPtr<class USceneComponent> Root;
	
	/** DamageAmount 위젯의 클래스 레퍼런스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DamageAmount")
	TSubclassOf<class UUserWidget> DamageAmountWidgetClass;

	/** DamageAmount 위젯의 인스턴스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DamageAmount")
	TObjectPtr<class UPRDamageAmountWidget> DamageAmountWidgetInstance;
	
	/** 대미지를 나타내는 WidgetComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DamageAmount")
	TObjectPtr<class UWidgetComponent> DamageAmountWidget;
};
