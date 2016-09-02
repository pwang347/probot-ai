/**
	BuildMinimax.h
	Implementation of minimax (with and without alpha-beta pruning) for choosing next tech to rush.

	@author Paul Wang
	@version 9/8/16
*/

#pragma once

#include "Debug.h"
#include "TechTree.h"

namespace ProBot{
	typedef std::pair<TechTree, int> tree_score_pair_t;
	typedef int count_t;
	typedef int unit_type_id_t;
	typedef std::unordered_map<unit_type_id_t, count_t> tech_count_map_t;

	namespace BuildMinimax{
		// default search depth for minimax algorithm
		const int DEFAULT_DEPTH = 2;

		// enumeration of players (assumes 1v1 match)
		enum PLAYER_ID {
			SELF,
			ENEMY,
			PLAYER_ID_SIZE
		};
		
		// the target technology to research, as determined by minimax
		static BWAPI::UnitType _targetTech = BWAPI::UnitTypes::None;

		// 
		static TechTree _gameTreeState[2] = {NULL_TREE, NULL_TREE};

		/**
			Finds optimal techtree for given player by searching the game-tree to a given depth.

			@param depth - the remaining depth in which the game-tree will be searched
			@param player - the player for the current "turn"
			@return a tree-int pair representing the optimal tech tree and its corresponding score
		*/
		tree_score_pair_t minimax(int depth, int player);
		
		/**
			Finds optimal techtree for given player by searching the game-tree to a given depth.
			Alpha-beta pruning implementation.

			@param depth - the remaining depth in which the game-tree will be searched
			@param player - the player for the current "turn"
			@param alpha - the max value found so far for the maximizing node
			@param beta - the min value found so far for the minimizing node
			@return a tree-int pair representing the optimal tech tree and its corresponding score
		*/
		tree_score_pair_t minimaxAlphaBeta(int depth, int player, int alpha, int beta);
		
		/**
			Finds the next building to invest in.
			@param depth - the depth to which the game-tree will be searched.
			@return the result of the minimax as a UnitType.
		*/
		BWAPI::UnitType minimaxNextBuilding(int depth);
		/**
			@param tree - the current tech tree
			@return list of nodes that can be obtained from current tech tree
		*/
		std::vector<BWAPI::UnitType> generateMoves(TechTree tree);
		
		// @TODO: ideally the advantage of one player over the other shouldn't be parallel, so that
		// the game state evaluation could be the advantage of self - advantage of enemy
		/**
			Estimates a score for the current state of the game.
		*/
		int evaluateGameState();
		/**
			Estimates the advantage one tech tree has over the other.
		*/
		int evaluateTree(TechTree, TechTree);
		/**
			Updates a tech tree by setting corresponding unit nodes as obtained for a given player.
		*/
		void updateTechTree(tech_count_map_t, int player);
		/**
			Based on race, sets the player's base tech tree to one of the three race trees.
		*/
		void initTree(int player);
		/**
			Sets the target tech to the result of a minimax call.
		*/
		void updateTargetTech();
		/**
			Returns true if the parent node of the given unit is obtained.
		*/
		bool prereqObtained(BWAPI::UnitType);
		BWAPI::UnitType getParentType(BWAPI::UnitType);
		BWAPI::UnitType getTargetTech();
		TechTree getGameTree(int player);
	}
}
