// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "common.h"
#include "LifeDisplayRectComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEGRID_API ULifeDisplayRectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULifeDisplayRectComponent();
	void Init(PlayerFaction, float, float, FVector);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void setFilled(bool);
	bool getFilled();
private:
	bool _isFilled;
	float _currentTransparency;
	float _transparencyChangePerSecond;
	float _lastUpdateTime;

	UMaterialInstanceDynamic* _planeMaterial;
	static UStaticMesh* _planeMesh;
	static UMaterial* _blueMaterialOpaque;
	static UMaterial* _orangeMaterialOpaque;
};
