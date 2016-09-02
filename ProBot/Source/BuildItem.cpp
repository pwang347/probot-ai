/**
	BuildItem.cpp
	Represents a build item in a player's build order, including flags to invade and scout.

	@author Paul Wang
	@version 9/8/16
*/

#include "BuildItem.h"

using namespace ProBot;

BuildItem::BuildItem(BWAPI::UnitType unit, int multiplicity, bool persist, std::vector<req_value_pair_t> requirements){
	_unit = unit;
	_type = Build::ItemTypes::UNIT;
	_multiplicity = multiplicity;
	_persist = persist;
	_requirements = requirements;
	_isFlag = false;
}

BuildItem::BuildItem(BWAPI::TechType tech, std::vector<req_value_pair_t> requirements){
	_tech = tech;
	_type = Build::ItemTypes::TECH;
	_requirements = requirements;
	_isFlag = false;
}

BuildItem::BuildItem(BWAPI::UpgradeType upgrade, int level, std::vector<req_value_pair_t> requirements){
	_upgrade = upgrade;
	_type = Build::ItemTypes::UPGRADE;
	_upgradeLevel = level;
	_requirements = requirements;
	_isFlag = false;
}

BuildItem::BuildItem(Flag flag, std::vector<req_value_pair_t> requirements){
	_flag = flag;
	_isFlag = true;
	_requirements = requirements;
}

int BuildItem::getMineralCost(){
	switch (_type){
	case Build::ItemTypes::UNIT:
		return _unit.mineralPrice();
	case Build::ItemTypes::TECH:
		return _tech.mineralPrice();
	case Build::ItemTypes::UPGRADE:
		return _upgrade.mineralPrice(_upgradeLevel);
	default:
		return Build::INT_ERROR;
	}
}

int BuildItem::getGasCost(){
	switch (_type){
	case Build::ItemTypes::UNIT:
		return _unit.gasPrice();
	case Build::ItemTypes::TECH:
		return _tech.gasPrice();
	case Build::ItemTypes::UPGRADE:
		return _upgrade.gasPrice(_upgradeLevel);
	default:
		return Build::INT_ERROR;
	}
}

int BuildItem::getType(){
	return _type;
}

BWAPI::UnitType BuildItem::getUnit(){
	return _unit;
}

BWAPI::TechType BuildItem::getTech(){
	return _tech;
}

BWAPI::UpgradeType BuildItem::getUpgrade(){
	return _upgrade;
}

void BuildItem::decreaseMulitplicity(){
	_multiplicity--;
}

int BuildItem::getMultiplicity(){
	return _multiplicity;
}

int BuildItem::getLevel(){
	return _upgradeLevel;
}

bool BuildItem::isFlag(){
	return _isFlag;
}

Flag BuildItem::getFlag(){
	return _flag;
}

bool BuildItem::isPersist(){
	return _persist;
}

std::vector<req_value_pair_t> BuildItem::getRequirements(){
	return _requirements;
}

BWAPI::UnitType BuildItem::getParent(){
	switch (_type){
	case Build::ItemTypes::UNIT:
		return _unit.whatBuilds().first;
	case Build::ItemTypes::TECH:
		return _tech.whatResearches();
	case Build::ItemTypes::UPGRADE:
		return _upgrade.whatUpgrades();
	default:
		return BWAPI::UnitTypes::None;
	}
}