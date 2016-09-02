
#include "ProBotModule.h"
#include <iostream>

using namespace ProBot;
using namespace BWAPI;

void ProBotModule::onStart()
{
	// set default to 2x speed (24 is 1x speed)
	BWAPI::Broodwar->setLocalSpeed(12);
	BWAPI::Broodwar->setFrameSkip(0);

	// enables user input
	Broodwar->enableFlag(BWAPI::Flag::UserInput);

	// groups commands together to reduce bot apm
	Broodwar->setCommandOptimizationLevel(2);

	// if race isn't protoss, then may as well surrender.
	if (Broodwar->self()->getRace() != Races::Protoss)
		Broodwar->leaveGame();
}

void ProBotModule::onEnd(bool isWinner)
{
	// called when the game ends
	//if (isWinner)
	// TODO: Export stats as JSON
}

void ProBotModule::onFrame()
{
	_gameManager.update();
}

void ProBotModule::onSendText(std::string text)
{
	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());
}

void ProBotModule::onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void ProBotModule::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void ProBotModule::onNukeDetect(BWAPI::Position target)
{
	_gameManager.onNukeDetect(target);
}

void ProBotModule::onUnitDiscover(BWAPI::Unit unit)
{
	_gameManager.onUnitDiscover(unit);
}

void ProBotModule::onUnitEvade(BWAPI::Unit unit)
{
	_gameManager.onUnitEvade(unit);
}

void ProBotModule::onUnitShow(BWAPI::Unit unit)
{
	_gameManager.onUnitShow(unit);
}

void ProBotModule::onUnitHide(BWAPI::Unit unit)
{
	_gameManager.onUnitHide(unit);
}

void ProBotModule::onUnitCreate(BWAPI::Unit unit)
{
	_gameManager.onUnitCreate(unit);
}

void ProBotModule::onUnitDestroy(BWAPI::Unit unit)
{
	_gameManager.onUnitDestroy(unit);
}

void ProBotModule::onUnitMorph(BWAPI::Unit unit)
{
	_gameManager.onUnitMorph(unit);
}

void ProBotModule::onUnitRenegade(BWAPI::Unit unit)
{
	_gameManager.onUnitRenegade(unit);
}

void ProBotModule::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void ProBotModule::onUnitComplete(BWAPI::Unit unit)
{
	_gameManager.onUnitComplete(unit);
}
