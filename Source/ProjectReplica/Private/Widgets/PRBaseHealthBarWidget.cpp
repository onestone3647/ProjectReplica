// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRBaseHealthBarWidget.h"
#include "Interfaces/Interface_PRDamageable.h"
#include "Components/ProgressBar.h"

UPRBaseHealthBarWidget::UPRBaseHealthBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// BaseHealthBar
	HealthBar = nullptr;
	HealthBarBuffer = nullptr;
	HealthBuffer = 1.0f;
	HealthBufferLerpSpeed = 0.02f;

	// DamageableActor
	DamageableActor = nullptr;
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
	if(HealthBarBuffer && IsImplementsDamageableInterface(DamageableActor.GetObject()))
	{
		PR_LOG_SCREEN("buffer true");

		const float CurrentHealth = DamageableActor->Execute_GetCurrentHealth(DamageableActor.GetObject());
		const float MaxHealth = DamageableActor->Execute_GetMaxHealth(DamageableActor.GetObject());
		HealthBuffer = FMath::Lerp(HealthBuffer, CurrentHealth / MaxHealth, HealthBufferLerpSpeed);
		HealthBarBuffer->SetPercent(HealthBuffer);
	}
	else
	{
		PR_LOG_SCREEN("buffer false");
	}
}

float UPRBaseHealthBarWidget::GetHealthBarPercent() const
{
	if(HealthBar && IsImplementsDamageableInterface(DamageableActor.GetObject()))
	{
		PR_LOG_SCREEN("Bar true");
		const float CurrentHealth = DamageableActor->Execute_GetCurrentHealth(DamageableActor.GetObject());
		const float MaxHealth = DamageableActor->Execute_GetMaxHealth(DamageableActor.GetObject());

		return CurrentHealth / MaxHealth;
	}
	else
	{
		PR_LOG_SCREEN("Bar false");
	}

	return 0.0f;
}

#pragma region DamageableActor
void UPRBaseHealthBarWidget::InitializeDamageableActor(TScriptInterface<IInterface_PRDamageable> NewDamageableActor)
{
	DamageableActor = NewDamageableActor;
}

bool UPRBaseHealthBarWidget::IsImplementsDamageableInterface(UObject* DamageableObject) const
{
	return IsValid(DamageableObject) && DamageableObject->GetClass()->ImplementsInterface(UInterface_PRDamageable::StaticClass());
}
#pragma endregion

