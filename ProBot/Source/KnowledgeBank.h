/**
	KnowledgeBank.h
	Keeps track of enemy unit information as well as tech trees for both players

	@author Paul Wang
	@version 9/8/16
*/

#pragma once

#include <BWAPI.h>
#include "EnemyUnit.h"
#include "Debug.h"
#include "UnitStorage.h"
#include "Common.h"
#include "BuildMinimax.h"

namespace ProBot{

	typedef int unit_id_t;

	const int ARMY_STRENGTH_BUFFER = 1000;

	class KnowledgeBank
	{
		// list of all enemies seen so far
		std::unordered_map<unit_id_t, EnemyUnit> _knownEnemies;
		
		// note: a tech unit denotes any unit that contributes to a player's tech tree
		
		tech_count_map_t _enemyTechUnits;
		tech_count_map_t _myTechUnits;
		
		// bases / resource depots for both players
		std::vector<EnemyUnit> _enemyBases;
		std::vector<BWAPI::Unit> _myBases;
		BWAPI::Position _closestEnemyBase = BWAPI::Positions::Unknown;

		// possible expansion locations that have been located on the map
		std::vector<BWAPI::Position> _expansionLocations;

		// @TODO
		enum InformationStates{
			UNKNOWN,
			STALE,
			PARTIAL,
			OMNISCIENT,
			NUM_INFORMATION_STATES
		};

	public:
		KnowledgeBank();
		/**
			Adds new unit to list of tech units for the AI player.
			@param unit - the unit that has been discovered
		*/
		void handleMyUnitFirstSighting(BWAPI::Unit);
		/**
			Removes unit from list of tech units for the AI player.
			@param unit - the unit that has been discovered
		*/
		void handleMyUnitKilled(BWAPI::Unit);
		/**
			Adds new unit to list of tech units for the enemy player.
			@param enemy - the enemy that has been discovered
		*/
		void handleEnemyUnitFirstSighting(BWAPI::Unit);
		/**
			Removes unit from list of tech units for the enemy player.
			@param enemy - the enemy that has been discovered
		*/
		void handleEnemyUnitKilled(BWAPI::Unit);

		/**
			Iterate through all available enemy units and update currently known information. 
		*/
		void updateEnemyUnits();
		/**
			Iterate through all known enemy bases and search for the base closest to start location.
		*/
		void updateClosestEnemyBase();
		/**
			Inserts a unit type into the given map by incrementing the count value for the given unit type key by one.
			@param unitType - the unitType being inserted
			@param techCountMap - the map into which the unitType is inserted
		*/
		void insertTech(BWAPI::UnitType, tech_count_map_t &);
		
		/**
			Estimates the combat value currently possessed by enemy units.
		*/
		int evaluateEnemyArmyStrength();
		/**
			Estimates the combat value currently possessed by own units.
		*/
		int evaluateMyArmyStrength();
		/**
			Returns true if own army is weaker than enemy army by constant ARMY_STRENGTH_BUFFER.
		*/
		bool shouldNotFight();

		/**
			Draws the information state for debugging purposes.
		*/
		void draw();

		/**
			Returns the enemy base closest to the AI's starting location.
		*/
		BWAPI::Position getClosestEnemyBase();
		tech_count_map_t getEnemyTech();
		tech_count_map_t getMyTech();
		int getNumberOfEnemiesOfType(BWAPI::UnitType);
		EnemyUnit& getEnemyDataByID(unit_id_t);
	};
}