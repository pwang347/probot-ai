/**
	CombatUtil.cpp
	Heuristics and utility functions used for combat and tech decision-making.

	@author Paul Wang
	@version 9/8/16
*/

#include "CombatUtil.h"
#include "Debug.h"

using namespace ProBot;

int CombatUtil::evaluateDynamicAdvantage(BWAPI::Unit attacker, BWAPI::Unit target){
	// give lowest possible score if invalid attacker or target
	if (!attacker || !target)
		return INT_MIN;

	int score = 0;

	// check if enemy can be killed in next attack
	score += (canKill(attacker, target)) ? CAN_KILL_WEIGHT : 0;

	// check cost difference to see if it's worth sacrificing the attacking unit
	score += (canKill(target, attacker)) ? (getCostScore(target->getType()) - getCostScore(attacker->getType())) : 0;
	
	score -= target->getHitPoints() + target->getShields(); // how much health does the enemy have left?
	score += attacker->getHitPoints() + attacker->getShields(); // how confident is the attacker with its remaining hp?
	
	// check travel time needed to get in range (assuming that minRange is always zero)
	score -=  static_cast<int>(static_cast<double>(getDistanceToAttack(attacker, target))// distance
		     / attacker->getType().topSpeed()) // speed
			 * SAFETY_DISTANCE_WEIGHT;

	return score;
}

int CombatUtil::evaluateStaticAdvantage(BWAPI::UnitType myUnitType, BWAPI::UnitType targetType){
	int score = 0;

	// check if my unit can detect a cloakable target
	if (isInvisibleThreat(targetType)){
		score += (myUnitType.isDetector()) ? DETECTOR_ADVANTAGE : 0;
	}

	if (myUnitType.canAttack()){
		// check if target deals no damage to my unit
		bool enemyIsDefenseless = cannotRetaliate(targetType, myUnitType);
		score += enemyIsDefenseless ? SAFETY_DEFENSELESS_WEIGHT : 0;

		// check if my unit is defenseless against an aggressive target (hard counter)
		if (cannotRetaliate(myUnitType, targetType) && !enemyIsDefenseless){
			return -SAFETY_DEFENSELESS_WEIGHT;
		}

		// check weapon effectiveness for my unit attacking the target
		score += static_cast<int>(getSizeModFactor(getWeaponUsed(myUnitType, targetType),
			targetType.size() * WEAPON_EFFECTIVENESS_WEIGHT));

		// check mobility advantage of my unit over the target
		score += static_cast<int>(myUnitType.topSpeed() - targetType.topSpeed()) * 3
				+ (myUnitType.acceleration() - targetType.acceleration());
	}

	return score;
}

int CombatUtil::evaluateTarget(BWAPI::Unit attacker, BWAPI::Unit target){
	// give lowest possible score if invalid attacker or target
	if (!attacker || !target)
		return INT_MIN;

	// give lowest possible score if unit is one of the units to be ignored
	for (auto & unit : IGNORED_TARGETS)
		if (target->getType() == unit)
			return INT_MIN;

	// give lowest possible score if we haven't detected the target
	if (!target->isDetected())
		return INT_MIN;

	// give lowest possible score if we can't reach the target
	if (!target->hasPath(target))
		return INT_MIN;
	
	// high priority to attack drop ships and bunkers, if we can attack it
	if (target->getLoadedUnits().size() > 0){
		if (getWeaponUsed(attacker, target) != BWAPI::WeaponTypes::None)
		return CONTAINER_BASE - target->getHitPoints() - attacker->getPosition().getApproxDistance(target->getPosition());
	}

	if ((target->getType().canAttack() && !target->getType().isWorker())){ // target is a combat unit
		BWAPI::WeaponType weapon = getWeaponUsed(attacker, target);
		if (weapon.maxRange() > RANGED_UNIT_CUTOFF) // ranged units should just target closest
			return RANGED_BASE - attacker->getPosition().getApproxDistance(target->getPosition()) * 10;
		else 
			return MELEE_BASE + evaluateDynamicAdvantage(attacker, target) * DYNAMIC_WEIGHT + evaluateStaticAdvantage(attacker->getType(), target->getType() * STATIC_WEIGHT);
	}
	else if (target->getType().isWorker()){ // workers should never be prioritized over fighting units
		return WORKER_BASE - target->getHitPoints() - attacker->getPosition().getApproxDistance(target->getPosition());
	}
	else{ // for everything else just prioritize based on distance and health
		return MISC_BASE - target->getHitPoints() - attacker->getPosition().getApproxDistance(target->getPosition());
	}
}

int evaluateSpellTarget(BWAPI::TechType tech){
	// @TODO
	return 0;
}

int CombatUtil::getDistanceToAttack(BWAPI::Unit attacker, BWAPI::Unit target){
	return attacker->getPosition().getApproxDistance(target->getPosition())
		- getWeaponUsed(attacker, target).maxRange();
}

int CombatUtil::getCostScore(BWAPI::UnitType unitType){
	return unitType.mineralPrice() * COST_MINERAL_WEIGHT
		+ unitType.gasPrice() * COST_GAS_WEIGHT;
}

bool CombatUtil::cannotRetaliate(BWAPI::UnitType targetType, BWAPI::UnitType attackerType){
	return (getWeaponUsed(targetType, attackerType) == BWAPI::WeaponTypes::None)
		&& !(targetType.getID() == Protoss_Carrier.getID()) && !(targetType.getID() == Protoss_Reaver && !attackerType.isFlyer());
}

BWAPI::WeaponType CombatUtil::getWeaponUsed(BWAPI::Unit attacker, BWAPI::Unit target){
	return getWeaponUsed(attacker->getType(), target->getType());
}

BWAPI::WeaponType CombatUtil::getWeaponUsed(BWAPI::UnitType attackerType, BWAPI::UnitType targetType){
	if (targetType.isFlyer())
		return attackerType.airWeapon();
	else
		return attackerType.groundWeapon();
}

float CombatUtil::getSizeModFactor(BWAPI::WeaponType weapon, BWAPI::UnitSizeType targetSize){
	return damageMultipliers[weapon.damageType().getID()][targetSize.getID()];
}

bool CombatUtil::canAttackAir(BWAPI::Unit unit)
{
	return unit->getType().airWeapon() != BWAPI::WeaponTypes::None;
}

bool CombatUtil::canAttackGround(BWAPI::Unit unit)
{
	return unit->getType().groundWeapon() != BWAPI::WeaponTypes::None;
}

bool CombatUtil::canKill(BWAPI::Unit attacker, BWAPI::Unit target){
	return (BWAPI::Broodwar->getDamageFrom(attacker->getType(), target->getType(), attacker->getPlayer(), target->getPlayer()) >= target->getHitPoints() + target->getShields() + HEAL_BUFFER);
}

bool CombatUtil::enemiesInRadius(BWAPI::Unit unit, int radius, const BWAPI::UnitFilter & pred){
	return unit->getUnitsInRadius(radius, BWAPI::Filter::IsEnemy && pred).size()>0;
}

bool CombatUtil::enemiesInRadius(BWAPI::Position position, int radius, const BWAPI::UnitFilter & pred){
	return BWAPI::Broodwar->getUnitsInRadius(position, radius, BWAPI::Filter::IsEnemy && pred).size()>0;
}

BWAPI::Unit CombatUtil::getClosestEnemyInSight(BWAPI::Unit unit, const BWAPI::UnitFilter & pred){
	return unit->getClosestUnit(BWAPI::Filter::IsEnemy && pred, unit->getType().sightRange());
}

BWAPI::Unit CombatUtil::getPriorityTarget(BWAPI::Unit attacker){
	BWAPI::Unit airPriority = nullptr;
	if (attacker->getType().airWeapon() != BWAPI::WeaponTypes::None){
		airPriority = getPriorityFromTargets(attacker, BWAPI::Broodwar->getUnitsInRadius(attacker->getPosition(),
			attacker->getType().airWeapon().maxRange() + RANGE_BUFFER,
			BWAPI::Filter::IsEnemy && BWAPI::Filter::IsFlyer));
	}
	BWAPI::Unit groundPriority = nullptr;
	if (attacker->getType().groundWeapon() != BWAPI::WeaponTypes::None){
		groundPriority = getPriorityFromTargets(attacker, BWAPI::Broodwar->getUnitsInRadius(attacker->getPosition(),
			attacker->getType().groundWeapon().maxRange() + RANGE_BUFFER,
			BWAPI::Filter::IsEnemy && !BWAPI::Filter::IsFlyer));
	}
	if (!airPriority) // air is null
		return groundPriority;
	else if (!groundPriority) // air is non-null, ground is null
		return airPriority;
	else // both non-null
		return getPriorityFromPair(attacker, airPriority, groundPriority);
}

BWAPI::Unit CombatUtil::getPriorityFromPair(BWAPI::Unit attacker, BWAPI::Unit unit1, BWAPI::Unit unit2){
	return (CombatUtil::evaluateTarget(attacker, unit1) >= CombatUtil::evaluateTarget(attacker, unit2)) ? unit1 : unit2;
}

BWAPI::Unit CombatUtil::getPriorityFromTargets(BWAPI::Unit attacker, BWAPI::Unitset targets){
	if (targets.size() <= 0)
		return nullptr;
	int maxVal = -10000;
	BWAPI::Unit bestUnit;
	for (auto & target : targets){
		int evaluation = CombatUtil::evaluateTarget(attacker, target);
		if (evaluation > maxVal){
			maxVal = evaluation;
			bestUnit = target;
		}
	}
	return bestUnit;
}

BWAPI::Unitset CombatUtil::getAttackersInRange(BWAPI::Unit target){
	BWAPI::Unitset units = target->getUnitsInRadius(LONGEST_ATTACK_RANGE, BWAPI::Filter::IsEnemy);
	for (auto & unit : units)
		if (!unit->isInWeaponRange(target))
			units.erase(unit);
	return units;
}

int CombatUtil::getStaticUnitScore(BWAPI::UnitType unitType){
	int score = 0;
	score += unitType.maxHitPoints();
	score += unitType.maxShields();
	score += unitType.groundWeapon().damageAmount();
	score += unitType.airWeapon().damageAmount();
	score += static_cast<int>(unitType.topSpeed() * 10.0);
	return score;
}
int CombatUtil::getUnitScore(BWAPI::Unit unit){
	// @TODO
	return 0;
}

std::unordered_map<int, int> & CombatUtil::getUnitCooldowns(){
	return _unitAttackCooldown;
}

bool CombatUtil::isInRange(BWAPI::Unit attacker, BWAPI::Unit target){
	BWAPI::WeaponType weapon = getWeaponUsed(attacker, target);
	int distance = attacker->getPosition().getApproxDistance(target->getPosition());
	return (distance <= weapon.maxRange()
		&& distance >= weapon.minRange());
}

int CombatUtil::estimateMaxDPSAtPos(BWAPI::Position pos, BWAPI::Unit unit){
	int eval = 0;
	BWAPI::Unitset enemies = BWAPI::Broodwar->getUnitsInRadius(pos, 1000, BWAPI::Filter::IsEnemy && BWAPI::Filter::CanAttack);
	for (auto & enemy : enemies)
		if (enemy->isInWeaponRange(unit))
			eval += BWAPI::Broodwar->getDamageFrom(enemy->getType(), unit->getType(), enemy->getPlayer(), unit->getPlayer());
	return eval;
}

BWAPI::Position CombatUtil::getSafestPositionNearby(BWAPI::Unit unit, int distance){
	int minVal = INT_MAX;
	BWAPI::Position bestPos;
	for (auto & angle : MovementUtil::ORBIT_ANGLES){
		BWAPI::Position pos = MovementUtil::getPointFromAngleDist(angle, distance);
		int score = estimateMaxDPSAtPos(pos, unit);
		if (score < minVal){
			minVal = score;
			bestPos = pos;
		}
	}
	return bestPos;
}

std::vector<BWAPI::TechType> CombatUtil::getAbilities(BWAPI::Unit unit){
	using namespace BWAPI::TechTypes;

	std::vector<BWAPI::TechType> abilities;
	if (unit->getType().getID() == Protoss_High_Templar.getID()){
		if (BWAPI::Broodwar->self()->hasResearched(Psionic_Storm))
			abilities.push_back(Psionic_Storm);
		if (BWAPI::Broodwar->self()->hasResearched(Hallucination))
			abilities.push_back(Hallucination);
	}
	else if (unit->getType().getID() == Protoss_Dark_Archon.getID()){
		if (BWAPI::Broodwar->self()->hasResearched(Maelstrom))
			abilities.push_back(Maelstrom);
		if (BWAPI::Broodwar->self()->hasResearched(Mind_Control))
			abilities.push_back(Mind_Control);
	}
	else if (unit->getType().getID() == Protoss_Arbiter.getID()){
		if (BWAPI::Broodwar->self()->hasResearched(Stasis_Field))
			abilities.push_back(Stasis_Field);
		if (BWAPI::Broodwar->self()->hasResearched(Recall))
			abilities.push_back(Recall);
	}
	return abilities;
}

bool CombatUtil::isInvisibleThreat(BWAPI::UnitType type){
	if (type == Protoss_Observer
		|| type == Protoss_Dark_Templar
		|| type == Protoss_Arbiter
		|| type == Protoss_Arbiter
		|| type == Terran_Wraith
		|| type == Terran_Ghost
		|| type == Zerg_Lurker)
		return true;
	else
		return false;
}

bool CombatUtil::isRangedUnitType(BWAPI::UnitType unitType){
	if (unitType.groundWeapon() != BWAPI::WeaponTypes::None)
		return unitType.groundWeapon().maxRange() >= RANGED_UNIT_CUTOFF;
	else if (unitType.airWeapon() != BWAPI::WeaponTypes::None)
		return unitType.airWeapon().maxRange() >= RANGED_UNIT_CUTOFF;
	else 
		return false;
}

bool CombatUtil::isCasterUnitType(BWAPI::UnitType unitType){
	if (unitType == Protoss_High_Templar
		|| unitType == Protoss_Dark_Archon
		|| unitType == Protoss_Arbiter)
		return true;
	else
		return false;
}