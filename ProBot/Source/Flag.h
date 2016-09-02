/**
	Flag.h
	Contains enumeration class for strategy flags used in build items and control groups.

	@author Paul Wang
	@version 9/8/16
*/

#pragma once

namespace ProBot{
	enum class Flag {
		NONE,
		ALL_IN,
		INVADE,
		CLEAR_AREA,
		RETREAT,
		DEFEND_BASE,
		SCOUT_ENEMY,
		SCOUT_RESOURCES,
		NUM_FLAGS
	};
}