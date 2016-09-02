/**
	Debug.h
	Utility functions used to output text, and draw information on the screen and map

	@author Paul Wang
	@version 9/8/16
*/


#pragma once

#include <BWAPI.h>
#include "MovementUtil.h"

namespace ProBot{
	namespace Debug{
		const int SCREEN_WIDTH = 0;
		const int SCREEN_HEIGHT = 0;
		const int TEXT_HEIGHT = 15;
		const int BOX_SIZE_SMALL = 25;
		const int BOX_SIZE_LARGE = 50;

		const BWAPI::Color RED = BWAPI::Color::Color(255, 0, 0);
		const BWAPI::Color GREEN = BWAPI::Color::Color(0, 255, 0);
		const BWAPI::Color BLUE = BWAPI::Color::Color(0, 0, 255);

		void drawBoxWithLabel(std::string, BWAPI::Position pos, BWAPI::Color = Debug::BLUE);
		void drawBoxNoLabel(BWAPI::Position pos, BWAPI::Color color = Debug::BLUE);
		void drawLine(BWAPI::Position, BWAPI::Position, BWAPI::Color color = Debug::BLUE);
		void log(std::string, int pos);
		void log(std::string, int posy, int posx);
		void message(std::string);
		std::string pos_to_string(BWAPI::Position);
		void trackUnit(BWAPI::Unit);
		void drawUnitSightRange(BWAPI::Unit);
	}
}