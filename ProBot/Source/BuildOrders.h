/**
	BuildOrders.h
	Collection of pre-made build orders that may be used by the MacroManager.

	@author Paul Wang
	@version 9/8/16
*/

#pragma once

#include "Common.h"
#include "BuildItem.h"

using namespace ProBot;

using namespace BWAPI::UnitTypes;
using namespace BWAPI::TechTypes;
using namespace BWAPI::UpgradeTypes;

namespace ProBot{
	namespace BuildOrders{
		// do nothing
		const std::vector<BuildItem> DEBUG_EMPTY = make_vector<BuildItem>();

		// order army to attack on spawn
		const std::vector<BuildItem> DEBUG_ALL_IN = make_vector<BuildItem>()
			<< BuildItem{Flag::ALL_IN}
		<< BuildItem{Protoss_Gateway};
		
		const std::vector<BuildItem> DT_RUSH = make_vector<BuildItem>()
			<< BuildItem{ Protoss_Probe, 4 } // 8
			<< BuildItem{ Protoss_Pylon }
			<< BuildItem{ Flag::SCOUT_ENEMY }
			<< BuildItem{ Protoss_Probe, 2 } // 10
			<< BuildItem{ Protoss_Gateway}
		<< BuildItem{ Protoss_Probe, 2 } // 12
		<< BuildItem{Protoss_Assimilator}
		<< BuildItem{ Protoss_Probe } // 13
		<< BuildItem{ Protoss_Zealot } // 15
		<< BuildItem{ Protoss_Probe } // 16
		<< BuildItem{ Protoss_Pylon }
		<< BuildItem{ Protoss_Probe, 2 } // 18
		<< BuildItem{ Protoss_Cybernetics_Core }
		<< BuildItem{ Protoss_Probe } // 19
		<< BuildItem{ Protoss_Zealot } // 21
		<< BuildItem{ Protoss_Probe } // 22
		<< BuildItem{ Protoss_Pylon }
		<< BuildItem{ Protoss_Probe } // 23
		<< BuildItem{ Protoss_Dragoon } // 25
		<< BuildItem{ Protoss_Probe } // 26
		<< BuildItem{ Protoss_Citadel_of_Adun }
		<< BuildItem{ Protoss_Probe } // 27
		<< BuildItem{ Protoss_Dragoon } // 29
		<< BuildItem{ Protoss_Pylon }
		<< BuildItem{ Protoss_Templar_Archives }
		<< BuildItem{ Flag::INVADE }
		<< BuildItem{ Protoss_Zealot, 2} // 33
		<< BuildItem{ Protoss_Pylon }
		<< BuildItem{ Protoss_Dark_Templar, 2 }
		<< BuildItem{ Protoss_Forge }
		<< BuildItem{ Protoss_Pylon }
		//<< BuildItem{ Protoss_Nexus }
		<< BuildItem{ Protoss_Photon_Cannon }
		<< BuildItem{ Protoss_Photon_Cannon }
		<< BuildItem{ Flag::INVADE }
		<< BuildItem{ Protoss_Photon_Cannon }
		<< BuildItem{ Protoss_Photon_Cannon }
		<< BuildItem{ Protoss_Dark_Templar, 100 };
	
		const std::vector<BuildItem> A99GATEWAYS = make_vector<BuildItem>()
			<< BuildItem{ Protoss_Probe, 4 } // 8
			<< BuildItem{ Protoss_Pylon }
			<< BuildItem{ Flag::SCOUT_ENEMY }
			<< BuildItem{ Protoss_Probe } // 9
			<< BuildItem{ Protoss_Gateway }
			<< BuildItem{ Protoss_Gateway }
			<< BuildItem{ Protoss_Probe, 2 } // 11
			<< BuildItem{ Protoss_Zealot } // 13
			<< BuildItem{ Protoss_Pylon }
			<< BuildItem{ Protoss_Zealot, 2 } // 17
			<< BuildItem{ Protoss_Gateway }
			<< BuildItem{ Protoss_Pylon }
			<< BuildItem{ Flag::ALL_IN }
			<< BuildItem{ Protoss_Zealot, 4 } // 25
			<< BuildItem{ Protoss_Pylon }
			<< BuildItem{ Protoss_Gateway }
		<< BuildItem{ Protoss_Zealot, 4 }
		<< BuildItem{ Protoss_Pylon }
		<< BuildItem{ Protoss_Zealot, 4 }
		<< BuildItem{ Protoss_Pylon }
		<< BuildItem{ Protoss_Zealot, 4 }
		<< BuildItem{ Protoss_Pylon }
		<< BuildItem{ Protoss_Zealot, 4 }
		<< BuildItem{ Protoss_Pylon }
		<< BuildItem{ Protoss_Zealot, 4 }
		<< BuildItem{ Protoss_Pylon }
		<< BuildItem{ Protoss_Zealot, 4 }
		<< BuildItem{ Protoss_Pylon }
		<< BuildItem{ Protoss_Zealot, 1000 };

		const std::vector<BuildItem> BASIC_DRAGOON = make_vector<BuildItem>()
			<< BuildItem{Protoss_Probe, 4} // 8
			<< BuildItem{Protoss_Pylon}
			<< BuildItem{Protoss_Probe, 2} // 10
			<< BuildItem{ Protoss_Gateway}
			<< BuildItem{Protoss_Probe} // 11
			<< BuildItem{Protoss_Assimilator}
			<< BuildItem{Protoss_Probe, 2} // 13
			<< BuildItem{Protoss_Cybernetics_Core}
			<< BuildItem{Protoss_Probe, 2} // 15
		<< BuildItem{ Protoss_Gateway }
		<< BuildItem{Protoss_Pylon}
		<< BuildItem{Protoss_Dragoon} // 17
		<< BuildItem{Singularity_Charge}
		<< BuildItem{ Protoss_Dragoon, 4 }
		<< BuildItem{ Protoss_Gateway }
		<< BuildItem{ Protoss_Dragoon, 8 }
		<< BuildItem{ Flag::ALL_IN}
		<< BuildItem{ Protoss_Dragoon, 100 };
	}

}