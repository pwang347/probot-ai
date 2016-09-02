/**
	MicroManager.h
	Manager class for managing control groups.

	@author Paul Wang
	@version 9/8/16
*/

#pragma once

#include <BWAPI.h>
#include "KnowledgeBank.h"
#include "ControlGroup.h"

namespace ProBot
{
	class MicroManager
	{
		KnowledgeBank * _knowledgeBank;
		// @TODO
		//std::vector<BWAPI::Position> _projectedContestingPoints;
		std::vector<ControlGroup> _controlGroups;

	public:
		MicroManager();
		MicroManager(KnowledgeBank*);
		/**
			Validate and update each control group.
		*/
		void update();
		/**
			Draws the information state for debugging purposes.
		*/
		void draw();
		/**
			Adds a new control group.
		*/
		void createControlGroup(BWAPI::Unitset, Flag);
		/**
			Adds a new control group.
		*/
		void createControlGroup(BWAPI::Unit, Flag);
		/**
			Removes all empty groups.
		*/
		void validateGroups();
		/**
			Removes a control group.
		*/
		void removeControlGroup(ControlGroup);
		/**
			Merges the two provided control groups.
		*/
		void mergeControlGroups(ControlGroup, ControlGroup);
		/**
			Merges all control groups currently in the manager.
		*/
		void mergeAllControlGroups(ControlGroup);
		/**
			Overrides the strategy flag for all control groups.
		*/
		void issueGlobalOrder(Flag);
		/**
			Overrides the strategy flag for control groups within radius of a position.
		*/
		void issueLocalOrder(Flag, BWAPI::Position, int radius);
		// @TODO
		//void onContestingPointCreate();
		//void onContestingPointResolve();
		std::vector<ControlGroup> getControlGroups();

	};

}