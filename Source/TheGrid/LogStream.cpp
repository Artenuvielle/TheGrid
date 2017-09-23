// Fill out your copyright notice in the Description page of Project Settings.

#include "LogStream.h"

int LogStream::sync() {
	UE_LOG(LogTemp, Log, TEXT("%s"), *FString(str().c_str()));
	str("");
	return std::stringbuf::sync();
}
