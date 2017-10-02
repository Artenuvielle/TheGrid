// Fill out your copyright notice in the Description page of Project Settings.

#include "LifeDisplayRectComponent.h"

UStaticMesh* ULifeDisplayRectComponent::_planeMesh = nullptr;
UMaterial*   ULifeDisplayRectComponent::_blueMaterialOpaque = nullptr;
UMaterial*   ULifeDisplayRectComponent::_orangeMaterialOpaque = nullptr;

ULifeDisplayRectComponent::ULifeDisplayRectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	_currentTransparency = 1.f;
	_transparencyChangePerSecond = 0.f;

	if (!_planeMesh) {
		_planeMesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"), NULL, LOAD_None, NULL);
		_blueMaterialOpaque = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/Geometry/Meshes/robot_material_blue_opaque.robot_material_blue_opaque'"), NULL, LOAD_None, NULL);
		_orangeMaterialOpaque = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/Geometry/Meshes/robot_material_orange_opaque.robot_material_orange_opaque'"), NULL, LOAD_None, NULL);
	}
}

void ULifeDisplayRectComponent::Init(PlayerFaction faction, float width, float height, FVector pos)
{
	UStaticMeshComponent* _planeComponent = NewObject<UStaticMeshComponent>(this);
	_planeComponent->RegisterComponent();
	_planeComponent->SetStaticMesh(_planeMesh);
	_planeMaterial = UMaterialInstanceDynamic::Create(faction == userFaction ? _blueMaterialOpaque : _orangeMaterialOpaque, this);
	_planeMaterial->SetScalarParameterValue(FName("Opacity"), 0.0);
	_planeComponent->SetMaterial(0, _planeMaterial);
	_planeComponent->SetWorldLocation(pos);
	_planeComponent->SetWorldScale3D(FVector::RightVector * width * 0.01 + FVector::ForwardVector * height * 0.01);

	UMaterialInstanceDynamic* _borderMaterial = UMaterialInstanceDynamic::Create(faction == userFaction ? _blueMaterialOpaque : _orangeMaterialOpaque, this);
	_borderMaterial->SetScalarParameterValue(FName("Opacity"), 0.8);
	UStaticMeshComponent* borderLeft = NewObject<UStaticMeshComponent>(this);
	borderLeft->RegisterComponent();
	borderLeft->SetStaticMesh(_planeMesh);
	borderLeft->SetMaterial(0, _borderMaterial);
	borderLeft->SetWorldLocation(pos - FVector::RightVector * width / 2);
	borderLeft->SetWorldScale3D(FVector::RightVector * 3 * 0.01 + FVector::ForwardVector * (height + 3) * 0.01);

	UStaticMeshComponent* borderRight = NewObject<UStaticMeshComponent>(this);
	borderRight->RegisterComponent();
	borderRight->SetStaticMesh(_planeMesh);
	borderRight->SetMaterial(0, _borderMaterial);
	borderRight->SetWorldLocation(pos + FVector::RightVector * width / 2);
	borderRight->SetWorldScale3D(FVector::RightVector * 3 * 0.01 + FVector::ForwardVector * (height + 3) * 0.01);

	UStaticMeshComponent* borderBack = NewObject<UStaticMeshComponent>(this);
	borderBack->RegisterComponent();
	borderBack->SetStaticMesh(_planeMesh);
	borderBack->SetMaterial(0, _borderMaterial);
	borderBack->SetWorldLocation(pos - FVector::ForwardVector * height / 2);
	borderBack->SetWorldScale3D(FVector::RightVector * width * 0.01 + FVector::ForwardVector * 3 * 0.01);

	UStaticMeshComponent* borderNear = NewObject<UStaticMeshComponent>(this);
	borderNear->RegisterComponent();
	borderNear->SetStaticMesh(_planeMesh);
	borderNear->SetMaterial(0, _borderMaterial);
	borderNear->SetWorldLocation(pos + FVector::ForwardVector * height / 2);
	borderNear->SetWorldScale3D(FVector::RightVector * width * 0.01 + FVector::ForwardVector * 3 * 0.01);
}

void ULifeDisplayRectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	_currentTransparency = FMath::Min(1.f, FMath::Max(lifeCounterMaxTransparency, _currentTransparency + _transparencyChangePerSecond * DeltaTime));
	_planeMaterial->SetScalarParameterValue(FName("Opacity"), 1.f - _currentTransparency);
}

void ULifeDisplayRectComponent::setFilled(bool shouldBeFilled)
{
	_transparencyChangePerSecond = ((shouldBeFilled ? lifeCounterMaxTransparency : 1) - _currentTransparency) / lifeCounterTransparencyChangeTime;
	_isFilled = shouldBeFilled;
}

bool ULifeDisplayRectComponent::getFilled()
{
	return _isFilled;
}
