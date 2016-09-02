/**
	BuildItem.h
	Represents a build item in a player's build order, including flags to invade and scout.

	@author Paul Wang
	@version 9/8/16
*/


#pragma once

#include <BWAPI.h>
#include "Flag.h"

namespace ProBot
{
	typedef std::pair<int, int> req_value_pair_t;

	namespace Build{
		// enumeration of non-flag BuildItem types
		enum ItemTypes{
			UNIT,
			TECH,
			UPGRADE,
			NUM_TYPES
		};

		// enumeration of possible requirements
		enum ItemRequirements{
			MINERAL,
			GAS,
			SUPPLY,
			TIME,
			NUM_REQUIREMENTS
		};

		// additional requirements for unit count
		// note: the constants are used to distinguish between my unit or enemy unit
		const int MY_UNIT_COUNT_ID = 1000; // there are roughly 200 unit types, so ints in 1000 <= i < 2000 will be owned units
		const int ENEMY_UNIT_COUNT_ID = 2000;

		// error code for enumerations
		const int INT_ERROR = -1;
	}

	class BuildItem{
		BWAPI::UnitType _unit;
		BWAPI::TechType _tech;
		BWAPI::UpgradeType _upgrade;
		Flag _flag;
		std::vector<req_value_pair_t> _requirements;
		bool _persist;
		bool _isFlag;
		int _upgradeLevel;
		int _type;
		int _multiplicity;

	public:
		// Constructor for unit build item.
		BuildItem(BWAPI::UnitType unit, int multiplicity = 1, bool persist = false, std::vector<req_value_pair_t> requirements = {});
		// Constructor for tech build item.
		BuildItem(BWAPI::TechType tech, std::vector<req_value_pair_t> requirements = {});
		// Constructor for upgrade build item.
		BuildItem(BWAPI::UpgradeType upgrade, int level = 1, std::vector<req_value_pair_t> requirements = {});
		// Constructor for flag build item.
		BuildItem(Flag flag = Flag::NONE, std::vector<req_value_pair_t> requirements = {});
		
		int getMineralCost();
		int getGasCost();
		int getType();
		BWAPI::UnitType getUnit();
		BWAPI::TechType getTech();
		BWAPI::UpgradeType getUpgrade();
		int getMultiplicity();
		// Decrements the multiplicity value for a unit build item by one.
		void decreaseMulitplicity();
		int getLevel();
		bool isFlag();
		Flag getFlag();
		bool isPersist();
		std::vector<req_value_pair_t> getRequirements();
		// Retrieves the trainer/researcher/upgrader of the build item.
		BWAPI::UnitType getParent();
	};
}