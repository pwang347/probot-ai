/**
	MacroManager.h
	Main manager class for macro decision-making.

	@author Paul Wang
	@version 0.1 9/8/16
*/

#pragma once

#include <BWAPI.h>
#include "BuildItem.h"
#include "BuildOrders.h"
#include "MicroManager.h"
#include "BuildMinimax.h"

using namespace BWAPI::UnitTypes;
using namespace BWAPI::TechTypes;
using namespace BWAPI::UpgradeTypes;

namespace ProBot
{
	const int MAX_QUEUE_SIZE = 5;

	class MacroManager
	{
		MicroManager _microManager;
		KnowledgeBank _knowledgeBank;
		UnitStorage _unitStorage;

		// units that were last used to train/research/upgrade
		BWAPI::Unit _lastTrainer;
		BWAPI::Unit _lastResearcher;
		BWAPI::Unit _lastUpgrader;
		// the last seen flag
		Flag _lastFlag = Flag::NONE;

		// the build item that should be built, based on minimax algorithm
		BuildItem _targetTech;
		// unit types (building only) that should be rebuilt when destroyed
		std::vector<BWAPI::UnitType> _persistUnitTypes;

		int _queuedMinerals = 0;
		int _queuedGas = 0;
		std::vector<BuildItem> _buildOrder = BuildOrders::BASIC_DRAGOON;

	public:
		MacroManager();
		/**
			Check for building of next build item and delegate workers to gas and minerals.
		*/
		void update();
		/**
			Draws the information state for debugging purposes.
		*/
		void draw();
		/**
			Updates the next tech to build based on call to minimax function.
		*/
		void updateTargetTech();
		/**
			Attempts creation of next build item in queue by checking if the requirements are met.
		*/
		void attemptBuildOrder();
		/**
			Returns true if all requirements for a build item are met, false otherwise.
		*/
		bool buildRequirementsAreMet(std::vector<req_value_pair_t>);
		/**
			Delegates units into control groups based on the newest flag.
		*/
		void handleNewFlag(Flag flag);
		/**
			Attempts the training of a new unit.
			@param next - the next build item
			@return true if unit successfully trained, false otherwise
		*/
		bool trainUnit(BuildItem &);
		/**
			Attempts the research of a new tech.
			@param next - the next build item
			@return true if tech successfully researched, false otherwise
		*/
		bool researchTech(BuildItem &);
		/**
			Attempts the purchase of a new upgrade.
			@param next - the next build item
			@return true if upgrade successfully purchased, false otherwise
		*/
		bool purchaseUpgrade(BuildItem &);
		/**
			Attempts constructing a building using a given worker.
			@param worker - the worker unit constructing the building
			@param building - the building to be constructed
			@return true if building successfully constructed, false otherwise
		*/
		bool buildStructure(BWAPI::Unit, BWAPI::UnitType);
		/**
			Event handler for building creation.
		*/
		void onBuildingCreated(BWAPI::Unit);
		/**
			Event handler for building destruction.
		*/
		void onBuildingDestroyed(BWAPI::Unit);

		std::vector<BuildItem> getBuildOrder();
		void setBuildOrder(std::vector<BuildItem>);

		KnowledgeBank * getKnowledgeBank();
		UnitStorage * getUnitStorage();
		MicroManager * getMicroManager();

		BWAPI::Unit getOptimalTrainer(BWAPI::UnitType);
		BWAPI::Unit getOptimalResearcher(BWAPI::UnitType);
		BWAPI::Unit getOptimalUpgrader(BWAPI::UnitType);
		BWAPI::Unit getOptimalBuilder();
	};

}