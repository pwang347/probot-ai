/**
	CombatUtil.h
	Heuristics and utility functions used for combat and tech decision-making.

	@author Paul Wang
	@version 9/8/16
*/

#pragma once

#include <BWAPI.h>
#include "Common.h"
#include "Debug.h"
#include "MovementUtil.h"

using namespace BWAPI::UnitTypes;

namespace ProBot{
	namespace CombatUtil{
		// arbitrary / modifiable values assigned to factors relevant to the heuristic evaluation 
		const int CAN_KILL_WEIGHT = 10000;
		const int COST_GAS_WEIGHT = 2;
		const int COST_MINERAL_WEIGHT = 1;
		const int WEAPON_EFFECTIVENESS_WEIGHT = 1000;
		const int SAFETY_DISTANCE_WEIGHT = 5;
		const int SAFETY_DEFENSELESS_WEIGHT = 1000;
		const int HP_SHIELD_WEIGHT = 2;
		const int DETECTOR_ADVANTAGE = 10000;

		// base evaluation values for unit types
		const int MELEE_BASE = 1000;
		const int RANGED_BASE = 10000;
		const int CONTAINER_BASE = 5000;
		const int WORKER_BASE = 500;
		const int MISC_BASE = 0;

		// coefficients for relative weight of different factors
		const int DYNAMIC_WEIGHT = 2; // heavier weight since actual battle conditions should be more important in decision-making
		const int STATIC_WEIGHT = 1;

		// if a unit is healing, then we shouldn't assume that we can kill it even if we have lethal
		const int HEAL_BUFFER = 3;
		// longest attack range in the game
		const int LONGEST_ATTACK_RANGE = 384;
		// the furthest possible target (outside of attack range)
		const int RANGE_BUFFER = 1000;
		// the minimum range for a unit to be considered a ranged unit
		const int RANGED_UNIT_CUTOFF = 100;

		// matrix representing multiplier corresponding to given damage type x size type
		const int NUM_DAMAGE_TYPE = 7;
		const int NUM_SIZE_TYPE = 6;
		const float damageMultipliers[NUM_DAMAGE_TYPE][NUM_SIZE_TYPE] =
		{
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, .5f, .75f, 1, 0, 0 },
			{ 0, 1, .5f, .25, 0, 0 },
			{ 0, 1, 1, 1, 0, 0 },
			{ 0, 1, 1, 1, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 }
		};

		// units that should not be considered for target selection
		const std::vector<BWAPI::UnitType> IGNORED_TARGETS = { Zerg_Egg, Zerg_Larva };

		static std::unordered_map<int, int> _unitAttackCooldown;
		/**
			Returns the weapon used by the attacker for a given target (air or ground).
		*/
		BWAPI::WeaponType getWeaponUsed(BWAPI::Unit attacker, BWAPI::Unit target);
		/**
			Returns the weapon used by the attacker for a given target (air or ground).
		*/
		BWAPI::WeaponType getWeaponUsed(BWAPI::UnitType attackerType, BWAPI::UnitType targetType);
		/**
			Returns the damage multiplier corresponding to given weapon and unit size.
		*/
		float getSizeModFactor(BWAPI::WeaponType weapon, BWAPI::UnitSizeType targetSize);

		std::vector<BWAPI::TechType> getAbilities(BWAPI::Unit);
		/**
			Returns true if unit can attack air units.
		*/
		bool canAttackAir(BWAPI::Unit unit);
		/**
			Returns true if unit can attack ground units.
		*/
		bool canAttackGround(BWAPI::Unit unit);
		/**
			Returns true if an attacker will deal damage  >= target hit points with an additional buffer value for healing.
		*/
		bool canKill(BWAPI::Unit, BWAPI::Unit);
		/**
			Estimates the inherent advantage of a unit type over another.

			@param myUnitType - the unit type being evaluated
			@param targetType - the unit type used as comparison
			@return sum of all factors involved
		*/
		int evaluateTarget(BWAPI::Unit, BWAPI::Unit);
		/**
			Estimates the military value for a unit attacking a target.
			ASSUMPTION: target is a combat unit used for offense

			@param attacker - the attacking unit
			@param target - the target unit
			@return sum of all factors involved
		*/
		int evaluateDynamicAdvantage(BWAPI::Unit, BWAPI::Unit);
		/**
			Estimates the inherent advantage of a unit type over another.

			@param myUnitType - the unit type being evaluated
			@param targetType - the unit type used as comparison
			@return sum of all factors involved
		*/
		int evaluateStaticAdvantage(BWAPI::UnitType, BWAPI::UnitType);
		int estimateMaxDPSAtPos(BWAPI::Position, BWAPI::Unit);
		/**
			Returns weighted values of a unit's mineral and gas price.
		*/
		int getCostScore(BWAPI::UnitType);

		/**
			Returns the distance between two units minus the max range of the attacker.
		*/
		int getDistanceToAttack(BWAPI::Unit, BWAPI::Unit);
		/**
			Returns an estimate of a unit's base value given its unit type.
		*/
		int getStaticUnitScore(BWAPI::UnitType);
		/**
			Returns an estimate of a unit's base value given its current properties.
		*/
		int getUnitScore(BWAPI::Unit);
		/**
			Returns true if the target has no weapon against the attacker.
		*/
		bool cannotRetaliate(BWAPI::UnitType, BWAPI::UnitType);
		/**
			Returns true if there are enemies in the radius of a given unit.
		*/
		bool enemiesInRadius(BWAPI::Unit, int, const BWAPI::UnitFilter &pred = nullptr);
		/**
			Returns true if there are enemies in the radius of a given unit.
		*/
		bool enemiesInRadius(BWAPI::Position, int, const BWAPI::UnitFilter &pred = nullptr);
		/**
			Returns the closest enemy in a unit's sight range.
		*/
		BWAPI::Unit getClosestEnemyInSight(BWAPI::Unit unit, const BWAPI::UnitFilter & pred = nullptr);
		/**
			Determines the highest priority target for an attacker based on calls to evaluateTarget

			@param attacker - the unit attacking
			@return the highest priority target
		*/
		BWAPI::Unit getPriorityTarget(BWAPI::Unit attacker);
		/**
			Returns the higher priority target between two possible units
		*/
		BWAPI::Unit getPriorityFromPair(BWAPI::Unit attacker, BWAPI::Unit unit1, BWAPI::Unit unit2);
		/**
			Returns the highest priority target from a list of targets
		*/
		BWAPI::Unit getPriorityFromTargets(BWAPI::Unit attacker, BWAPI::Unitset targets);
		/**
			Returns a list of possible enemy attackers for a given unit
		*/
		BWAPI::Unitset getAttackersInRange(BWAPI::Unit target);
		std::unordered_map<int, int> & getUnitCooldowns();
		bool isInRange(BWAPI::Unit, BWAPI::Unit);
		BWAPI::Position getSafestPositionNearby(BWAPI::Unit, int);
		bool isInvisibleThreat(BWAPI::UnitType type);
		bool isRangedUnitType(BWAPI::UnitType);
		bool isCasterUnitType(BWAPI::UnitType);
	}
}