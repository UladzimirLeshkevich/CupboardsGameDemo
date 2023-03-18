#pragma once
#include "Board.h"

class cWinMap : public cocos2d::Layer
{
public:
	cWinMap(const Cells& cells, const sGameProperties& config);

private:
	void draw(const Cells& cells, const sGameProperties& config);
};