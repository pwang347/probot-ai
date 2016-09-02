/**
	BuildMinimax.cpp
	Implementation of minimax (with and without alpha-beta pruning) for choosing next tech to rush.

	@author Paul Wang
	@version 9/8/16
*/

#include "BuildMinimax.h"

using namespace ProBot;

tree_score_pair_t BuildMinimax::minimaxAlphaBeta(int depth, int player, int alpha, int beta) {
	// generate a list of units that can be researched from current player tree
	std::vector<BWAPI::UnitType> nextMoves = generateMoves(_gameTreeState[player]);

	// SELF is maximizing; ENEMY is minimizing
	int score;
	TechTree bestTree = NULL_TREE;

	if (nextMoves.size() <= 0 || depth <= 0) {	// nothing left to research or maximum depth reached
		score = evaluateGameState();
		return std::make_pair(bestTree, score);
	}
	else {
		for (BWAPI::UnitType move : nextMoves) { // otherwise evaluate each move
			// apply the move to the current player's gamestate
			setBuildingNodeObtained(move, _gameTreeState[player]);

			if (player == SELF) {
				// evaluate ENEMY's turn
				score = minimaxAlphaBeta(depth - 1, ENEMY, alpha, beta).second;
				if (score > alpha) {
					alpha = score;
					bestTree = _gameTreeState[player];
				}
			}
			else {
				// evaluate SELF's turn
				score = minimaxAlphaBeta(depth - 1, SELF, alpha, beta).second;
				if (score < beta) {
					beta = score;
					bestTree = _gameTreeState[player];
				}
			}
			// undo the move for the current player's gamestate
			setBuildingNodeObtained(move, _gameTreeState[player], false);

			// cut-off
			if (alpha >= beta) break;
		}
		// return best tree and its corresponding score
		return std::make_pair(bestTree, (player == SELF) ? alpha : beta);
	}
}

 tree_score_pair_t BuildMinimax::minimax(int depth, int player) {
	
	 // generate a list of units that can be researched from current player tree
	std::vector<BWAPI::UnitType> nextMoves = generateMoves(_gameTreeState[player]);
	
	// SELF is maximizing; ENEMY is minimizing
	int bestScore = (player == SELF) ? INT_MIN : INT_MAX;
	int currentScore;
	TechTree bestTree = NULL_TREE;

	if (nextMoves.size() <= 0 || depth <= 0) {	// nothing left to research or maximum depth reached
		bestScore = evaluateGameState();
	}
	else {
		for (BWAPI::UnitType move : nextMoves) { // otherwise evaluate each move
			// apply the move to the current player's gamestate
			setBuildingNodeObtained(move, _gameTreeState[player]);

			if (player == SELF) {
				// evaluate ENEMY's turn
				currentScore = minimax(depth - 1, ENEMY).second;

				if (currentScore > bestScore) { // maximizing step
					bestScore = currentScore;
					bestTree = _gameTreeState[player];
				}
			}
			else {
				// evaluate SELF's turn
				currentScore = minimax(depth - 1, SELF).second;
				if (currentScore < bestScore) { // minimizing step
					bestScore = currentScore;
					bestTree = _gameTreeState[player];
				}
			}

			// undo the move for the current player's gamestate
			setBuildingNodeObtained(move, _gameTreeState[player], false);
		}
	}
	// return best tree and its corresponding score
	return std::make_pair(bestTree, bestScore);
}

 BWAPI::UnitType BuildMinimax::minimaxNextBuilding(int depth){
	 TechTree result = minimaxAlphaBeta(depth, SELF, INT_MIN, INT_MAX).first; // get the minimax tree
	 //TechTree result = minimax(depth, SELF).first;
	 return getFirstDifference(result, _gameTreeState[SELF]); // get the unit that was set to obtained
 }

std::vector<BWAPI::UnitType> BuildMinimax::generateMoves(TechTree tree) {
	std::vector<BWAPI::UnitType> nextMoves;

	for (TechTree & obtainableNode : findNextObtainableNodes(tree)){
			nextMoves.push_back(obtainableNode._unitType);
	}

	return nextMoves;
}

int BuildMinimax::evaluateGameState(){
	int eval = 0;

	eval += evaluateTree(_gameTreeState[SELF], _gameTreeState[ENEMY]);
	//eval -= evaluateTree(_gameTreeState[ENEMY], _gameTreeState[SELF]);

	return eval;
}

int BuildMinimax::evaluateTree(TechTree first, TechTree second){
	int eval = 0;

	for (TechTree & firstNode : findNonBuildingLeafNodes(first)){
		// give a base score for each unit, so that it's more advantageous to have access to
		// a greater diversity of units
		eval += 1000;
		for (TechTree & secondNode : findNonBuildingLeafNodes(second)){
			int currentEval = 0;
			// consider static unit scores and then check if one unit has an advantage over the other
			currentEval += CombatUtil::getStaticUnitScore(firstNode._unitType);
			currentEval -= CombatUtil::getStaticUnitScore(secondNode._unitType);
			currentEval += CombatUtil::evaluateStaticAdvantage(firstNode._unitType, secondNode._unitType);
			eval += currentEval;
		}
	}
	return eval;
}

void BuildMinimax::initTree(int player){
	// get base tree based on the race of player
	BWAPI::Race race = (player == ENEMY) ?
		BWAPI::Broodwar->enemy()->getRace() : BWAPI::Broodwar->self()->getRace();
	_gameTreeState[player] = getBaseTree(race);
}

void BuildMinimax::updateTechTree(tech_count_map_t techUnits, int player){
	// if the tree hasn't been initialized yet, do it
	if (_gameTreeState[player] == NULL_TREE){
		initTree(player);
	}
	
	for (auto & techCountPair : techUnits){
		// for each tech unit, set its prereqs as obtained (in case some of them aren't visible to us)
		setPrereqsObtained(BWAPI::UnitType(techCountPair.first), _gameTreeState[player]);
		// set the units that can be produced from the building as obtained / obtainable
		setBuildingNodeObtained(BWAPI::UnitType(techCountPair.first), _gameTreeState[player]);
	}

	updateTargetTech(); // whenever either tree is updated, run analysis again
}

void BuildMinimax::updateTargetTech(){
	_targetTech = minimaxNextBuilding(DEFAULT_DEPTH);
}

bool BuildMinimax::prereqObtained(BWAPI::UnitType unitType){
	return getImmediateParent(unitType, _gameTreeState[SELF])->_obtained;
}

BWAPI::UnitType BuildMinimax::getParentType(BWAPI::UnitType unitType){
	return getImmediateParent(unitType, _gameTreeState[SELF])->_unitType;
}

BWAPI::UnitType BuildMinimax::getTargetTech(){
	return _targetTech;
}

TechTree BuildMinimax::getGameTree(int player){
	return _gameTreeState[player];
}