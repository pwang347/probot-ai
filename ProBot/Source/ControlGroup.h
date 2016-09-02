/**
	ControlGroup.h
	A control group for delegating individual unit tasks, such as invading and scouting.

	@author Paul Wang
	@version 9/8/16
*/

#pragma once

#include <BWAPI.h>
#include "KnowledgeBank.h"
#include "UnitBehaviour.h"
#include "Flag.h"

namespace ProBot {
	// the maximum distance to the lagging unit in the group before a unit has to turn back to follow it
	const int MAX_LAGGING_DISTANCE = 300;

	class ControlGroup {
		BWAPI::Unitset _controlUnits;
		Flag _currentStrategy;
		Flag _lastStrategy;
		bool _isInCombat;

	public:
		bool operator==(ControlGroup other);

		ControlGroup(const BWAPI::Unitset& units = BWAPI::Unitset::none, Flag flag = Flag::NONE);
		/**
			Update the units in the control group based on the current state of information in the knowledge bank.
		*/
		void update(KnowledgeBank*);
		Flag getFlag();
		void overrideFlag(Flag newFlag);
		void addUnit(const BWAPI::Unit&);
		void addUnits(const BWAPI::Unitset&);
		void removeUnit(const BWAPI::Unit&);
		void removeUnits(const BWAPI::Unitset&);
		/**
			Overrides units in the control group to a given group of units.
		*/
		void setControlGroup(const BWAPI::Unitset&);
		/**
			Orders the units in the group to move using their own unit behaviours.
			Ensures that the lagging units do not fall behind.
		*/
		void moveAsGroup(BWAPI::Position);
		/**
			Removes units that are dead or invalid pointers.
		*/
		void validateUnits();
		bool isInCombat();
		BWAPI::Unitset getUnits();
	};
}