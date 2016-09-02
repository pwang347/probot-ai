/**
	GameManager.cpp
	Receives events passed down by the AI module and delegates these to the respective managers

	@author Paul Wang
	@version 9/8/16
*/

#include "GameManager.h"

using namespace ProBot;
using namespace BWAPI;
using namespace Filter;

void GameManager::update()
{
	// draws debug information if enabled
	if (Constants::DEBUG_ENABLED)
	draw();
	
	// ignore unusable frames
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;

	// make sure some frames pass before updating game information
	if (Broodwar->getFrameCount() % FRAMES_BEFORE_UPDATE == 0)
	{
		_macroManager.getKnowledgeBank()->updateEnemyUnits();
		_macroManager.update();
		_macroManager.getMicroManager()->update();

		// check if any of our units are under attack, if so, fire event
		for (auto & unit : Broodwar->self()->getUnits()){
			if (unit->isUnderAttack())
				onUnitUnderAttack(unit);
		}
	}
}

void GameManager::draw(){
	// draw each starting location on the map
	for (auto points : Broodwar->getStartLocations())
		Debug::drawBoxWithLabel("START", BWAPI::Position(points), Debug::RED);

	// show the current issued order for each selected unit, if they are owned
	for (auto &unit : Broodwar->getSelectedUnits()){
		if (!unit)
			continue;
		if (IsOwned(unit))
			Debug::trackUnit(unit);
	}

	// call draw functions for sub-managers
	_macroManager.getUnitStorage()->draw();
	_macroManager.getKnowledgeBank()->draw();
	_macroManager.draw();
	_macroManager.getMicroManager()->draw();
}

void GameManager::onUnitUnderAttack(BWAPI::Unit unit){
	// @TODO
}

void GameManager::onNukeDetect(BWAPI::Position target){
	// check if the target is a valid position
	if (target)
	{
		int blastRadius = Nuclear_Strike.getWeapon().outerSplashRadius();
		for (auto & unit : Broodwar->getUnitsInRadius(target, blastRadius))
			// move unit away from blast center to distance blastRadius
			unit->move(unit->getPosition()
			+ MovementUtil::getPointFromAngleDist(MovementUtil::getAngleBetweenPositions(unit->getPosition(), target)
			, blastRadius));
	}
}

void GameManager::onUnitCreate(BWAPI::Unit unit){
	if (IsOwned(unit)){
		_macroManager.getUnitStorage()->handleUnitCreate(unit);
		if (IsBuilding(unit))
			_macroManager.onBuildingCreated(unit);
	}
}

void GameManager::onUnitDestroy(BWAPI::Unit unit){
	if (IsOwned(unit)){
		_macroManager.getUnitStorage()->handleUnitDestroy(unit);
		_macroManager.getKnowledgeBank()->handleMyUnitKilled(unit);
		if (IsBuilding(unit))
			_macroManager.onBuildingDestroyed(unit);
	}
	else {
		_macroManager.getKnowledgeBank()->handleEnemyUnitKilled(unit);
	}
}

void GameManager::onUnitMorph(BWAPI::Unit unit){
	if (IsOwned(unit) && IsBuilding(unit) && IsRefinery(unit))
		_macroManager.onBuildingCreated(unit);
}

void GameManager::onUnitComplete(BWAPI::Unit unit){
	if (IsOwned(unit)){
		_macroManager.getUnitStorage()->handleUnitComplete(unit);
		_macroManager.getKnowledgeBank()->handleMyUnitFirstSighting(unit);
	}
}

void GameManager::onUnitEvade(BWAPI::Unit unit){}
void GameManager::onUnitDiscover(BWAPI::Unit unit){}
void GameManager::onUnitShow(BWAPI::Unit unit){}
void GameManager::onUnitHide(BWAPI::Unit unit){}
void GameManager::onUnitRenegade(BWAPI::Unit unit){}