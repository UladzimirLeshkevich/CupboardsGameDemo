#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"

struct sCellConnection;

using cocos2d::Vec2;
using cocos2d::Rect;
using cocos2d::Sprite;
using cocos2d::Label;
using cocos2d::Menu;
using cocos2d::MenuItemImage;

using cocos2d::ui::Button;
using cocos2d::ui::Widget;

using cocos2d::Event;
using cocos2d::EventMouse;

using cocos2d::Color4F;

using  Uint32Array = std::vector<uint32_t>;
using  PointsArray = std::vector<Vec2>;
using  CellsConnections = std::vector<sCellConnection>;

using Strings = std::vector<std::string>;

// if you want more than 6 cups add more sprites for them here
static const char* CupsSprites[] = { "cups/yellow.png",
									 "cups/green.png",
									 "cups/grey.png",
									 "cups/blue.png",
									 "cups/violet.png",
									 "cups/red.png" };

struct sCellConnection
{
	uint32_t cell;
	uint32_t connectedCell;
};

struct sGameProperties
{
	uint32_t numberOfCups = 0u;
	uint32_t numberOfCells = 0u;

	PointsArray cellsPositions;

	Uint32Array startCupsPositions;
	Uint32Array winCupsPositions;

	CellsConnections cellsConnections;
};

enum class Direction
{
	Up,
	Down,
	Left,
	Right
};