// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "common.h"
#include "LightTrailActor.generated.h"

UCLASS()
class THEGRID_API ALightTrailActor : public AActor
{
	GENERATED_BODY()
	
public:
	ALightTrailActor();
	virtual void Tick(float DeltaTime) override;	
	void Init(PlayerFaction, FVector);
	void addPoint(FVector);
	void setHeadPos(FVector);
	void endTrail();
private:
	float _time;
	bool _shouldEnd;
	void addPointWithoutTest(FVector);
	FVector _lastPointAdded, _secondLastPointAdded, _headPos;
	float _timeOfLastPointAdded;
	TArray<FVector> _points;
	TArray<FVector> _vertices, _normals;
	TArray<FVector2D> _uvs;
	TArray<FProcMeshTangent> _tangents;
	TArray<FLinearColor> _colors;
	TArray<int32> _triangles;

	UProceduralMeshComponent* _mesh;
	static UMaterial* _blueTrailMaterial;
	static UMaterial* _orangeTrailMaterial;
};
