// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PRAICharacter_General.h"
#include "Components/WidgetComponent.h"
#include "Widgets/PRBaseHealthBarWidget.h"

APRAICharacter_General::APRAICharacter_General()
{
	// HealthBarWidgetClass
	HealthBarWidgetClass = nullptr;
	static ConstructorHelpers::FClassFinder<UUserWidget> W_PRHealthBar_General(TEXT("/Game/Blueprints/Widget/HealthBar/W_PRHealthBar_General"));
	if(W_PRHealthBar_General.Succeeded() == true)
	{
		HealthBarWidgetClass = W_PRHealthBar_General.Class;
	}

	// HealthBarWidget
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetMesh());
	HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetDrawSize(FVector2D(140.f, 10.0f));
	if(HealthBarWidgetClass)
	{
		HealthBarWidget->SetWidgetClass(HealthBarWidgetClass);
	}
}

#pragma region HealthBar
void APRAICharacter_General::CreateHealthBarWidget()
{
	Super::CreateHealthBarWidget();
	
	if(HealthBarWidgetClass && HealthBarWidget)
	{
		UPRBaseHealthBarWidget* HealthBarWidgetInstance = CreateWidget<UPRBaseHealthBarWidget>(GetWorld(), HealthBarWidgetClass);
		if(IsValid(HealthBarWidgetInstance))
		{
			// HealthBar의 DamageableTarget을 초기화합니다.
			HealthBarWidgetInstance->InitializeDamageableTarget(this);
			
			HealthBarWidget->SetWidget(HealthBarWidgetInstance);
		}
	}
}
#pragma endregion 
