
#ifndef _Simulation_H_
#define _Simulation_H_

#include "CoreMinimal.h"

struct SimStep {
	float timestamp;
	float triggerPush;
	FVector head_position;
	FQuat head_orientation;
	FVector disk_arm_position;
	FQuat disk_arm_orientation;
	FVector shield_arm_position;
	FQuat shield_arm_orientation;
	SimStep(float t, float tp, FVector hp, FQuat ho, FVector dap, FQuat dao, FVector sap, FQuat sao) :
		timestamp(t),
		triggerPush(tp),
		head_position(hp),
		head_orientation(ho),
		disk_arm_position(dap),
		disk_arm_orientation(dao),
		shield_arm_position(sap),
		shield_arm_orientation(sao) {};
};

extern SimStep getSimulationStep(float currentSimulationTimestamp);
extern void initSimulation();

#endif