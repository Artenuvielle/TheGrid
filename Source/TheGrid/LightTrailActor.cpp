// Fill out your copyright notice in the Description page of Project Settings.

#include "LightTrailActor.h"

UMaterial* ALightTrailActor::_blueTrailMaterial = nullptr;
UMaterial* ALightTrailActor::_orangeTrailMaterial = nullptr;

ALightTrailActor::ALightTrailActor()
{
	PrimaryActorTick.bCanEverTick = true;
	_time = 0;
	_headPos = FVector::UpVector * 170;
	_shouldEnd = false;
	_mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = _mesh;
	_points = TArray<FVector>();

	if (!_blueTrailMaterial) {
		_blueTrailMaterial = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/Geometry/Meshes/trail_material_blue.trail_material_blue'"), NULL, LOAD_None, NULL);
		_orangeTrailMaterial = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/Geometry/Meshes/trail_material_orange.trail_material_orange'"), NULL, LOAD_None, NULL);
	}
}

void ALightTrailActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	_time += DeltaTime;
	if (_points.Num() > 0) {
		if (_shouldEnd) {
			addPointWithoutTest(_lastPointAdded);
			if ((_points[0] - _points[_points.Num() - 1]).SizeSquared() < 1.f) {
				Destroy();
				return;
			}
		}
		for (int i = 0; i < lightTrailMaxPoints; i++) {
			FVector direction;
			if (i + 1 < lightTrailMaxPoints) {
				direction = _points[i + 1] - _points[i];
			}
			else {
				direction = _points[i] - _points[i - 1];
			}
			FVector offset = FVector::CrossProduct(direction, _headPos - _points[i]);
			float width = lightTrailMaxSize;
			if (i < lightTrailSizeGrow) {
				width *= ((float)i) / lightTrailSizeGrow;
			}
			offset *= width / offset.Size();
			_vertices[i * 3] = _points[i];
			_vertices[i * 3 + 1] = _points[i] + offset;
			_vertices[i * 3 + 2] = _points[i] - offset;

			FVector n = FVector::CrossProduct(direction, offset);
			n.Normalize();
			_normals[i * 3] = n;
			_normals[i * 3 + 1] = n;
			_normals[i * 3 + 2] = n;

			_tangents[i * 3] = FProcMeshTangent(/*direction, false*/);
			_tangents[i * 3 + 1] = FProcMeshTangent(/*direction, false*/);
			_tangents[i * 3 + 2] = FProcMeshTangent(/*direction, false*/);
		}
		_mesh->ClearAllMeshSections();
		_mesh->CreateMeshSection_LinearColor(0, _vertices, _triangles, _normals, _uvs, _colors, _tangents, false);
		//_mesh->UpdateMeshSection_LinearColor(0, _vertices, _normals, _uvs, _colors, _tangents);
	}
}

void ALightTrailActor::Init(PlayerFaction faction, FVector startPosition)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *startPosition.ToString());
	_vertices = TArray<FVector>();
	_normals = TArray<FVector>();
	_uvs = TArray<FVector2D>();
	_tangents = TArray<FProcMeshTangent>();
	_colors = TArray<FLinearColor>();
	_triangles = TArray<int32>();
	_lastPointAdded = startPosition;
	_secondLastPointAdded = startPosition;
	_timeOfLastPointAdded = _time;
	for (int i = 0; i < lightTrailMaxPoints; i++) {
		_points.Add(startPosition /*+ FVector::ForwardVector * -i*/);
	}
	for (int i = 0; i < lightTrailMaxPoints; i++) {
		FVector direction;
		if (i + 1 < lightTrailMaxPoints) {
			direction = _points[i + 1] - _points[i];
		}
		else {
			direction = _points[i] - _points[i - 1];
		}
		FVector offset = FVector::CrossProduct(direction, _headPos - _points[i]);
		offset *= lightTrailMaxSize / offset.Size();
		_vertices.Add(_points[i]);
		_vertices.Add(_points[i] + offset);
		_vertices.Add(_points[i] - offset);
		FVector n = FVector::CrossProduct(direction, offset);
		n.Normalize();
		_normals.Add(n);
		_normals.Add(n);
		_normals.Add(n);
		_uvs.Add(FVector2D(0.5, 0.5));
		_uvs.Add(FVector2D(0.f, 0.f));
		_uvs.Add(FVector2D(1.f, 1.f));
		_tangents.Add(FProcMeshTangent(direction, false));
		_tangents.Add(FProcMeshTangent(direction, false));
		_tangents.Add(FProcMeshTangent(direction, false));
		_colors.Add(FLinearColor(0.75, 0.75, 0.75, 1.f));
		_colors.Add(FLinearColor(0.75, 0.75, 0.75, 1.f));
		_colors.Add(FLinearColor(0.75, 0.75, 0.75, 1.f));
	}
	for (int i = 1; i < lightTrailMaxPoints; i++) {
		_triangles.Add((i - 1) * 3);
		_triangles.Add((i - 1) * 3 + 1);
		_triangles.Add(i * 3 + 1);

		_triangles.Add((i - 1) * 3);
		_triangles.Add(i * 3 + 1);
		_triangles.Add(i * 3);

		_triangles.Add((i - 1) * 3);
		_triangles.Add(i * 3 + 2);
		_triangles.Add((i - 1) * 3 + 2);

		_triangles.Add((i - 1) * 3);
		_triangles.Add(i * 3);
		_triangles.Add(i * 3 + 2);
	}
	_mesh->CreateMeshSection_LinearColor(0, _vertices, _triangles, _normals, _uvs, _colors, _tangents, false);
	_mesh->SetMaterial(0, (faction == userFaction ? _blueTrailMaterial : _orangeTrailMaterial));
}

void ALightTrailActor::addPoint(FVector position)
{
	FVector direction = position - _lastPointAdded;
	_points.RemoveAt(_points.Num() - 1);
	if (_timeOfLastPointAdded - _time < 1.f / lightTrailPointsPerSecond && direction.SizeSquared() > lightTrailInputPointMinDistance * lightTrailInputPointMinDistance) {
		FVector secondLastDirection = _lastPointAdded - _secondLastPointAdded;
		FVector Q1 = _secondLastPointAdded + secondLastDirection * 0.75;
		FVector R0 = _lastPointAdded + direction * 0.25;
		FVector Q0 = _lastPointAdded + direction * 0.75;
		_points.Add(Q1 + (R0 - Q1) * 0.25);
		_points.Add(Q1 + (R0 - Q1) * 0.75);
		_points.Add(R0 + (Q0 - R0) * 0.25);
		_points.Add(R0 + (Q0 - R0) * 0.75);
		_timeOfLastPointAdded = _time;
		_secondLastPointAdded = _lastPointAdded;
		_lastPointAdded = position;
		while (_points.Num() > lightTrailMaxPoints - 1) {
			_points.RemoveAt(0);
		}
	}
	_points.Add(position);
}

void ALightTrailActor::setHeadPos(FVector headPosition)
{
	_headPos = headPosition;
}

void ALightTrailActor::endTrail()
{
	_shouldEnd = true;
}

void ALightTrailActor::addPointWithoutTest(FVector position)
{
	_points.Add(position);
	_secondLastPointAdded = _lastPointAdded;
	_lastPointAdded = position;
	while (_points.Num() > lightTrailMaxPoints) {
		_points.RemoveAt(0);
	}
}

