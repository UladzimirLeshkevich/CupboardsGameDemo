#include "WinMap.h"

cWinMap::cWinMap(const Cells& cells, const sGameProperties& config) 
{
	if (!Layer::init())
	{
		return;
	}

	draw(cells, config);
}

void cWinMap::draw(const Cells& cells, const sGameProperties& config)
{
	auto draw = cocos2d::DrawNode::create();

	// draw map board
	//auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	//draw->drawRect(Vec2(0.0f, 0.0f), Vec2(visibleSize.height, visibleSize.width), Color4F(1.0, 1.0, 1.0, 1.0));

	// draw cells rectangles
	for (const auto& cell : cells)
	{
		draw->drawRect(cell.touchRect.origin, Vec2(cell.touchRect.origin.x + 50.0f, cell.touchRect.origin.y + 50.0f), Color4F(1.0, 1.0, 1.0, 1.0));
	}

	// draw cells connections
	for (const auto& con : config.cellsConnections)
	{
		const auto& c1 = cells[con.cell - 1u];
		const auto& c2 = cells[con.connectedCell - 1u];

		draw->drawLine(c1.position, c2.position, cocos2d::Color4F(1.0, 1.0, 1.0, 1.0));
	}

	// draw cups in win positions
	for (auto i = 0u; i < config.numberOfCups; ++i)
	{
		auto sprite = Sprite::create(CupsSprites[i]);
		auto cellIndex = config.winCupsPositions[i] - 1u;

		sprite->setPosition(cells[cellIndex].position);

		this->addChild(sprite, 1);
	}

	this->addChild(draw, 0);
}