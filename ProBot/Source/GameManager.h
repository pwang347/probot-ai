/**
	GameManager.h
	Receives events passed down by the AI module and delegates these to the respective managers

	@author Paul Wang
	@version 9/8/16
*/

#pragma once

#include "UnitStorage.h"
#include "MicroManager.h"
#include "MacroManager.h"
#include "KnowledgeBank.h"
#include "ControlGroup.h"
#include "Debug.h"
#include "Common.h"
#include <BWAPI.h>

namespace ProBot
{
	// update MacroManager and other sub managers only after this many frames
	const int FRAMES_BEFORE_UPDATE = 6;

	class GameManager
	{
		MacroManager _macroManager;

	public:
		/**
			Update sub-managers and draw debug information (if enabled).
		*/
		void update();
		/**
			Draws information for each manager onto the game screen.
		*/
		void draw();
		/**
			Event handler for unit under attack.
		*/
		void onUnitUnderAttack(BWAPI::Unit unit);
		/**
			Event handler for nuke detected.
		*/
		void onNukeDetect(BWAPI::Position target);
		/**
			Event handler for unit discovered.
		*/
		void onUnitDiscover(BWAPI::Unit unit);
		/**
			Event handler for unit destroyed.
		*/
		void onUnitEvade(BWAPI::Unit unit);
		/**
			Event handler for unit shown.
		*/
		void onUnitShow(BWAPI::Unit unit);
		/**
			Event handler for unit hidden.
		*/
		void onUnitHide(BWAPI::Unit unit);
		/**
			Event handler for unit created.
		*/
		void onUnitCreate(BWAPI::Unit unit);
		/**
			Event handler for unit destroyed.
		*/
		void onUnitDestroy(BWAPI::Unit unit);
		/**
			Event handler for unit morphed.
		*/
		void onUnitMorph(BWAPI::Unit unit);
		/**
			Event handler for unit switching allegiance.
		*/
		void onUnitRenegade(BWAPI::Unit unit);
		/**
			Event handler for unit completed.
		*/
		void onUnitComplete(BWAPI::Unit unit);
	};

}