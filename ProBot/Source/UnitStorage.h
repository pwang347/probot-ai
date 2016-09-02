/**
	UnitStorage.h
	Utility class to store and retrieve units according to common categories to reduce filtering.

	@author Paul Wang
	@version 9/8/16
*/


#pragma once

#include <BWAPI.h>
#include <array>
#include "Debug.h"

namespace ProBot
{
	typedef std::pair<BWAPI::Unit, int> refinery_worker_count_pair_t;

	class UnitStorage
	{
	public:
		enum UnitCategories{
			ALL,
			WORKER,
			COMBAT,
			SCOUT,
			TRAINER,
			UPGRADER,
			RESEARCHER,
			BASE,
			NUM_CATEGORIES
		};

		UnitStorage();
		void draw();

		BWAPI::Unitset getUnitsUsingFilter(const BWAPI::UnitFilter & pred);

		BWAPI::Unitset& getUnits(int category);
		BWAPI::Unitset getUnitsOfType(BWAPI::UnitType);

		std::array<bool, NUM_CATEGORIES> getCategories(BWAPI::UnitType);

		std::vector<refinery_worker_count_pair_t>& getRefineryUnits();
		std::vector<BWAPI::Unit> getQueuedUnits();

		bool isInQueue(BWAPI::Unit);
		bool isInQueue(BWAPI::UnitType);
		void removeInvalidQueueUnits();
		void removeFromQueue(BWAPI::Unit);

		void setAll();
		void clearAll();
		bool isValidUnit(BWAPI::Unit);
		void handleUnitDestroy(BWAPI::Unit);
		void handleUnitComplete(BWAPI::Unit);
		void handleUnitCreate(BWAPI::Unit);
		BWAPI::Unitset getNUnitsOfType(BWAPI::UnitType unitType, int n, const BWAPI::UnitFilter & pred = &BWAPI::UnitFilter::isValid);
		BWAPI::Unitset getNUnitsOfCategory(int category, int n, const BWAPI::UnitFilter & pred = &BWAPI::UnitFilter::isValid);

		bool isScout(BWAPI::UnitType);
	private:
		BWAPI::Unitset _units[NUM_CATEGORIES];
		std::vector<refinery_worker_count_pair_t> _refineryUnits;
		std::vector<BWAPI::Unit> _queuedUnits;
	};
}