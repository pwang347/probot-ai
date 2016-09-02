/**
	MovementUtil.cpp
	Utility functions for movement-based calculations

	@author Paul Wang
	@version 9/8/16
*/

#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <BWAPI.h>
#include "Debug.h"
#include "CombatUtil.h"

using namespace BWAPI::UnitTypes;

namespace ProBot{
	namespace MovementUtil{
		const double ORBIT_SMOOTHING = 0.4; // how close (radians) before issuing next move command 
		const int ORBIT_DISTANCE_DEFAULT = 300; // default set to probe sight range
		const double ORBIT_ANGLES[] = { M_PI / 8, 3 * M_PI / 8, 5 * M_PI / 8, 7 * M_PI / 8,
			9 * M_PI / 8, 11 * M_PI / 8, 13 * M_PI / 8, 15 * M_PI / 8 }; // angles of an octahedral

		/**
			Returns the unit vector for the unit's facing angle.
		*/
		BWAPI::Point<double, 1> getTravelDirection(BWAPI::Unit unit);
		/**
			Returns the position between two points at a given distance from the first point. 
		*/
		BWAPI::Position getIntermediatePos(BWAPI::Position, BWAPI::Position, int distance);
		/**
			Returns the position at a given distance away from a unit in its facing direction.
		*/
		BWAPI::Position getFleePosition(BWAPI::Unit unit, int distance = 1000);
		/**
			Returns the position at which the attacking unit will be at maximum attack range when attacking the given target.
		*/
		BWAPI::Position getPositionAtMaxAttackRange(BWAPI::Unit, BWAPI::Unit);
		/**
			Returns the next position to travel to in an orbit walk.
			An orbit walk is an octahedral-like move pattern around a target.
		*/
		BWAPI::Position orbitWalk(BWAPI::Unit walker, BWAPI::Unit target, int distance = ORBIT_DISTANCE_DEFAULT);
		/**
			Returns the next position to travel to in an orbit walk.
			An orbit walk is an octahedral-like move pattern around a target.
		*/
		BWAPI::Position orbitWalk(BWAPI::Position walkerPos, BWAPI::Position targetPos, int distance = ORBIT_DISTANCE_DEFAULT);
		double getNextOrbitAngle(double currentAngle);
		/**
			Returns an angle after an operation of mod 2PI.
		*/
		double getModAngle(double angle);
		/**
			Returns relative point (double) based on an angle and distance.
		*/
		BWAPI::Point<double, 1> getPointFromAngleDist(double angle, int distance);
		/**
			Estimates the time it takes for a unit to get to a given position.
		*/
		double estimateTravelTime(BWAPI::Unit, BWAPI::Position);
		double getAngleBetweenPositions(BWAPI::Position, BWAPI::Position);
		BWAPI::Position getXComponent(BWAPI::Position);
		BWAPI::Position getYComponent(BWAPI::Position);
		/**
			Returns a mirrored position given the current map.
		*/
		BWAPI::Position findMirroredLocation(BWAPI::Position);
		/**
			Returns a position that is closer to the AI's starting location for positions that are out of bound.
		*/
		BWAPI::Position validateTowardsBase(BWAPI::Position, BWAPI::Unit);
	}
}