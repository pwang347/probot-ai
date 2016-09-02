/**
	MicroManager.cpp
	Manager class for managing control groups.

	@author Paul Wang
	@version 9/8/16
*/

#include "MicroManager.h"

using namespace ProBot;

MicroManager::MicroManager(){
}

MicroManager::MicroManager(KnowledgeBank * knowledgeBank){
	_knowledgeBank = knowledgeBank;
}

void MicroManager::update()
{
	validateGroups();
	for (int i = 0; i < static_cast<int>(_controlGroups.size()); i++){	
		_controlGroups.at(i).update(_knowledgeBank);
		for (int j = 0; j < static_cast<int>(_controlGroups.size()); j++){
			if (j != i){
				ControlGroup group1 = _controlGroups.at(i);
				ControlGroup group2 = _controlGroups.at(j);
				if (group1.getFlag() == group2.getFlag()
					&& (group1.getUnits().getPosition().getApproxDistance(group2.getUnits().getPosition())) < 1000){
					mergeControlGroups(group1, group2);
				}
			} 
		}
	}
}

void MicroManager::mergeControlGroups(ControlGroup group1, ControlGroup group2){
	group1.addUnits(group2.getUnits());
	removeControlGroup(group2);
}

void MicroManager::mergeAllControlGroups(ControlGroup group){
	for (auto & aGroup : _controlGroups){
		if (!(group == aGroup)){
			group.addUnits(aGroup.getUnits());
		}
		removeControlGroup(aGroup);
	}
}

void MicroManager::removeControlGroup(ControlGroup group){
	auto it = std::find(_controlGroups.begin(), _controlGroups.end(), group);
	if (it != _controlGroups.end()) {
		std::iter_swap(it, _controlGroups.end() - 1);
		_controlGroups.erase(_controlGroups.end() - 1);
	}
}

void MicroManager::validateGroups(){
	_controlGroups.erase(
		std::remove_if(
		_controlGroups.begin(),
		_controlGroups.end(),
		[](ControlGroup group) -> bool {
		return (group.getUnits().size() <= 0);
		}
		),
		_controlGroups.end()
		);
}

void MicroManager::createControlGroup(BWAPI::Unitset units, Flag flag){
	for (ControlGroup & group : _controlGroups){
		// if an existing group has the same objective and the units are nearby, then merge units into group
		if (group.getFlag() == flag && group.getUnits().size()
			&& units.getPosition().getApproxDistance(group.getUnits().getPosition()) < 1000){
			group.addUnits(units);
			return;
		}
	} // otherwise create a new group
	ControlGroup newGroup{ units, flag };
	_controlGroups.push_back(newGroup);
}

void MicroManager::createControlGroup(BWAPI::Unit unit, Flag flag){
	BWAPI::Unitset units;
	units.insert(unit);
	createControlGroup(units, flag);
}

std::vector<ControlGroup> MicroManager::getControlGroups(){
	return _controlGroups;
}

void MicroManager::issueGlobalOrder(Flag flag){
	for (ControlGroup group : _controlGroups)
		group.overrideFlag(flag);
}

void MicroManager::issueLocalOrder(Flag flag, BWAPI::Position pos, int radius){
	for (ControlGroup group : _controlGroups){
		if (pos.getApproxDistance(group.getUnits().getPosition()) <= radius)
		group.overrideFlag(flag);
	}
}

void MicroManager::draw(){
	int counter = 10;
	for (ControlGroup group : getControlGroups()){
		Debug::log("Control group " + std::to_string(counter - 9) + " size : " + std::to_string(group.getUnits().size()), counter, 200);
		counter++;
	}
}