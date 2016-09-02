/**
	MovementUtil.cpp
	Utility functions for movement-based calculations

	@author Paul Wang
	@version 9/8/16
*/

#include "MovementUtil.h"
#include "Debug.h"

using namespace ProBot;

BWAPI::Point<double, 1> MovementUtil::getPointFromAngleDist(double angle, int distance){
	// multiply x by cos and y by sin
	return BWAPI::Point<double, 1>(std::cos(angle) * distance,
									std::sin(angle) * distance);
}

BWAPI::Point<double, 1> MovementUtil::getTravelDirection(BWAPI::Unit unit){
	// get the unit's facing angle and get the corresponding unit vector
	double angle = unit->getAngle();
	return getPointFromAngleDist(angle, 1);
}

BWAPI::Position MovementUtil::getIntermediatePos(BWAPI::Position walkerPos, BWAPI::Position targetPos, int distance){
	BWAPI::Point<double, 1> delta = BWAPI::Point<double, 1> (targetPos - walkerPos);
	double totalDistance = static_cast<double>(targetPos.getApproxDistance(walkerPos));
	return walkerPos + BWAPI::Position(delta / totalDistance * distance);
}

BWAPI::Position MovementUtil::getFleePosition(BWAPI::Unit unit, int distance){
	return BWAPI::Position(getTravelDirection(unit) * distance);
}

BWAPI::Position MovementUtil::getPositionAtMaxAttackRange(BWAPI::Unit attacker, BWAPI::Unit target){
	double angle = getAngleBetweenPositions(target->getPosition(), attacker->getPosition());
	int weaponRange = BWAPI::Broodwar->self()->weaponMaxRange(CombatUtil::getWeaponUsed(attacker, target));
	return target->getPosition() + getPointFromAngleDist(angle, weaponRange);
}

BWAPI::Position MovementUtil::orbitWalk(BWAPI::Unit walker, BWAPI::Unit target, int distance){
	return orbitWalk(walker->getPosition(), target->getPosition());
}

BWAPI::Position MovementUtil::orbitWalk(BWAPI::Position walkerPos, BWAPI::Position targetPos, int distance){
	// compute angle between walker and target
	// treat target as origin
	double angle = getAngleBetweenPositions(walkerPos, targetPos);

	if (std::abs(angle - getNextOrbitAngle(angle)) < ORBIT_SMOOTHING){
		angle = getNextOrbitAngle(angle);
	}

	BWAPI::Position pos = getPointFromAngleDist(getNextOrbitAngle(angle), distance);
	BWAPI::Position x = BWAPI::Position(pos.x, 0);
	BWAPI::Position y = BWAPI::Position(0, pos.y);

	Debug::drawLine(targetPos + x - y, targetPos);
	return targetPos + x - y;
	// use angle to determine new position
}

double MovementUtil::getAngleBetweenPositions(BWAPI::Position walkerPos, BWAPI::Position targetPos){
	BWAPI::Position delta = walkerPos - targetPos; // vector from target to walker
	double angle = std::atan2(static_cast<double>(-delta.y), static_cast<double>(delta.x));
	if (angle <= 0) { angle += 2 * M_PI; }
	return angle;
}

double MovementUtil::getNextOrbitAngle(double currentAngle){
	int arrLength = sizeof(ORBIT_ANGLES) / sizeof(*ORBIT_ANGLES);
	
	// if angle is < mod angle, then that is our next target
	// ie. if ==, then continue
	for (int i = 0; i < arrLength; i++){
		if (currentAngle < ORBIT_ANGLES[i]){
			//Debug::message("Orbit #" + std::to_string(i));
			return ORBIT_ANGLES[i];
		}
	}
	
	return ORBIT_ANGLES[0];
}

double MovementUtil::getModAngle(double angle){
	double modAngle = angle;
	while (modAngle > 2 * M_PI){
		modAngle -= 2 * M_PI;
	}
	return modAngle;
}

double MovementUtil::estimateTravelTime(BWAPI::Unit unit, BWAPI::Position pos){
	return static_cast<double>(unit->getPosition().getApproxDistance(pos)) / unit->getType().topSpeed();
}

BWAPI::Position MovementUtil::getXComponent(BWAPI::Position pos){
	return BWAPI::Position(pos.x, 0);
}

BWAPI::Position MovementUtil::getYComponent(BWAPI::Position pos){
	return BWAPI::Position(0, pos.y);
}

BWAPI::Position MovementUtil::validateTowardsBase(BWAPI::Position pos, BWAPI::Unit unit){
	if (!pos.isValid() || !unit->hasPath(pos)){
		int distance = pos.getApproxDistance(unit->getPosition());
		return getIntermediatePos(unit->getPosition(), BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()), distance);
	}
	else {
		return pos;
	}
}

BWAPI::Position MovementUtil::findMirroredLocation(BWAPI::Position pos){
	return BWAPI::Position((BWAPI::Broodwar->mapWidth() * TILE_SIZE - pos.x), (BWAPI::Broodwar->mapHeight() * TILE_SIZE - pos.y));
}