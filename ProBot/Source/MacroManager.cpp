#include "MacroManager.h"

using namespace ProBot;
using namespace BWAPI::Filter;

MacroManager::MacroManager(){
	_microManager = MicroManager(&_knowledgeBank);
}

KnowledgeBank * MacroManager::getKnowledgeBank(){
	return &_knowledgeBank;
}

UnitStorage * MacroManager::getUnitStorage(){
	return &_unitStorage;
}
MicroManager * MacroManager::getMicroManager(){
	return &_microManager;
}

void MacroManager::attemptBuildOrder(){
	// do nothing if there's nothing left in the build order
	if (_buildOrder.size() > 0){
		BuildItem & next = _buildOrder.at(0);

		// if additional requirements are not met, do nothing
		if (!buildRequirementsAreMet(next.getRequirements()))
			return;

		// short-hand aliases for game info
		int minerals = BWAPI::Broodwar->self()->minerals();
		int gas = BWAPI::Broodwar->self()->gas();
		int supply = BWAPI::Broodwar->self()->supplyUsed();

		// if the next build item is a flag
		if (next.isFlag()){
			handleNewFlag(next.getFlag());
			_lastFlag = next.getFlag();
			_buildOrder.erase(_buildOrder.begin());
		}
		else { // else, the next build item is something that should be trained/researched/upgraded
			
			// validate build orders by inserting build items needed before next item

			if (next.getType() == Build::ItemTypes::UNIT){
				// check if supply locked
				// don't do anything if a pylon is already queued or under construction
				if (next.getUnit().getID() != Protoss_Pylon.getID() && next.getUnit().supplyRequired() + supply > BWAPI::Broodwar->self()->supplyTotal()
					&& BWAPI::Broodwar->self()->incompleteUnitCount(BWAPI::UnitTypes::Protoss_Pylon) <= 0){
					_buildOrder.insert(_buildOrder.begin(), BuildItem{ Protoss_Pylon });
					// ensure that we have just the right amount of pylons, not too many
				} else if (next.getUnit().getID() == BWAPI::UnitTypes::Protoss_Pylon.getID() && BWAPI::Broodwar->self()->supplyUsed() <= BWAPI::Broodwar->self()->supplyTotal() - 8){
					_buildOrder.erase(_buildOrder.begin());
				}

				if (!BWAPI::Broodwar->canMake(next.getUnit())){
					// check if we have the prereqs for the unit, if not add it to build order
					BWAPI::UnitType parentType = BuildMinimax::getParentType(next.getUnit());
					if (!BuildMinimax::prereqObtained(next.getUnit()) && BWAPI::Broodwar->self()->incompleteUnitCount(parentType) <= 0){
						Debug::message("Adding parent " + parentType.getName() + " for " + next.getUnit().getName());
						//_buildOrder.insert(_buildOrder.begin(), BuildItem{ parentType });
					}
				}
			}

			// check if we have enough resources to build the next build item
			if (next.getMineralCost() <= (minerals - _queuedMinerals) && next.getGasCost() <= (gas - _queuedGas)){
				if (next.getType() == Build::ItemTypes::UNIT){
					trainUnit(next);
				}
				else if (next.getType() == Build::ItemTypes::TECH){
					researchTech(next);
				}
				else if (next.getType() == Build::ItemTypes::UPGRADE){
					purchaseUpgrade(next);
				}
			}
		}
	}
}

bool MacroManager::buildRequirementsAreMet(std::vector<req_value_pair_t> requirements){
	for (auto & requirement : requirements){
		switch (requirement.first){
		case Build::ItemRequirements::MINERAL:
			if (BWAPI::Broodwar->self()->minerals() < requirement.second)
				return false;
			break;
		case Build::ItemRequirements::GAS:
			if (BWAPI::Broodwar->self()->gas() < requirement.second)
				return false;
			break;
		case Build::ItemRequirements::SUPPLY:
			if (BWAPI::Broodwar->self()->supplyUsed() < requirement.second)
				return false;
			break;
		case Build::ItemRequirements::TIME:
			if (BWAPI::Broodwar->getFrameCount() < requirement.second)
				return false;
			break;
		default:
			if (requirement.first >= Build::ENEMY_UNIT_COUNT_ID){
				if (_knowledgeBank.getNumberOfEnemiesOfType(BWAPI::UnitType(requirement.first - Build::ENEMY_UNIT_COUNT_ID))
					< requirement.second)
					return false;
			}
			else if (requirement.first >= Build::MY_UNIT_COUNT_ID){
				if (static_cast<int>(_unitStorage.getUnitsOfType(BWAPI::UnitType(requirement.first - Build::MY_UNIT_COUNT_ID)).size())
					< requirement.second)
					return false;
			}
			break;
		}
	}
	return true;
}

// @TODO
void MacroManager::handleNewFlag(Flag flag){
	switch (flag){
	case Flag::ALL_IN:
		break;
	case Flag::INVADE:
		break;
	case Flag::DEFEND_BASE:
		break;
	case Flag::SCOUT_ENEMY:
		_microManager.createControlGroup(_lastTrainer, Flag::SCOUT_ENEMY);
		_unitStorage.getUnits(UnitStorage::WORKER).erase(_lastTrainer);
		break;
	}
}

bool MacroManager::trainUnit(BuildItem & next){
	// last check to see if the unit can be made
	if (BWAPI::Broodwar->canMake(next.getUnit())){
		// if unit to be trained is a building, get suitable builder
		if (next.getUnit().isBuilding()){
			BWAPI::Unit builder = getOptimalBuilder();
			if (!builder)
				return false;
			if (!buildStructure(builder, next.getUnit()))
				return false;
			_lastTrainer = builder;
		}
		else {
			// unit to be trained is not a building, get suitable trainer
			BWAPI::UnitType trainerType = next.getUnit().whatBuilds().first;
			BWAPI::Unit trainer = getOptimalTrainer(trainerType);
			if (!trainer)
				return false;
			if (!trainer->train(next.getUnit()))
				return false;
			_lastTrainer = trainer;
		}

		// successful build / train, so decrease the multiplicity of the build item by one
		next.decreaseMulitplicity();
		// if no more of unit type, then remove item from build order
		if (next.getMultiplicity() <= 0)
			_buildOrder.erase(_buildOrder.begin());
		// if the unit needs to persist (ie. be rebuilt when destroyed), add to list 
		if (next.isPersist()
			&& std::find(_persistUnitTypes.begin(), _persistUnitTypes.end(), next.getUnit())
			== _persistUnitTypes.end()){
			_persistUnitTypes.push_back(next.getUnit());
		}
		return true;
	}
	else {
		return false;
	}
}

bool MacroManager::researchTech(BuildItem & next){
	// last check to see if the tech can be researched
	if (BWAPI::Broodwar->canResearch(next.getTech())){
		BWAPI::UnitType researcherType = next.getTech().whatResearches();
		BWAPI::Unit researcher = getOptimalResearcher(researcherType);
		if (!researcher)
			return false;
		if (!researcher->research(next.getTech()))
			return false;
		_lastResearcher = researcher;
		// successfully researched tech, erase item from build order
		_buildOrder.erase(_buildOrder.begin());
		return true;
	}
	else {
		return false;
	}
}

bool MacroManager::purchaseUpgrade(BuildItem & next){
	// last check to see if the upgrade can be purchased
	if (BWAPI::Broodwar->canUpgrade(next.getUpgrade())){
		BWAPI::UnitType upgraderType = next.getUpgrade().whatUpgrades();
		BWAPI::Unit upgrader = getOptimalUpgrader(upgraderType);
		if (!upgrader)
			return false;
		if (!upgrader->upgrade(next.getUpgrade()))
			return false;
		_lastUpgrader = upgrader;
		// successfully purchased upgrade, erase item from build order
		_buildOrder.erase(_buildOrder.begin());
		return true;
	}
	else {
		return false;
	}
}

BWAPI::Unit MacroManager::getOptimalBuilder(){
	BWAPI::Unit builder = nullptr;
	// ignore workers that are carrying minerals, as this would slightly delay resource gain
	for (auto & unit : _unitStorage.getUnits(UnitStorage::WORKER))
		if (unit->isGatheringMinerals()){
			builder = unit;
			if (!unit->isCarryingMinerals())
				return builder;
		}
	return builder;
}

BWAPI::Unit MacroManager::getOptimalTrainer(BWAPI::UnitType trainerType){
	int smallestQueue = INT_MAX;
	BWAPI::Unit trainer = nullptr;
	// for each unit that is a potential trainer
	for (auto & unit : _unitStorage.getUnitsOfType(trainerType)){
		// choose trainer if not incomplete and nothing is in queue
		if (unit->isCompleted() && unit->getTrainingQueue().size() == 0){
			trainer = unit;
			break;
			// choose trainer with smallest queue, if not already at max capacity
		} else if (smallestQueue > static_cast<int>(unit->getTrainingQueue().size())
			&& static_cast<int>(unit->getTrainingQueue().size()) < MAX_QUEUE_SIZE){
			smallestQueue = static_cast<int>(unit->getTrainingQueue().size());
			trainer = unit;
		}
	}
	return trainer;
}

BWAPI::Unit MacroManager::getOptimalResearcher(BWAPI::UnitType researcherType){
	BWAPI::Unit researcher = nullptr;
	// any researcher is fine, since it's unlikely we have more than one of that type
	for (auto & unit : _unitStorage.getUnitsOfType(researcherType)){
		researcher = unit;
		break;
	}
	return researcher;
}

BWAPI::Unit MacroManager::getOptimalUpgrader(BWAPI::UnitType upgraderType){
	BWAPI::Unit upgrader = nullptr;
	// any upgrader is fine, since it's unlikely we have more than one of that type
	for (auto & unit : _unitStorage.getUnitsOfType(upgraderType)){
		upgrader = unit;
		break;
	}
	return upgrader;
}

bool MacroManager::buildStructure(BWAPI::Unit worker, BWAPI::UnitType building){
	BWAPI::TilePosition targetBuildLocation = BWAPI::Broodwar->getBuildLocation(building, worker->getTilePosition());
	
	// make sure there aren't any units in the way
	BWAPI::Unitset units = BWAPI::Broodwar->getUnitsInRectangle(BWAPI::Position(targetBuildLocation), BWAPI::Position(targetBuildLocation + building.tileSize()));
	for (auto & unit : units){
		// ignore geysers, flying units and stationary units
		if (building.isRefinery() && unit->getType().getID() == BWAPI::UnitTypes::Resource_Vespene_Geyser.getID()
			|| unit->isFlying() || !unit->canMove())
			continue;
		else {
			unit->move(unit->getClosestUnit(BWAPI::Filter::IsResourceDepot)->getPosition());
			return false;
		}
	}

	if (!targetBuildLocation)
		return false;
	
	// register an event that draws the target build location
	BWAPI::Broodwar->registerEvent([targetBuildLocation, building](BWAPI::Game*)
			{
				BWAPI::Broodwar->drawBoxMap(BWAPI::Position(targetBuildLocation),
					BWAPI::Position(targetBuildLocation + building.tileSize()),
						BWAPI::Colors::Blue);
			},
			nullptr,  // no condition to run
			building.buildTime() + 100);  // frames to run

	// last check to see if the building can be built at given location
	if (BWAPI::Broodwar->canBuildHere(targetBuildLocation, building, worker)){
		// queue resources so that they can't be used for the next build item while worker is moving to build site
		if (worker->build(building, targetBuildLocation)){
			_queuedMinerals += building.mineralPrice();
			_queuedGas += building.gasPrice();
			return true;
		}
	}
	return false;
}

void MacroManager::onBuildingCreated(BWAPI::Unit unit){
	if (_queuedMinerals > 0 || _queuedGas > 0){ // ignore units generated at start of game, and also don't go into negatives.
		_queuedMinerals -= unit->getType().mineralPrice();
		_queuedGas -= unit->getType().gasPrice();
	} // validates queued resource state if ever negative (shouldn't be possible though)
	else if (_queuedMinerals < 0 || _queuedGas < 0){
		_queuedMinerals = 0;
		_queuedGas = 0;
	}
}

void MacroManager::onBuildingDestroyed(BWAPI::Unit unit){
	// if the unit type is in the list of persistent types, try to rebuild it first
	// in general, it may be safer to just rely on missing dependencies
	// so you will only build what is necessary for future builds
	if (std::find(_persistUnitTypes.begin(), _persistUnitTypes.end(), unit->getType()) != _persistUnitTypes.end())
		_buildOrder.insert(_buildOrder.begin(), BuildItem{unit->getType()});
}

void MacroManager::update()
{
	// try building the next build item
	attemptBuildOrder();
	
	// ensure every refinery has three workers harvesting
	for (auto & refineryWorkerPair : _unitStorage.getRefineryUnits()){
		int workersNeeded = 3 - refineryWorkerPair.second;
		// if no workers are needed, skip this refinery
		if (workersNeeded <= 0)
			continue;
		// try to get the workers needed and order them to gather; increment count by actual number acquired
		BWAPI::Unitset workers = _unitStorage.getNUnitsOfCategory(UnitStorage::WORKER, workersNeeded, BWAPI::Filter::IsIdle || !BWAPI::Filter::IsCarryingSomething && !BWAPI::Filter::IsConstructing);
		workers.gather(refineryWorkerPair.first);
		refineryWorkerPair.second += workers.size();
	}

	// delegate remaining idle units to nearest minerals, and return held resources to nearest depot
	for (auto & unit : _unitStorage.getUnits(UnitStorage::WORKER)){
		if (unit->isIdle())
			if (unit->isCarryingGas() || unit->isCarryingMinerals())
				unit->returnCargo();
			else
				unit->gather(unit->getClosestUnit(IsMineralField));
	}
	 
	// if the current flag is still to invade all in, then continue adding units to control groups with that flag
	if (_lastFlag == Flag::ALL_IN&&_unitStorage.getUnits(UnitStorage::COMBAT).size()>0){
		_microManager.createControlGroup(_unitStorage.getUnits(UnitStorage::COMBAT), Flag::ALL_IN);
		_unitStorage.getUnits(UnitStorage::COMBAT).clear();
	}

	updateTargetTech();
}

void MacroManager::updateTargetTech(){
	_targetTech = BuildItem{ BuildMinimax::getTargetTech() };
}

std::vector<BuildItem> MacroManager::getBuildOrder(){
	return _buildOrder;
}

void MacroManager::setBuildOrder(std::vector<BuildItem> buildOrder){
	_buildOrder = buildOrder;
}


void MacroManager::draw(){
	// draw build order list
	int counter = 3;
	for (BuildItem bi : _buildOrder){
		std::string s;
		if (bi.isFlag()){
			s = "Flag";
		}
		else {
			switch (bi.getType()){
			case Build::ItemTypes::UNIT:
				s = bi.getUnit().getName() + std::string(" x") + std::to_string(bi.getMultiplicity());
				break;
			case Build::ItemTypes::TECH:
				s = bi.getTech().getName();
				break;
			case Build::ItemTypes::UPGRADE:
				s = bi.getUpgrade().getName() + std::string(" lvl") + std::to_string(bi.getLevel());
				break;
			}
		}
		Debug::log(s, counter);
		counter++;
		if (counter >= 10){
			break;
		}
	}

	// draw queued minerals, gas, and units
	Debug::log("Queued Minerals: " + std::to_string(_queuedMinerals), 3, 150);
	Debug::log("Queued Gas: " + std::to_string(_queuedGas), 4, 150);
	Debug::log("Queued Units:", 5, 150);
	counter = 6;
	for (auto & unit : _unitStorage.getQueuedUnits()){
		Debug::log(unit->getType().getName(), counter, 150);
		counter++;
	}

	// draw the target tech
	Debug::log("The target tech is " + _targetTech.getUnit().getName(), 10, 100);
}