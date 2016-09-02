/**
	KnowledgeBank.cpp
	Keeps track of enemy unit information as well as tech trees for both players

	@author Paul Wang
	@version 9/8/16
*/

#include "KnowledgeBank.h"

using namespace ProBot;

KnowledgeBank::KnowledgeBank(){}

void KnowledgeBank::handleMyUnitFirstSighting(BWAPI::Unit unit){
	// anything that isn't a supply provider building (and not a resource depot)
	// can be considered a tech unit
	if (unit->getType().isResourceDepot()
		|| (unit->getType().isBuilding() && unit->getType().supplyProvided() <= 0)){
		// update list of tech units and the tech tree
		insertTech(unit->getType(), _myTechUnits);
		BuildMinimax::updateTechTree(_myTechUnits, BuildMinimax::SELF);
	}
}

void KnowledgeBank::handleMyUnitKilled(BWAPI::Unit unit){
	// if unit type exists (and is a positive count) in map then decrement the count by one 
	if ((_myTechUnits.find(unit->getType().getID()) != _myTechUnits.end())
		&& _myTechUnits[unit->getType().getID()] > 0)
		_myTechUnits[unit->getType().getID()]--;
}

void KnowledgeBank::updateEnemyUnits(){
	for (auto &enemy : BWAPI::Broodwar->enemies().getUnits()){
		if (!enemy)
			continue; // do nothing if null ptr
		else if (_knownEnemies.find(enemy->getID()) == _knownEnemies.end())
			handleEnemyUnitFirstSighting(enemy); // haven't seen this unit before, so handle it
		else
			if (enemy->getPosition().isValid()) // ensure that position on map is still valid, not invisible
				_knownEnemies.at(enemy->getID()).update(enemy); // update existing
	}
}

void KnowledgeBank::handleEnemyUnitFirstSighting(BWAPI::Unit enemy){
	// if resource depot, add to list of bases
	if (enemy->getType().isResourceDepot()){
		_enemyBases.push_back(EnemyUnit{ enemy });
		updateClosestEnemyBase();
		Debug::message("Found an enemy base!");
	}

	// anything that isn't a supply provider building (and not a resource depot)
	// can be considered a tech unit
	if (enemy->getType().isResourceDepot()
		|| (enemy->getType().isBuilding() && enemy->getType().supplyProvided() <= 0)){
		insertTech(enemy->getType(), _enemyTechUnits);
		BuildMinimax::updateTechTree(_enemyTechUnits, BuildMinimax::ENEMY);
	}

	// add unit to list of all known enemies
	EnemyUnit enemyUnitInfo(enemy);
	std::pair<unit_id_t, EnemyUnit> idEnemyPair(enemy->getID(), enemyUnitInfo);
	_knownEnemies.insert(idEnemyPair);
}

void KnowledgeBank::handleEnemyUnitKilled(BWAPI::Unit enemy){
	if (enemy->getType().isResourceDepot()){
		// searches for the base in the list and removes it if found
		for (int i = 0; i < static_cast<int>(_enemyBases.size()); i++)
			if (_enemyBases.at(i).getUnitId() == enemy->getID())
				_enemyBases.erase(_enemyBases.begin() + i);
		updateClosestEnemyBase();
	}
	_knownEnemies.erase(enemy->getID());
}

void KnowledgeBank::insertTech(BWAPI::UnitType techUnitType, tech_count_map_t & techCountMap){
	// set count to 1 if first instance of key, otherwise increment by 1
	if (techCountMap.find(techUnitType.getID()) == techCountMap.end())
		techCountMap[techUnitType.getID()] = 1;
	else
		techCountMap[techUnitType.getID()]++;
}

int KnowledgeBank::evaluateEnemyArmyStrength(){
	int eval = 0;
	for (auto & enemyUnit : _knownEnemies){
		eval += CombatUtil::getStaticUnitScore(enemyUnit.second.getUnitType());
	}
	return eval;
}

int KnowledgeBank::evaluateMyArmyStrength(){
	int eval = 0;
	for (auto & myUnit : BWAPI::Broodwar->self()->getUnits()){
		eval += CombatUtil::getStaticUnitScore(myUnit->getType());
	}
	return eval;
}

bool KnowledgeBank::shouldNotFight(){
	return evaluateEnemyArmyStrength() > evaluateMyArmyStrength() + ARMY_STRENGTH_BUFFER;
}

BWAPI::Position KnowledgeBank::getClosestEnemyBase(){
	// set closest enemy base to default start location of enemy
	if (_closestEnemyBase == BWAPI::Positions::Unknown){
		// find default start location by searching through list of start locations for map
		for (auto & point : BWAPI::Broodwar->getStartLocations()){
			if (point != BWAPI::Broodwar->self()->getStartLocation()){
				_closestEnemyBase = BWAPI::Position(point);
			}
		}
	}
	return _closestEnemyBase;
}

void KnowledgeBank::updateClosestEnemyBase(){
	// remove dead bases
	_enemyBases.erase(std::remove_if(_enemyBases.begin(), _enemyBases.end(), [](EnemyUnit base){ return base.getLastKnownHealth() <= 0; }), _enemyBases.end());

	int smallestDistance = INT_MAX;
	EnemyUnit closestBase = NULL_ENEMY;

	// compare distance of each known base to own start location
	for (EnemyUnit & base : _enemyBases){
		int distance = base.getLastKnownPos().getApproxDistance(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()));
		if (distance < smallestDistance){
			smallestDistance = distance;
			closestBase = base;
		}
	}

	_closestEnemyBase = closestBase.getLastKnownPos();
}

EnemyUnit& KnowledgeBank::getEnemyDataByID(unit_id_t id){
	return _knownEnemies.at(id);
}

tech_count_map_t KnowledgeBank::getEnemyTech(){
	return _enemyTechUnits;
}

tech_count_map_t KnowledgeBank::getMyTech(){
	return _myTechUnits;
}

int KnowledgeBank::getNumberOfEnemiesOfType(BWAPI::UnitType type){
	if (type.isBuilding()){ // if it's a tech unit then just return value in map
		return _enemyTechUnits[type.getID()]; // defaults to 0
	}
	else { // else search through list of known enemies and sum
		int counter = 0;
		for (auto & enemyUnit : _knownEnemies){
			if (enemyUnit.second.getUnitType().getID() == type.getID()){
				counter++;
			}
		}
		return counter;
	}
}

void KnowledgeBank::draw(){
	// draw name and hp info for each enemy on the map (at their last known positions)
	for (auto it = _knownEnemies.begin(); it != _knownEnemies.end(); ++it){
		BWAPI::Color color = Debug::BLUE;
		if (it->second.getUnitType().isResourceDepot()){
			color = Debug::RED;
		}
		std::string s = it->second.getUnitType().getName();
		s.append(" | HP: " + std::to_string(it->second.getLastKnownHealth()));
		Debug::drawBoxWithLabel(s, it->second.getLastKnownPos(), color);
	}

	// draw scouted tech as a list
	int counter = 3;
	Debug::log("Scouted tech:", counter, 500);
	for (auto & techCountPair : _enemyTechUnits){
		counter++;
		Debug::log(BWAPI::UnitType(techCountPair.first).getName() + " x " + std::to_string(techCountPair.second), counter, 500);
	}

	// draw own tech as a list
	counter = 3;
	Debug::log("My tech:", counter, 350);
	for (auto & techCountPair : _myTechUnits){
		counter++;
		Debug::log(BWAPI::UnitType(techCountPair.first).getName() + " x " + std::to_string(techCountPair.second), counter, 350);
	}
}