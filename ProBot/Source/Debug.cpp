/**
	Debug.cpp
	Utility functions used to output text, and draw information on the screen and map

	@author Paul Wang
	@version 9/8/16
*/

#include "Debug.h"

using namespace ProBot;

void Debug::drawBoxWithLabel(std::string text, BWAPI::Position pos, BWAPI::Color color){
	BWAPI::Broodwar->drawBox(BWAPI::CoordinateType::Map, pos.x, pos.y, pos.x+BOX_SIZE_SMALL, pos.y+BOX_SIZE_SMALL, color, false);
	BWAPI::Broodwar->drawTextMap(pos, text.c_str());
}

void Debug::drawBoxNoLabel(BWAPI::Position pos, BWAPI::Color color){
	BWAPI::Broodwar->drawBox(BWAPI::CoordinateType::Map, pos.x - 50, pos.y - 50, pos.x - 50 + BOX_SIZE_LARGE, pos.y - 50 + BOX_SIZE_LARGE, color, false);
}

void Debug::log(std::string text, int pos){
	BWAPI::Broodwar->drawTextScreen(10, TEXT_HEIGHT*pos, text.c_str(), '\x04');
}

void Debug::log(std::string text, int posy, int posx){
	BWAPI::Broodwar->drawTextScreen(posx, TEXT_HEIGHT*posy, text.c_str(), '\x04');
}

void Debug::message(std::string text){
	BWAPI::Broodwar->sendText(text.c_str());
}

std::string Debug::pos_to_string(BWAPI::Position pos){
	return std::to_string(pos.x).append(",").append(std::to_string(pos.y));
}

void Debug::trackUnit(BWAPI::Unit unit){
	BWAPI::Position pos = unit->getTargetPosition();
	if (!pos){
		// didn't find a target position
	}
	else {
		drawLine(unit->getPosition(), pos, Debug::GREEN);
		drawBoxWithLabel(unit->getOrder().getName(), pos, Debug::GREEN);
	}
}

void Debug::drawLine(BWAPI::Position pos1, BWAPI::Position pos2, BWAPI::Color color){
	BWAPI::Broodwar->drawLine(BWAPI::CoordinateType::Map, pos1.x, pos1.y, pos2.x, pos2.y, color);
}

void Debug::drawUnitSightRange(BWAPI::Unit unit){
	BWAPI::Broodwar->drawCircleMap(unit->getPosition(), unit->getType().sightRange(), Debug::BLUE, false);
}