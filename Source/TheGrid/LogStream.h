// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <sstream>
#include "CoreMinimal.h"

/**
 * 
 */
class THEGRID_API LogStream : public std::stringbuf {
protected:
	int sync();
};
