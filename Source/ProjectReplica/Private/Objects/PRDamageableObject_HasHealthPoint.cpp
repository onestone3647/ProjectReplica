// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PRDamageableObject_HasHealthPoint.h"
#include "Components/WidgetComponent.h"
#include "Widgets/PRBaseHealthBarWidget.h"

APRDamageableObject_HasHealthPoint::APRDamageableObject_HasHealthPoint()
{
	// Root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Health
	MaxHealth = 100.0f;
	Health = MaxHealth;
	
	// HealthBarWidgetClass
	HealthBarWidgetClass = nullptr;
	
	static ConstructorHelpers::FClassFinder<UUserWidget> W_PRHealthBar_DamageableObject(TEXT("/Game/Blueprints/Widget/HealthBar/W_PRHealthBar_DamageableObject"));
	if(W_PRHealthBar_DamageableObject.Succeeded() == true)
	{
		HealthBarWidgetClass = W_PRHealthBar_DamageableObject.Class;
	}

	// HealthBarWidget
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetDrawSize(FVector2D(140.f, 10.0f));
	if(HealthBarWidgetClass)
	{
		HealthBarWidget->SetWidgetClass(HealthBarWidgetClass);
	}
}

void APRDamageableObject_HasHealthPoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitializeObject();
}

void APRDamageableObject_HasHealthPoint::BeginPlay()
{
	Super::BeginPlay();
	
	// HealthBar를 생성합니다.
	CreateHealthBarWidget();
}

void APRDamageableObject_HasHealthPoint::InitializeObject()
{
	Health = MaxHealth;
}

float APRDamageableObject_HasHealthPoint::GetCurrentHealth_Implementation()
{
	return Health;
}

float APRDamageableObject_HasHealthPoint::GetMaxHealth_Implementation()
{
	return MaxHealth;
}

void APRDamageableObject_HasHealthPoint::CreateHealthBarWidget()
{
	if(HealthBarWidgetClass && HealthBarWidget)
	{
		UPRBaseHealthBarWidget* HealthBarWidgetInstance = CreateWidget<UPRBaseHealthBarWidget>(GetWorld(), HealthBarWidgetClass);
		if(IsValid(HealthBarWidgetInstance))
		{
			// HealthBar의 DamageableActor를 초기화합니다.
			HealthBarWidgetInstance->InitializeDamageableActor(this);
			
			HealthBarWidget->SetWidget(HealthBarWidgetInstance);
		}
	}
}
