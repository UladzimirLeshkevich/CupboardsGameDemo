#pragma once

#include "../Definitions.h"

struct sCell;
struct sCup;

using  Cells = std::vector<sCell>;
using  Cups = std::vector<sCup>;
using  Graph = std::vector <std::vector<std::pair<uint32_t, uint32_t>>>;

struct sCup
{
	enum class State
	{
		Released,
		Pressed,
	};

	uint32_t number = 0u;
	uint32_t cellNumber = 0u;
	Vec2 position = {0.0f, 0.0f};

	Button* button = nullptr;

	State state = State::Released;
};

struct sCell
{
	enum class State
	{
		Selected,
		NotSelected,
	};

	uint32_t number = 0u;
	uint32_t cupNumber = 0u;

	Vec2 position = { 0.0f, 0.0f };

	Uint32Array connectedCells;

	Sprite* shine = nullptr;

	Rect touchRect;

	State state = State::NotSelected;
};

class cBoard : public cocos2d::Layer
{
public:
	cBoard();

private:
	void drawBoard();

	void createCells();

	void createCellConnections(sCell& cell);

	void createCups();

	void cupButtonCallback(uint32_t cupIndex, Button* button, Widget::TouchEventType type);
	void onCupPressEnded(uint32_t cupIndex, Button* button);

	void onMouseUp(Event* event);

	void resetAllSelections();

	bool touchBoard(const Vec2& pos);

	void moveCupToCell(sCell& cell);
	sCup& getPressedCup();

	void selectFreeConnections(Uint32Array connections);

	void checkWin();

	void createGraphOfConnections();

	Uint32Array getPathIndices(uint32_t from, uint32_t to) const;
	PointsArray getPathPoints(uint32_t from, uint32_t to) const;

	void updateGraphOfConnections();

	void createWinMap();

private:
	enum State
	{
		Locked,
		Unlocked,
	};

	Cells m_cells;
	Cups m_cups;

	State m_state = State::Unlocked;

	sGameProperties m_config;
	Graph m_connectionsGraph;

	Label* m_winLabel;
};
