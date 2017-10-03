// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AnimationActor.generated.h"

UCLASS()
class THEGRID_API AAnimationActor : public AActor
{
	GENERATED_BODY()

public:
	AAnimationActor();
protected:
	void createAnimationWithNormal(UMaterialInterface*, float, FVector, FVector);
	void createAnimationWithNormal(UMaterialInterface*, float, FVector, FQuat);
private:
	static UStaticMesh* _planeMesh;
};
