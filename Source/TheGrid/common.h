#ifndef _Common_H_
#define _Common_H_

enum PlayerFaction {
	PLAYER_FACTION_BLUE = 0,
	PLAYER_FACTION_ORANGE
};

const PlayerFaction userFaction = PLAYER_FACTION_BLUE;
const PlayerFaction enemyFaction = (userFaction == PLAYER_FACTION_BLUE ? PLAYER_FACTION_ORANGE : PLAYER_FACTION_BLUE);

const float WALL_X_MAX = 135;
const float WALL_X_MIN = -135;
const float WALL_Y_MAX = 270;
const float WALL_Y_MIN = 0;
const float WALL_Z_MAX = 135;
const float WALL_Z_MIN = -945;

const float WALL_X_MID = (WALL_X_MAX + WALL_X_MIN) / 2;
const float WALL_Y_MID = (WALL_Y_MAX + WALL_Y_MIN) / 2;
const float WALL_Z_MID = (WALL_Z_MAX + WALL_Z_MIN) / 2;
const float WALL_Z_DIFF = WALL_Z_MAX - WALL_Z_MIN;

const float PLAYER_HEAD_SIZE = 15;
const float PLAYER_TORSO_HEAD_OFFSET = 10;
const float PLAYER_GEOMETRY_SCALE = 2;

const int lifeCounterMaxLife = 4;
const float lifeCounterMaxTransparency = 0.3f;
const float lifeCounterTransparencyChangeTime = 0.2f; // s

const float diskRadius = 15; // cm
const float diskHeight = 3; // cm
const float diskSpeed = 0.5f; // cm/ms
const float diskAxisRotationFactor = 100;
const float diskRotationTimeAfterCollision = 0.3f; // s
const float diskEnemyMomentumAttractionFactor = 1.8f; // deg/sec
const float diskOwnerMomentumAttractionFactor = 2.0f; // deg/sec
const float diskMinimalAxialRotationAfterCollision = 0.0004f; // deg/millisec

const float shieldMinimumRadius = 2.5; // cm
const float shieldMaximumRadius = 20; // cm
const float shieldGrowStartDistance = 400; // cm
const float shieldGrowEndDistance = 250; // cm
const int shieldMaximumCharges = 3;

const FVector aiDefaultHeadPosition = FVector(0.0, 165.0, -810.0);
const float aiHeadMaxSpeed = 25.f; // cm/s
const float aiHeadMaxRotation = 0.8f; // rad/s
const float aiArmMaxSpeed = 50.f; // cm/s
const float aiArmMaxRotation = 3.5f; // rad/s
const float aiMinTimeUntilAttack = 2.f; // s
const float aiDefendArmTorsoDistance = 35.f; // cm
const float aiCatchArmTorsoDistance = 50.f; // cm

const float collisionAnimationSize = 150; // in cm
const float scoreAnimationSize = 1200; // in cm

const int lightTrailMaxPoints = 70;
const float lightTrailPointsPerSecond = 30.f;
const float lightTrailSizeGrow = 100.f;
const float lightTrailMaxSize = 1.5f;
const float lightTrailInputPointMinDistance = 20.f;

extern bool gameRunning;
extern float elapsedTime;

enum GameNotifications {
	GAME_NOTIFICATION_PLAYER_CHANGED_LIFE = 0,
	GAME_NOTIFICATION_PLAYER_CHANGED_SHIELD_CHARGE,
	GAME_NOTIFICATION_DISK_STATE_CHANGED,
	GAME_NOTIFICATION_DISK_THROWN
};

#endif