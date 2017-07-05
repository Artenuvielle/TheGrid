// Fill out your copyright notice in the Description page of Project Settings.

#include "PvPModeBase.h"

void APvPModeBase::InitGame(const FString & in1, const FString & in2, FString & in3)
{
	UE_LOG(LogTemp, Warning, TEXT("PvP Game initiating..."));
	_client = new Client();
	if (_client->connect("127.0.0.1", 13244)) {
		UE_LOG(LogTemp, Warning, TEXT("connection established"));
	} else {
		UE_LOG(LogTemp, Warning, TEXT("could not connect"));
	}
	Super::InitGame(in1, in2, in3);
}
