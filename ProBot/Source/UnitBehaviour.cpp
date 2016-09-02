/**
	UnitBehaviour.cpp
	Assorted classes representing different unit behaviours for common orders.

	@author Paul Wang
	@version 9/8/16
*/

#include "UnitBehaviour.h"

using namespace ProBot;

MeleeBehaviour::MeleeBehaviour(){}

void MeleeBehaviour::moveOrder(BWAPI::Unit, BWAPI::Position){}

void MeleeBehaviour::attackOrder(BWAPI::Unit melee, BWAPI::Position pos){
	if (!melee)
		return;

	// if there isn't an enemy nearby then just attack-move to the target position
	BWAPI::Unit closestEnemy = BWAPI::Broodwar->getClosestUnit(melee->getPosition(), BWAPI::Filter::IsEnemy);
	if (!closestEnemy || !melee){
		melee->attack(pos);
	}
	else { // else, if not already attacking
		if (!melee || !melee->canAttack() || melee->isStartingAttack() || melee->isAttackFrame())
			return;
		BWAPI::Unit closestEnemy = melee->getClosestUnit(BWAPI::Filter::IsEnemy);
		if (!closestEnemy)
			return;

		if ((CombatUtil::getUnitCooldowns().find(melee->getID()) == CombatUtil::getUnitCooldowns().end())
			|| BWAPI::Broodwar->getFrameCount() > CombatUtil::getUnitCooldowns().at(melee->getID())){
			
			BWAPI::Unit target = CombatUtil::getPriorityTarget(melee);

			if (!target){
				melee->attack(pos);
			}
			else {
				melee->attack(target);
				if (CombatUtil::isInRange(melee, target)){
					CombatUtil::getUnitCooldowns()[melee->getID()] = BWAPI::Broodwar->getFrameCount() + CombatUtil::getWeaponUsed(melee, target).damageCooldown();

				}
				Debug::drawLine(melee->getPosition(), target->getPosition(), Debug::RED);
			}
		}
		else {
			//Debug::message("@@@@@@@Current frame " + std::to_string(BWAPI::Broodwar->getFrameCount()) + "| Cooldown expires at: " + std::to_string(CombatUtil::getUnitCooldowns().at(melee->getID())));
			melee->move(CombatUtil::getSafestPositionNearby(melee, 200));
			//melee->move(MovementUtil::validateTowardsBase(MovementUtil::getPositionAtMaxAttackRange(melee, closestEnemy), melee));
		}
	}
}
void MeleeBehaviour::idleOrder(BWAPI::Unit melee){}

RangedBehaviour::RangedBehaviour(){}

void RangedBehaviour::moveOrder(BWAPI::Unit, BWAPI::Position){}
void RangedBehaviour::attackOrder(BWAPI::Unit ranged, BWAPI::Position pos){
	if (!ranged)
		return;

	// if there isn't an enemy nearby then just attack-move to the target position
	BWAPI::Unit closestEnemy = BWAPI::Broodwar->getClosestUnit(ranged->getPosition(), BWAPI::Filter::IsEnemy);
	if (!closestEnemy || !ranged){
		ranged->attack(pos);
	}
	else { // else, if not already attacking
		if (!ranged || !ranged->canAttack() || ranged->isStartingAttack() || ranged->isAttackFrame())
			return;
		BWAPI::Unit closestEnemy = ranged->getClosestUnit(BWAPI::Filter::IsEnemy);
		if (!closestEnemy)
			return;

		if ((CombatUtil::getUnitCooldowns().find(ranged->getID()) == CombatUtil::getUnitCooldowns().end())
			|| BWAPI::Broodwar->getFrameCount() > CombatUtil::getUnitCooldowns().at(ranged->getID())){

			BWAPI::Unit target = CombatUtil::getPriorityTarget(ranged);

			if (!target){
				ranged->attack(pos);
			}
			else {
				ranged->attack(target);
				if (CombatUtil::isInRange(ranged, target)){
					CombatUtil::getUnitCooldowns()[ranged->getID()] = BWAPI::Broodwar->getFrameCount() + CombatUtil::getWeaponUsed(ranged, target).damageCooldown();
				}
				Debug::drawLine(ranged->getPosition(), target->getPosition(), Debug::RED);
			}
		}
		else {
			ranged->move(CombatUtil::getSafestPositionNearby(ranged, 200));
			//melee->move(MovementUtil::validateTowardsBase(MovementUtil::getPositionAtMaxAttackRange(melee, closestEnemy), melee));
		}
	}
}

void RangedBehaviour::idleOrder(BWAPI::Unit ranged){}

CasterBehaviour::CasterBehaviour(){}

void CasterBehaviour::moveOrder(BWAPI::Unit caster, BWAPI::Position pos){}
void CasterBehaviour::attackOrder(BWAPI::Unit caster, BWAPI::Position pos){
	if (!caster)
		return;
	/*
	//BWAPI::Broodwar->
	//caster->useTech
	// if there isn't an enemy nearby then just attack-move to the target position
	BWAPI::Unit closestEnemy = BWAPI::Broodwar->getClosestUnit(ranged->getPosition(), BWAPI::Filter::IsEnemy);
	if (!closestEnemy || !ranged){
		ranged->attack(pos);
	}
	else { // else, if not already attacking
		if (!ranged || !ranged->canAttack() || ranged->isStartingAttack() || ranged->isAttackFrame())
			return;
		BWAPI::Unit closestEnemy = ranged->getClosestUnit(BWAPI::Filter::IsEnemy);
		if (!closestEnemy)
			return;

		if ((CombatUtil::getUnitCooldowns().find(ranged->getID()) == CombatUtil::getUnitCooldowns().end())
			|| BWAPI::Broodwar->getFrameCount() > CombatUtil::getUnitCooldowns().at(ranged->getID())){

			BWAPI::Unit target = CombatUtil::getPriorityTarget(ranged);

			if (!target){
				ranged->attack(pos);
			}
			else {
				ranged->attack(target);
				if (CombatUtil::isInRange(ranged, target)){
					CombatUtil::getUnitCooldowns()[ranged->getID()] = BWAPI::Broodwar->getFrameCount() + CombatUtil::getWeaponUsed(ranged, target).damageCooldown();
				}
				Debug::drawLine(ranged->getPosition(), target->getPosition(), Debug::RED);
			}
		}
		else {
			ranged->move(CombatUtil::getSafestPositionNearby(ranged, 200));
			//melee->move(MovementUtil::validateTowardsBase(MovementUtil::getPositionAtMaxAttackRange(melee, closestEnemy), melee));
		}
	}*/
}

void CasterBehaviour::idleOrder(BWAPI::Unit ranged){}

ScoutBehaviour::ScoutBehaviour(){}

void ScoutBehaviour::moveOrder(BWAPI::Unit scout, BWAPI::Position pos){
	if (!scout){
		return;
	}
	Debug::drawUnitSightRange(scout);

	//if there are no enemies that can kill the scout in sight
	if (!CombatUtil::enemiesInRadius(scout->getPosition(), scout->getType().sightRange() + 150,
		BWAPI::Filter::CanAttack && !BWAPI::Filter::IsWorker)){
		if (scout->getPosition().getApproxDistance(pos) > 1000)
			scout->move(pos, true);
		else 
		scout->move(MovementUtil::orbitWalk(scout->getPosition(), pos));
	}
	else {
		BWAPI::Unit closestEnemy = scout->getClosestUnit(BWAPI::Filter::IsEnemy&&BWAPI::Filter::CanAttack&&!BWAPI::Filter::IsWorker);
		if (!closestEnemy){
			scout->move(pos, true);
		}
		else {
			Debug::message("Flee");
			BWAPI::Position newPos = scout->getPosition() + MovementUtil::getFleePosition(closestEnemy);
			if (scout->hasPath(newPos))
				scout->move(newPos);
			else
				scout->move(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()));

			Debug::drawBoxWithLabel("Closest enemy", closestEnemy->getPosition(), Debug::RED);
		}
	}
}

void ScoutBehaviour::attackOrder(BWAPI::Unit, BWAPI::Position){}

void ScoutBehaviour::idleOrder(BWAPI::Unit scout){}

IUnitBehaviour * ProBot::getUnitBehaviour(BWAPI::UnitType unitType){
	if (CombatUtil::isRangedUnitType(unitType))
		return UNIT_BEHAVIOURS.at(UnitBehaviours::RANGED);
	else if (CombatUtil::isCasterUnitType(unitType))
		return UNIT_BEHAVIOURS.at(UnitBehaviours::CASTER);
	else
		return UNIT_BEHAVIOURS.at(UnitBehaviours::MELEE);
}