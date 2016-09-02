/**
	EnemyUnit.cpp
	A shadow class of Unit to keep track of enemy information when they are not visible

	@author Paul Wang
	@version 9/8/16
*/


#include "EnemyUnit.h"

using namespace ProBot;

void EnemyUnit::update(BWAPI::Unit unit){
	if (unit){
		_unitId = unit->getID();
		_lastPos = unit->getPosition();
		_unitType = unit->getType();
		_lastHealth = unit->getHitPoints();
		_lastShield = unit->getShields();
		_lastUpdated = BWAPI::Broodwar->getFrameCount();
	}
}

bool EnemyUnit::operator==(EnemyUnit other){
	return _unitId == other.getUnitId();
}

EnemyUnit::EnemyUnit(){
	_unitId = INT_MAX;
	_unitType = BWAPI::UnitTypes::Unknown;
}

EnemyUnit::EnemyUnit(BWAPI::Unit unit){
	update(unit);
}

int EnemyUnit::getUnitId(){
	return _unitId;
}
BWAPI::Position EnemyUnit::getLastKnownPos(){
	return _lastPos;
}
BWAPI::UnitType EnemyUnit::getUnitType(){
	return _unitType;
}
int EnemyUnit::getLastKnownHealth(){
	return _lastHealth;
}
int EnemyUnit::getLastUpdatedFrame(){
	return _lastUpdated;
}