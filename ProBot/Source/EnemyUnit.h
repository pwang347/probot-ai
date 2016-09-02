/**
	EnemyUnit.h
	A shadow class of Unit to keep track of enemy information when they are not visible

	@author Paul Wang
	@version 9/8/16
*/


#pragma once

#include <BWAPI.h>

namespace ProBot{
	class EnemyUnit {
		int _unitId;
		BWAPI::Position _lastPos;
		BWAPI::UnitType _unitType;
		int _lastHealth;
		int _lastShield;
		int _lastUpdated;

	public:
		/**
			Constructs a null EnemyUnit with id as INT_MAX and unit type set to Unknown.
		*/
		EnemyUnit();
		/**
			Constructs a EnemyUnit using the properties of a given unit.
		*/
		EnemyUnit(BWAPI::Unit);
		/**
			Comparison operator for EnemyUnit.
		*/
		bool operator==(EnemyUnit other);
		/**
			Updates the information using the properties of a given unit.
		*/
		void update(BWAPI::Unit);
		int getUnitId();
		BWAPI::Position getLastKnownPos();
		BWAPI::UnitType getUnitType();
		int getLastKnownHealth();
		int getLastKnownShield();
		int getLastUpdatedFrame();
		int getAssignedDamage();
	};

	// used to check for null instances
	const EnemyUnit NULL_ENEMY = EnemyUnit();
}