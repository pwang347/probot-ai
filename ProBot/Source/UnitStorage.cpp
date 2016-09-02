/**
	UnitStorage.cpp
	Utility class to store and retrieve units by category.

	@author Paul Wang
	@version 9/8/16
*/


#include "UnitStorage.h"

using namespace ProBot;

UnitStorage::UnitStorage(){

}

void UnitStorage::draw()
{
	std::string s = ((" | All: " + std::to_string(_units[ALL].size()))).append
		((" | Worker: " + std::to_string(_units[WORKER].size()))).append
		((" | Combat: " + std::to_string(_units[COMBAT].size()))).append
		((" | Trainer: " + std::to_string(_units[TRAINER].size()))).append
		((" | Upgrader: " + std::to_string(_units[UPGRADER].size()))).append
		((" | Researcher: " + std::to_string(_units[RESEARCHER].size()))).append
		((" | Base: " + std::to_string(_units[BASE].size()))).append
		((" | Scout: " + std::to_string(_units[SCOUT].size())));
	Debug::log(s, 1);
}

std::array<bool, UnitStorage::NUM_CATEGORIES> UnitStorage::getCategories(BWAPI::UnitType unitType){
	std::array<bool, NUM_CATEGORIES> categories = std::array<bool, NUM_CATEGORIES>();
	categories.at(ALL) = true;
	if (unitType.isResourceDepot())
		categories.at(BASE) = true;
	if (unitType.isWorker())
		categories.at(WORKER) = true;
	else if (unitType.canAttack())
		categories.at(COMBAT) = true;
	if (isScout(unitType))
		categories.at(SCOUT) = true;
	if (unitType.isBuilding()){
		if (unitType.buildsWhat().size() > 0)
			categories.at(TRAINER) = true;
		if (unitType.upgradesWhat().size() > 0)
			categories.at(UPGRADER) = true;
		if (unitType.researchesWhat().size() > 0)
			categories.at(RESEARCHER) = true;
	}
	return categories;
}

BWAPI::Unitset& UnitStorage::getUnits(int category){
	return _units[category];
}

BWAPI::Unitset UnitStorage::getUnitsUsingFilter(const BWAPI::UnitFilter & pred){
	BWAPI::Unitset unitset;
	for (auto & unit : BWAPI::Broodwar->self()->getUnits())
		if (pred(unit))
			unitset.insert(unit);
	return unitset;
}

BWAPI::Unitset UnitStorage::getUnitsOfType(BWAPI::UnitType unitType){
	BWAPI::Unitset unitset;
	std::array<bool, NUM_CATEGORIES> categories = getCategories(unitType);
	
	int unitCategory = 0;
	for (int i = NUM_CATEGORIES-1; i > 0; i--){
		if (categories[i]){
			unitCategory = i;
			break;
		}
	}
	// we will search through only units that match at least one category
	for (auto & unit : getUnits(unitCategory)){
		if (unit->getType().getID() == unitType.getID())
			unitset.insert(unit);
	}
	return unitset;
}

BWAPI::Unitset UnitStorage::getNUnitsOfType(BWAPI::UnitType unitType, int n, const BWAPI::UnitFilter & pred){
	BWAPI::Unitset unitset;
	for (auto & unit : getUnitsOfType(unitType)){
		if (pred(unit)){
			unitset.insert(unit);
			if (static_cast<int>(unitset.size()) >= n)
				break;
		}
	}
	return unitset;
}

BWAPI::Unitset UnitStorage::getNUnitsOfCategory(int category, int n, const BWAPI::UnitFilter & pred){
	BWAPI::Unitset unitset;
	if (n <= 0){
		return unitset;
	}
	else {
		for (auto & unit : getUnits(category)){
			if (pred(unit)){
				unitset.insert(unit);
				if (static_cast<int>(unitset.size()) >= n)
					break;
			}
		}
	}
	return unitset;
}

std::vector<refinery_worker_count_pair_t>& UnitStorage::getRefineryUnits(){
	return _refineryUnits;
}

// ASSUMPTION: Unit was added correctly when spawned
void UnitStorage::handleUnitDestroy(BWAPI::Unit unit){
	std::array<bool, NUM_CATEGORIES> categories = getCategories(unit->getType());
	for (int i = 0; i < NUM_CATEGORIES; i++){
		if (categories[i])
			_units[i].erase(unit);
	}
}

void UnitStorage::handleUnitComplete(BWAPI::Unit unit){
	if (isValidUnit(unit))
	{
		removeFromQueue(unit);
		
		std::array<bool, NUM_CATEGORIES> categories = getCategories(unit->getType());
		for (int i = 0; i < NUM_CATEGORIES; i++)
			if (categories[i])
				_units[i].insert(unit);

		if (unit->getType().isRefinery())
			_refineryUnits.push_back(std::make_pair(unit, 0));
	}
}

void UnitStorage::handleUnitCreate(BWAPI::Unit unit){
	_queuedUnits.push_back(unit);
}

std::vector<BWAPI::Unit> UnitStorage::getQueuedUnits(){
	return _queuedUnits;
}

bool UnitStorage::isInQueue(BWAPI::Unit unit){
	auto it = std::find(_queuedUnits.begin(), _queuedUnits.end(), unit);

	return (it != _queuedUnits.end());
}

bool UnitStorage::isInQueue(BWAPI::UnitType type){
	removeInvalidQueueUnits();

	for (auto & unit : _queuedUnits)
		if (unit->getType() == type)
			return true;
	return false;
}

void UnitStorage::removeFromQueue(BWAPI::Unit unit){
	auto it = std::find(_queuedUnits.begin(), _queuedUnits.end(), unit);
	if (it != _queuedUnits.end()) {
		std::iter_swap(it, _queuedUnits.end() - 1);
		_queuedUnits.erase(_queuedUnits.end() - 1);
	}
}

void UnitStorage::removeInvalidQueueUnits(){
	for (auto & unit : _queuedUnits){
		if (!unit)
			removeFromQueue(unit);
	}
}


bool UnitStorage::isValidUnit(BWAPI::Unit unit){
	return	(!unit || !unit->exists() || // non-null
		(unit->isLockedDown() || unit->isMaelstrommed() || unit->isStasised()) || // status ailment
		(unit->isLoaded() || !unit->isPowered() || unit->isStuck()) || // bad state
		(unit->isCompleted() && !unit->isConstructing())); // incomplete
}

void UnitStorage::clearAll(){
	for (auto & unitset : _units){
		unitset.clear();
	}
}

void UnitStorage::setAll(){
	clearAll();
	for (auto & unit : BWAPI::Broodwar->self()->getUnits())
		handleUnitComplete(unit);
}

/**
	Invisible and flying units can be used as scouts
*/
bool UnitStorage::isScout(BWAPI::UnitType unitType){
	return CombatUtil::isInvisibleThreat(unitType) ||
		unitType.isFlyer();
}