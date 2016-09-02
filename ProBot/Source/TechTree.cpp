/**
	BuildMinimax.cpp
	Utility functions for tree traversal and mutating nodes

	@author Paul Wang
	@version 9/8/16
*/


#include "TechTree.h"

using namespace ProBot;

TechTree::TechTree(const BWAPI::UnitType unitType,
	bool obtained,
	bool isUnique,
	std::vector<TechTree> subs){
	_unitType = unitType;  _obtained = obtained; _isUnique = isUnique; _subs = subs;
}

TechTree::TechTree(const BWAPI::UnitType unitType, bool obtained, std::vector<TechTree> subs){
	_unitType = unitType;  _subs = subs; _obtained = obtained; _isUnique = true;
}

TechTree::TechTree(const BWAPI::UnitType unitType, std::vector<TechTree> subs){
	_unitType = unitType;  _subs = subs; _obtained = false; _isUnique = true;
}

bool TechTree::hasSubs(){
	return _subs.size() > 0;
}

bool ProBot::operator==(TechTree first, TechTree other){
	return first._unitType.getID() == other._unitType.getID();
}

bool ProBot::operator!=(TechTree first, TechTree other){
	return !(first == other);
}

TechTree * ProBot::findNode(BWAPI::UnitType nodeUnitType, TechTree & tree, bool ignoreUnobtained){
	if (tree._unitType.getID() == nodeUnitType.getID()) // if found node
		return &tree;
	if (!tree.hasSubs() || (ignoreUnobtained && !tree._obtained)) // if nothing left to check in current branch
		return nullptr;
	for (int i = 0; i < static_cast<int>(tree._subs.size()); i++){ // search through children recursively
		TechTree * res = findNode(nodeUnitType, tree._subs.at(i), ignoreUnobtained);
		if (res != nullptr){ // non-trivial result found
			return res;
		}
	}
	return nullptr; // all searches returned null in loop
}

TechTree * ProBot::findNode(TechTree * node, TechTree & tree, bool ignoreUnobtained){
	if (&tree == node) // if found node
		return &tree;
	if (!tree.hasSubs() || (ignoreUnobtained && !tree._obtained)) // if nothing left to check in current branch
		return nullptr;
	for (int i = 0; i < static_cast<int>(tree._subs.size()); i++){ // search through children recursively
		TechTree * res = findNode(node, tree._subs.at(i), ignoreUnobtained);
		if (res != nullptr){
			return res; // non-trivial result found
		}
	}
	return nullptr; // all searches returned null in loop
}

TechTree * ProBot::getImmediateParent(BWAPI::UnitType nodeUnitType, TechTree & tree, bool ignoreUnobtained){
	if (ignoreUnobtained && !tree._obtained) // if nothing left to check in current branch
		return nullptr;
	for (int i = 0; i < static_cast<int>(tree._subs.size()); i++){ // search through children recursively
		if (tree._subs.at(i)._unitType.getID() == nodeUnitType.getID())
			return &tree; // found the unit type in children, so return the current node
		TechTree * res = getImmediateParent(nodeUnitType, tree._subs.at(i), ignoreUnobtained);
		if (res != nullptr)
			return res; // non-trivial result found
	}
	return nullptr; // all searches returned null in loop
}

std::vector<TechTree*> ProBot::findAllNodesOfType(BWAPI::UnitType nodeUnitType, TechTree & tree, std::vector<TechTree*> rsf){
	if (tree._unitType.getID() == nodeUnitType.getID()) // if found node, add to rsf
		rsf.push_back(&tree);
	for (int i = 0; i < static_cast<int>(tree._subs.size()); i++){ // search through children recursively
		std::vector<TechTree*> res = findAllNodesOfType(nodeUnitType, tree._subs.at(i), rsf);
		for (auto & nodeRef : res)
			rsf.push_back(nodeRef); // add each element in temp result to rsf
	}
	return rsf;
}

bool ProBot::setNodeObtained(BWAPI::UnitType nodeUnitType, TechTree & tree, bool obtained){
	TechTree * foundNode = findNode(nodeUnitType, tree);
	if (foundNode != nullptr){
		foundNode->_obtained = obtained;
		return true;
	}
	else {
		return false;
	}
}

bool ProBot::setNodeObtained(TechTree * node, TechTree & tree, bool obtained){
	TechTree * foundNode = findNode(node, tree);
	// make sure that the node was found
	if (foundNode != nullptr){
		foundNode->_obtained = obtained;
		return true;
	}
	else {
		return false;
	}
}

bool ProBot::setBuildingNodeObtained(BWAPI::UnitType nodeBuildingUnitType, TechTree & tree, bool obtained){
	TechTree * foundNode = findNode(nodeBuildingUnitType, tree, obtained);
	// make sure that the node was found
	if (foundNode != nullptr){
		foundNode->_obtained = obtained;
		
		if (!foundNode->_isUnique){
			removeNode(foundNode->_unitType, tree, !obtained);
		}
		
		for (TechTree & childUnits : foundNode->_subs){
			if(!childUnits._unitType.isBuilding())
				childUnits._obtained = obtained;
		}
		return true;
	}
	else {
		return false;
	}
}

bool ProBot::setPrereqsObtained(BWAPI::UnitType nodeUnitType, TechTree & tree, bool obtained){
	// find prereqs, and then set each of their obtained properties
	for (auto & prereqs : findPrereqNodes(nodeUnitType, tree)){
		if (!setNodeObtained(prereqs, tree, obtained))
			return false;
	}
	return true;
}

bool ProBot::setNonUniqueNode(TechTree * node, TechTree & tree, bool obtained){
	// set the specific node first
	if (!setNodeObtained(node, tree, obtained))
		return false;
	// then remove the other node with the same unit type
	if (!removeNode(node->_unitType, tree, !obtained))
		return false;
	return true;
}

bool ProBot::removeNode(BWAPI::UnitType nodeUnitType, TechTree & tree, bool obtained){
	for (int i = 0; i < static_cast<int>(tree._subs.size()); i++){ // search through children recursively		
		if (tree._subs.at(i)._unitType.getID() == nodeUnitType.getID()
			&& tree._subs.at(i)._obtained == obtained){ // if it's the right unit type and obtained value
			tree._subs.erase(tree._subs.begin() + i);
			return true;
		}
		else {
			bool res = removeNode(nodeUnitType, tree._subs.at(i), obtained);
			if (res)
				return true; // if trivial result was true, return it
		}
	}
	return false;
}

bool ProBot::removeNode(TechTree * node, TechTree & tree){
	for (int i = 0; i < static_cast<int>(tree._subs.size()); i++){ // search through children recursively		
		if (&tree._subs.at(i) == node){
			tree._subs.erase(tree._subs.begin() + i); // if found the node in children, remove it
			return true;
		}
		else {
			bool res = removeNode(node, tree._subs.at(i));
			if (res)
				return true; // if trivial result was true, return it
		}
	}
	return false;
}

TechTree ProBot::getBaseTree(BWAPI::Race race){
	if (race == BWAPI::Races::Protoss){
		return PROTOSS_TREE;
	}
	else if (race == BWAPI::Races::Terran){
		return TERRAN_TREE;
	}
	else if (race == BWAPI::Races::Zerg){
		return ZERG_TREE;
	}
	else {
		return NULL_TREE;
	}
}

std::vector<TechTree> ProBot::findLeafNodes(TechTree tree, std::vector<TechTree> resultSoFar){
	if (!tree.hasSubs()) // if leaf node found, add it to rsf
		resultSoFar.push_back(tree);
	for (int i = 0; i < static_cast<int>(tree._subs.size()); i++) // search through children recursively for leaf nodes
		resultSoFar = findLeafNodes(tree._subs.at(i), resultSoFar);
	return resultSoFar; // return results
}

std::vector<TechTree> ProBot::findNonBuildingLeafNodes(TechTree tree){
	// get all leaf nodes
	std::vector<TechTree> leafNodes = findLeafNodes(tree);
	// remove non-obtained nodes
	leafNodes.erase(std::remove_if(leafNodes.begin(), leafNodes.end(), [](TechTree tree){ return !tree._obtained; }), leafNodes.end());
	// remove non-building nodes
	leafNodes.erase(std::remove_if(leafNodes.begin(), leafNodes.end(), [](TechTree tree){ return tree._unitType.isBuilding(); }), leafNodes.end());
	return leafNodes;
}

std::vector<TechTree> ProBot::findNextObtainableNodes(TechTree tree, std::vector<TechTree> resultSoFar){
	if (!tree._obtained) // if node is obtained, add to results
		resultSoFar.push_back(tree);
	else 
		for (int i = 0; i < static_cast<int>(tree._subs.size()); i++) // if any child is also obtained, remove from vector and check that child
			resultSoFar = findNextObtainableNodes(tree._subs.at(i), resultSoFar);
	return resultSoFar; // return results
}

BWAPI::UnitType ProBot::getFirstDifference(TechTree tree1, TechTree tree2){
	if (tree1._obtained != tree2._obtained) // if the two obtained values are different, then return the unit type
		return tree1._unitType;
	for (int i = 0; i < static_cast<int>(tree1._subs.size()); i++){ // otherwise check recursively through children
		BWAPI::UnitType res = getFirstDifference(tree1._subs.at(i), tree2._subs.at(i));
		if (res.getName() != BWAPI::UnitTypes::None.getName()) // ignore the null node at the base
			return res;
	}
	return BWAPI::UnitTypes::None; // there was only the null node for both trees
}

std::vector<BWAPI::UnitType> ProBot::findPrereqNodes(BWAPI::UnitType nodeUnitType,
	TechTree tree,
	std::vector<BWAPI::UnitType> path){
	if (!tree.hasSubs()){ // if nothing left to check in current branch, return nothing
		path.clear();
		return path;
	}
	path.push_back(tree._unitType);
	for (int i = 0; i < static_cast<int>(tree._subs.size()); i++){ // search through children recursively		
		if (tree._subs.at(i)._unitType.getID() == nodeUnitType.getID()){
			return path; // if found the correct node, return current path
		}
		else {
			std::vector<BWAPI::UnitType> res = findPrereqNodes(nodeUnitType, tree._subs.at(i), path);
			if (res.size() > 0)
				return res; // non-trivial result found
		}
	}
	path.clear();
	return path; // nothing found
}

std::vector<BWAPI::UnitType> ProBot::getUnitPrereqs(BWAPI::UnitType nodeUnitType, TechTree tree){
	std::vector<BWAPI::UnitType> prereqs = findPrereqNodes(nodeUnitType, tree);
	// get rid of the null node
	prereqs.erase(prereqs.begin());
	return prereqs;
}

std::vector<BWAPI::UnitType> ProBot::getObtainedNodes(TechTree tree, std::vector<BWAPI::UnitType> rsf){
	if (tree._obtained)
		rsf.push_back(tree._unitType); // base case, if obtained add to list size 1
	for (TechTree & sub : tree._subs) // check each child to see if the units should be added
		rsf = getObtainedNodes(sub, rsf);
	return rsf;
}