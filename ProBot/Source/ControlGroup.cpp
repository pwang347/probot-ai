/**
	ControlGroup.cpp
	A control group for delegating individual unit tasks, such as invading and scouting.

	@author Paul Wang
	@version 9/8/16
*/

#include "ControlGroup.h"

using namespace ProBot;

bool ControlGroup::operator==(ControlGroup other){
	return other.getUnits() == getUnits();
}

ControlGroup::ControlGroup(const BWAPI::Unitset& units, Flag flag){
	_controlUnits = units;
	_currentStrategy = flag;
}

void ControlGroup::validateUnits(){
	for (auto & unit : _controlUnits)
		if (!unit || unit->getType().getID() == BWAPI::UnitTypes::Unknown.getID() || !unit->getPosition().isValid() || !unit->exists() || unit->getHitPoints() <= 0)
			_controlUnits.erase(unit);
}

void ControlGroup::moveAsGroup(BWAPI::Position pos){
	int greatestDistance = INT_MIN;
	BWAPI::Unit laggingUnit;
	_isInCombat = false;
	for (auto & unit : _controlUnits){
		// check if there's an enemy in sight range or a unit is under attack
		// if so, we should be in combat mode
		if ((unit->getClosestUnit(BWAPI::Filter::IsEnemy, unit->getType().sightRange()) && !_isInCombat)
			|| unit->isUnderAttack()){
			_isInCombat = true;
			break;
		}
		// continue searching for the lagging unit in the group
		int dist = unit->getPosition().getApproxDistance(pos);
		if (dist > greatestDistance){
			greatestDistance = dist;
			laggingUnit = unit;
		}
	}
	// if there are no enemies nearby
	if (!_isInCombat)
		for (auto & unit : _controlUnits)
			// for each unit, if distance > MAX_LAGGING DISTANCE make it follow lagging unit
			if (std::abs(unit->getPosition().getApproxDistance(pos)
				- laggingUnit->getPosition().getApproxDistance(pos)) > MAX_LAGGING_DISTANCE
				&& !unit->isStuck() && !unit->isBraking())
				if (unit != laggingUnit)
					unit->follow(laggingUnit);
				// make sure the lagging unit continues to move toward target
				else
					getUnitBehaviour(unit->getType())->moveOrder(unit, pos);
			else 
				getUnitBehaviour(unit->getType())->moveOrder(unit, pos);
	else // if we are under attack, then issue attack-move orders instead
		for (auto & unit : _controlUnits)
			getUnitBehaviour(unit->getType())->attackOrder(unit, pos);
}

void ControlGroup::update(KnowledgeBank * knowledgeBank){
	validateUnits();
	switch (_currentStrategy)
	{
		// @TODO
	case Flag::ALL_IN:
		if (knowledgeBank->shouldNotFight())
			overrideFlag(Flag::RETREAT);
		for (auto & unit : _controlUnits)
			getUnitBehaviour(unit->getType())->attackOrder(unit, knowledgeBank->getClosestEnemyBase());
		break;
	case Flag::SCOUT_ENEMY:
		for (auto & unit : _controlUnits)
			UNIT_BEHAVIOURS.at(UnitBehaviours::SCOUT)->moveOrder(unit, knowledgeBank->getClosestEnemyBase());
		break;
	case Flag::RETREAT:
		if (!knowledgeBank->shouldNotFight()
			&& _lastStrategy == Flag::ALL_IN)
			overrideFlag(Flag::ALL_IN);
		for (auto & unit : _controlUnits)
			getUnitBehaviour(unit->getType())->moveOrder(unit, BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()));
		break;
	default:
		for (auto & unit : _controlUnits)
			getUnitBehaviour(unit->getType())->idleOrder(unit);
		break;
	}
}

Flag ControlGroup::getFlag(){
	return _currentStrategy;
}

void ControlGroup::overrideFlag(Flag newFlag){
	_lastStrategy = _currentStrategy;
	_currentStrategy = newFlag;
}

bool ControlGroup::isInCombat(){
	return _isInCombat;
}


void ControlGroup::addUnit(const BWAPI::Unit& unit){
	_controlUnits.insert(unit);
}

void ControlGroup::addUnits(const BWAPI::Unitset& units){
	for (auto & unit : units)
		addUnit(unit);
}

void ControlGroup::removeUnit(const BWAPI::Unit& unit){
	_controlUnits.erase(unit);
}

void ControlGroup::removeUnits(const BWAPI::Unitset& units){
	for (auto & unit : units)
		removeUnit(unit);
}

void ControlGroup::setControlGroup(const BWAPI::Unitset& controlUnits){
	_controlUnits = controlUnits;
}

BWAPI::Unitset ControlGroup::getUnits(){
	return _controlUnits;
}