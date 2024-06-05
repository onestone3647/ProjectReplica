// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRBaseHealthBarWidget.h"
#include "Interfaces/PRDamageableInterface.h"
#include "Components/ProgressBar.h"

UPRBaseHealthBarWidget::UPRBaseHealthBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// BaseHealthBar
	HealthBar = nullptr;
	HealthBarBuffer = nullptr;
	HealthBuffer = 1.0f;
	HealthBufferLerpSpeed = 0.02f;

	// DamageableTarget
	DamageableTarget = nullptr;
}

void UPRBaseHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HealthBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthBar")));
	HealthBarBuffer = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthBarBuffer")));
}

void UPRBaseHealthBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateHealthBarBuffer();
}

void UPRBaseHealthBarWidget::UpdateHealthBarBuffer()
{
	if(HealthBarBuffer && IsImplementsDamageableInterface(DamageableTarget.GetObject()))
	{
		const float CurrentHealth = DamageableTarget->Execute_GetCurrentHealth(DamageableTarget.GetObject());
		const float MaxHealth = DamageableTarget->Execute_GetMaxHealth(DamageableTarget.GetObject());
		HealthBuffer = FMath::Lerp(HealthBuffer, CurrentHealth / MaxHealth, HealthBufferLerpSpeed);
		HealthBarBuffer->SetPercent(HealthBuffer);
	}
}

float UPRBaseHealthBarWidget::GetHealthBarPercent() const
{
	if(HealthBar && IsImplementsDamageableInterface(DamageableTarget.GetObject()))
	{
		const float CurrentHealth = DamageableTarget->Execute_GetCurrentHealth(DamageableTarget.GetObject());
		const float MaxHealth = DamageableTarget->Execute_GetMaxHealth(DamageableTarget.GetObject());

		return CurrentHealth / MaxHealth;
	}

	return 0.0f;
}

#pragma region DamageableTarget
void UPRBaseHealthBarWidget::InitializeDamageableTarget(TScriptInterface<IPRDamageableInterface> NewDamageableTarget)
{
	DamageableTarget = NewDamageableTarget;
}

bool UPRBaseHealthBarWidget::IsImplementsDamageableInterface(UObject* DamageableObject) const
{
	return IsValid(DamageableObject) && DamageableObject->GetClass()->ImplementsInterface(UPRDamageableInterface::StaticClass());
}
#pragma endregion

