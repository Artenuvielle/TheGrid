// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimationActor.h"

UStaticMesh* AAnimationActor::_planeMesh = nullptr;

AAnimationActor::AAnimationActor() {
	if (!_planeMesh) {
		_planeMesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"), NULL, LOAD_None, NULL);
	}
}

void AAnimationActor::createAnimationWithNormal(UMaterialInterface* material, float size, FVector position, FVector normal)
{
	createAnimationWithNormal(material, size, position + normal, FQuat::FindBetweenNormals(FVector::UpVector, normal));
}

void AAnimationActor::createAnimationWithNormal(UMaterialInterface* material, float size, FVector position, FQuat rotation)
{
	UStaticMeshComponent* collisionPlane = NewObject<UStaticMeshComponent>(this);
	collisionPlane->RegisterComponent();
	collisionPlane->SetStaticMesh(_planeMesh);
	collisionPlane->SetMaterial(0, material);
	collisionPlane->SetWorldLocation(position);
	collisionPlane->SetWorldRotation(rotation);
	collisionPlane->SetWorldScale3D(FVector::RightVector * size * 0.01 + FVector::ForwardVector * size * 0.01);
}
