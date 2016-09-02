/**
	UnitBehaviour.h
	Assorted classes representing different unit behaviours for common orders.

	@author Paul Wang
	@version 9/8/16
*/

#pragma once

#include <BWAPI.h>
#include "Common.h"
#include "CombatUtil.h"

namespace ProBot{
	// enumeration of unit behaviours
	enum UnitBehaviours{
		MELEE,
		RANGED,
		CASTER,
		SCOUT,
		NUM_BEHAVIOURS
	};
	
	class IUnitBehaviour{
	public:
		/**
			Unit behaviour for the move order.
		*/
		virtual void moveOrder(BWAPI::Unit, BWAPI::Position) = 0;
		/**
			Unit behaviour for the attack order.
		*/
		virtual void attackOrder(BWAPI::Unit, BWAPI::Position) = 0;
		/**
			Unit behaviour for the idle order.
		*/
		virtual void idleOrder(BWAPI::Unit) = 0;
	};

	class MeleeBehaviour : public IUnitBehaviour{
	public:
		MeleeBehaviour();
		void moveOrder(BWAPI::Unit, BWAPI::Position);
		void attackOrder(BWAPI::Unit, BWAPI::Position);
		void idleOrder(BWAPI::Unit);
	};

	class RangedBehaviour : public IUnitBehaviour{
	public:
		RangedBehaviour();
		void moveOrder(BWAPI::Unit, BWAPI::Position);
		void attackOrder(BWAPI::Unit, BWAPI::Position);
		void idleOrder(BWAPI::Unit);
	};

	class CasterBehaviour : public IUnitBehaviour{
	public:
		CasterBehaviour();
		void moveOrder(BWAPI::Unit, BWAPI::Position);
		void attackOrder(BWAPI::Unit, BWAPI::Position);
		void idleOrder(BWAPI::Unit);
	};

	class ScoutBehaviour : public IUnitBehaviour{
	public:
		ScoutBehaviour();
		void moveOrder(BWAPI::Unit, BWAPI::Position);
		void attackOrder(BWAPI::Unit, BWAPI::Position);
		void idleOrder(BWAPI::Unit);
	};

	IUnitBehaviour * getUnitBehaviour(BWAPI::UnitType unitType);

	// initialize unit behaviours in vector
	const std::vector<IUnitBehaviour*> UNIT_BEHAVIOURS = make_vector<IUnitBehaviour*>()
		<< new MeleeBehaviour() << new RangedBehaviour() << new CasterBehaviour() << new ScoutBehaviour();

}