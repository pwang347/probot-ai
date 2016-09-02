/**
	BuildMinimax.h
	Utility functions for tree traversal and mutating nodes

	@author Paul Wang
	@version 9/8/16
*/

#pragma once

#include <BWAPI.h>
#include "Debug.h"

using namespace BWAPI::UnitTypes;

namespace ProBot{
		/**
			A struct representing a tech tree node.
		*/
		struct TechTree {
			TechTree(const BWAPI::UnitType unitType = BWAPI::UnitTypes::None,
				bool obtained = false,
				bool isUnique = true,
				std::vector<TechTree> subs = std::vector<TechTree>());
			TechTree(const BWAPI::UnitType unitType, bool obtained, std::vector<TechTree> subs);
			TechTree(const BWAPI::UnitType unitType, std::vector<TechTree> subs);
			BWAPI::UnitType _unitType;
			bool _obtained;
			bool _isUnique;
			std::vector<TechTree> _subs;
			bool hasSubs();
		};

		// constant used for checks in back-track searches
		const TechTree NULL_TREE = TechTree{ BWAPI::UnitTypes::Unknown };

		/**
			Equality operator for TechTree.
		*/
		bool operator==(TechTree first, TechTree other);
		/**
			Inequality operator for TechTree.
		*/
		bool operator!=(TechTree first, TechTree other);

		/**
			Returns the TechTree associated with a race, where each node is initialized with obtained = false. 
		*/
		TechTree getBaseTree(BWAPI::Race);
		/**
			Returns a reference to the node representation of a given unit type in a tree.
			@param nodeUnitType - the unit type to search for
			@param tree - the tree in which the node exists
			@param ignoreUnobtained - reduce searching to only obtained nodes if set to true
			@return the reference to the node of the unit type in the tree
		*/
		TechTree * findNode(BWAPI::UnitType nodeUnitType, TechTree & tree, bool ignoreUnobtained = false);
		/**
			Returns a reference to the found node in a tree.
			Used when there are multiple instances of a unit type in a given tree.
			@param node - the reference to a node to search for in the tree
			@param tree - the tree in which the node exists
			@param ignoreUnobtained - reduce searching to only obtained nodes if set to true
			@return the reference to the node of the unit type in the tree
		*/
		TechTree * findNode(TechTree * node, TechTree & tree, bool ignoreUnobtained = false);
		/**
			Returns the parent node of the given unit type.
			@param nodeUnitType - the unit type whose parent we must search for
			@param tree - the tree in which the nodes exist
			@param ignoreUnobtained - reduce searching to only obtained nodes if set to true
			@return the reference to the node of the parent of the unit type in the tree
		*/
		TechTree * getImmediateParent(BWAPI::UnitType nodeUnitType, TechTree & tree, bool ignoreUnobtained = false);
		/**
			Finds all nodes with the given unit type in a tree.
			@param nodeUnitType - the unit type to search for
			@param tree - the tree in which the node exists
			@param ignoreUnobtained - reduce searching to only obtained nodes if set to true
			@return all nodes with the provided unit type
		*/
		std::vector<TechTree*> findAllNodesOfType(BWAPI::UnitType nodeUnitType, TechTree & tree, std::vector<TechTree*> rsf = std::vector<TechTree*>());
		/**
			Finds the node with unit type in the tree and sets its obtained property.
			@param nodeUnitType - the unit type to search for
			@param tree - the tree in which the node exists
			@param obtained - the value to set obtained to
			@return true if successful in finding node and setting value, false otherwise
		*/
		bool setNodeObtained(BWAPI::UnitType nodeUnitType, TechTree & tree, bool obtained = true);
		/**
			Finds a node in the tree and sets its obtained property.
			Used when there are multiple instances of a unit type in a given tree.
			@param node - the reference to the specific node
			@param tree - the tree in which the node exists
			@param obtained - the value to set obtained to
			@return true if successful in finding node and setting value, false otherwise
		*/
		bool setNodeObtained(TechTree * node, TechTree & tree, bool obtained = true);
		/**
			Finds the node with unit type in the tree and sets its and its children's obtained property. 
			@param nodeBuildingUnitType - the unit type to search for
			@param tree - the tree in which the node exists
			@param obtained - the value to set obtained to
			@return true if successful in finding node and setting value, false otherwise
		*/
		bool setBuildingNodeObtained(BWAPI::UnitType nodeBuildingUnitType, TechTree & tree, bool obtained = true);
		/**
			Finds the node with unit type in the tree and sets its and its prereqs obtained property.
			@param nodeUnitType - the unit type to search for
			@param tree - the tree in which the node exists
			@param obtained - the value to set obtained to
			@return true if successful in finding node and setting value, false otherwise
		*/
		bool setPrereqsObtained(BWAPI::UnitType nodeUnitType, TechTree & tree, bool obtained = true);
		/**
			Finds the specific node in the tree and sets its obtained property.
			Removes any other nodes in the tree with the same unit type but that aren't set to be obtained.
			@param nodeUnitType - the unit type to search for
			@param tree - the tree in which the node exists
			@param obtained - the value to set obtained to
			@return true if successful in finding node and setting value, false otherwise
		*/
		bool setNonUniqueNode(TechTree * node, TechTree & tree, bool obtained = true);
		/**
			Removes the node for the given unit type.
			ASSUMPTION: the base tree node has at least one child (if not, always returns false).
			@param nodeUnitType - the unit type to remove
			@param tree - the tree in which the node exists
			@param obtained - only remove if the node has obtained value equal to this bool
			@return true if successful in removing node, false otherwise
		*/
		bool removeNode(BWAPI::UnitType nodeUnitType, TechTree & tree, bool obtained = true);
		/**
			Removes a specific node in the tree.
			ASSUMPTION: the base tree node has at least one child (if not, always returns false).
			@param node - the node to remove
			@param tree - the tree in which the node exists
			@return true if successful in removing node, false otherwise
		*/
		bool removeNode(TechTree * node, TechTree & tree);
		/**
			Returns a list of leaf nodes.
			Leaf nodes are nodes that do not have subs.
			@param tree - the tree in which the nodes exist
			@param resultSoFar - an accumulator used to store temporary result in recursive calls
			@param return the list of found leaf nodes.
		*/
		std::vector<TechTree> findLeafNodes(TechTree tree, std::vector<TechTree> resultSoFar = std::vector<TechTree>());
		/**
			Returns a list of leaf nodes that are not building units.
			Leaf nodes are nodes that do not have subs.
			@param tree - the tree in which the nodes exist
			@param return the list of found non-building leaf nodes.
		*/
		std::vector<TechTree> findNonBuildingLeafNodes(TechTree tree);
		/**
			Returns a list of nodes that are children of obtained nodes, but are not themselves obtained.
			@param tree - the tree in which the nodes exist
			@param resultSoFar - an accumulator used to store temporary result in recursive calls
			@param return the list of found next obtainable nodes
		*/
		std::vector<TechTree> findNextObtainableNodes(TechTree tree, std::vector<TechTree> resultSoFar = std::vector<TechTree>());
		/**
			Returns the first found unit type that is obtained in one tree but not the other.
			ASSUMPTION: both trees are the same base tree.
			@param tree1 - the first tech tree
			@param tree2 - the second tech tree
			@param return the unit type with different obtained value in the two trees
		*/
		BWAPI::UnitType getFirstDifference(TechTree tree1, TechTree tree2);
		/**
			Returns a list of pre-requisite nodes that form a path to reach the node representing the given unit type.
			@param nodeUnitType - the unit type to search for
			@param tree - the tree in which the node exists
			@param path - an accumulator used to store temporary result in recursive calls
			@param return the list of found prereq nodes
		*/
		std::vector<BWAPI::UnitType> findPrereqNodes(BWAPI::UnitType nodeUnitType, TechTree tree, std::vector<BWAPI::UnitType> path = std::vector<BWAPI::UnitType>());
		/**
			Returns pre-requisite nodes and removes the highest level null node for each tree.
		*/
		std::vector<BWAPI::UnitType> getUnitPrereqs(BWAPI::UnitType, TechTree);
		/**
			Returns all nodes with obtained property set to true.
		*/
		std::vector<BWAPI::UnitType> getObtainedNodes(TechTree, std::vector<BWAPI::UnitType> = std::vector<BWAPI::UnitType>());

		// CONSTANTS
		// PROTOSS TECH TREE
		const TechTree PROTOSS_ROBOTICS_FACILITY{
			Protoss_Robotics_Facility, {
				TechTree{ Protoss_Robotics_Support_Bay, { TechTree{ Protoss_Reaver } } },
				TechTree{ Protoss_Observatory, { TechTree{ Protoss_Observer } } },
				TechTree{ Protoss_Shuttle } }
		};

		const TechTree PROTOSS_STARGATE{
			Protoss_Stargate, false, false, {
				TechTree{ Protoss_Fleet_Beacon, {
					TechTree{ Protoss_Citadel_of_Adun, {
						TechTree{ Protoss_Templar_Archives, {
							TechTree{ Protoss_High_Templar },
							TechTree{ Protoss_Dark_Templar },
							TechTree{ Protoss_Archon },
							TechTree{ Protoss_Dark_Archon },
							TechTree{ Protoss_Arbiter_Tribunal, {
								TechTree{ Protoss_Arbiter } }
							}
						} } } },
						TechTree{ Protoss_Carrier } } },
				TechTree{Protoss_Scout},
				TechTree{Protoss_Corsair}
			}
		};

		const TechTree PROTOSS_CITADEL_OF_ADUN{
			Protoss_Citadel_of_Adun, false, false, {
				TechTree{ Protoss_Templar_Archives, {
					TechTree{ Protoss_High_Templar },
					TechTree{ Protoss_Dark_Templar },
					TechTree{ Protoss_Archon },
					TechTree{ Protoss_Dark_Archon },
					TechTree{ Protoss_Stargate, {
						TechTree{ Protoss_Fleet_Beacon, {
							TechTree{ Protoss_Carrier } } },
							TechTree{ Protoss_Arbiter_Tribunal, {
								TechTree{ Protoss_Arbiter } } },
								TechTree{ Protoss_Scout },
								TechTree{ Protoss_Corsair }
					} } } } }
		};

		const TechTree PROTOSS_TREE{
			BWAPI::UnitTypes::None, {
				TechTree{ Protoss_Nexus, {
					TechTree{ Protoss_Gateway, {
						TechTree{ Protoss_Cybernetics_Core, {
							TechTree{ Protoss_Dragoon }, PROTOSS_ROBOTICS_FACILITY, PROTOSS_STARGATE, PROTOSS_CITADEL_OF_ADUN
						} }
						, TechTree{ Protoss_Zealot }
						, TechTree{ Protoss_Shield_Battery }
					} }
					, TechTree{ Protoss_Forge, { TechTree{ Protoss_Photon_Cannon } } }
					, TechTree{ Protoss_Probe }
				} },
				TechTree{ Protoss_Pylon },
				TechTree{ Protoss_Assimilator }
			}
		};

		// TERRAN TECH TREE
		const TechTree TERRAN_STARPORT{
			Terran_Starport, {
				TechTree{ Terran_Science_Facility, { TechTree{ Terran_Physics_Lab }, TechTree{Terran_Covert_Ops} } },
				TechTree{ Terran_Control_Tower, { TechTree{ Terran_Dropship }, TechTree{ Terran_Armory, { TechTree{ Terran_Valkyrie } } } } },
				TechTree{ Terran_Wraith}
			}
		};

		const TechTree TERRAN_ACADEMY{
			Terran_Academy, { TechTree{ Terran_Firebat }, TechTree{ Terran_Medic }, TechTree{ Terran_Comsat_Station }}
		};

		const TechTree TERRAN_ARMORY{
			Terran_Armory, { TechTree{ Terran_Goliath }, TechTree{ Terran_Starport, {
				TechTree{ Terran_Science_Facility, { TechTree{ Terran_Physics_Lab }, TechTree{ Terran_Covert_Ops } } },
				TechTree{ Terran_Control_Tower, { TechTree{ Terran_Dropship } } },
				TechTree{ Terran_Wraith } } } }
		};

		const TechTree TERRAN_TREE{
			BWAPI::UnitTypes::None, {
				TechTree{ Terran_Command_Center, {
					TechTree{ Terran_Barracks, {
						TechTree{ Terran_Factory, {
							TERRAN_STARPORT, TERRAN_ARMORY,
							TechTree{ Terran_Vulture }
						} }
						, TechTree{ Terran_Bunker }
						, TERRAN_ACADEMY
						, TechTree{ Terran_Marine }
					} }
					, TechTree{ Terran_Engineering_Bay, { TechTree{ Terran_Missile_Turret } } }
					, TechTree{ Terran_SCV }
				} },
				TechTree{ Terran_Supply_Depot },
				TechTree{ Terran_Refinery }
			}
		};

		// @TODO
		// ZERG TECH TREE
		const TechTree ZERG_TREE{
			BWAPI::UnitTypes::None, {
				TechTree{ Protoss_Nexus, {
					TechTree{ Protoss_Gateway, {
						TechTree{ Protoss_Cybernetics_Core, {
							TechTree{ Protoss_Dragoon }, PROTOSS_ROBOTICS_FACILITY, PROTOSS_STARGATE, PROTOSS_CITADEL_OF_ADUN
						} }
						, TechTree{ Protoss_Zealot }
						, TechTree{ Protoss_Shield_Battery }
					} }
					, TechTree{ Protoss_Forge, { TechTree{ Protoss_Photon_Cannon } } }
					, TechTree{ Protoss_Probe }
				} },
				TechTree{ Protoss_Pylon },
				TechTree{ Protoss_Assimilator }
			}
		};
	}