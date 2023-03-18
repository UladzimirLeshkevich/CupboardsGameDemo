#include "Board.h"
#include "WinMap.h"
#include "../ConfigParser/ConfigParser.h"
#include "../Common/Common.h"

#include <iostream>

const char* configFilePath = "Resources/Configs/Config1.txt";

const float CellTouchAreaWidth = 50.0f;
const float CellTouchAreaHeight = 50.0f;
const float CupMoveSpeed = 500.0f;

cBoard::cBoard()
{
	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();

	if (!Layer::init())
	{
		return;
	}

	ñConfigParser parser;
	m_config = parser.parse(configFilePath);

	assert(CountOf(CupsSprites) >= m_config.numberOfCups, "Number of cups more then number of sprites for them");

	m_winLabel = Label::createWithTTF("YOU WIN !", "fonts/Marker Felt.ttf", 34);
	if (m_winLabel == nullptr)
	{
		std::cerr << "Can't load font " << "fonts/Marker Felt.ttf" << std::endl;
	}
	else
	{
		m_winLabel->setPosition(Vec2(300.0f, 550.0f));
		m_winLabel->setVisible(false);

		this->addChild(m_winLabel, 1);
	}

	createCells();
	createCups();

	createWinMap();

	createGraphOfConnections();

	drawBoard();

	auto mouseListener = cocos2d::EventListenerMouse::create();
	mouseListener->onMouseUp = CC_CALLBACK_1(cBoard::onMouseUp, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

void cBoard::createCells()
{
	m_cells.reserve(m_config.numberOfCells);

	for (auto i = 0u; i < m_config.numberOfCells; ++i)
	{
		sCell cell;

		cell.number = i + 1u;
		cell.cupNumber = 0u;

		cell.position = m_config.cellsPositions[i];

		cell.shine = Sprite::create("shine.png");
		checkSpriteLoading(cell.shine, "shine.png");

		cell.shine->setPosition(cell.position);
		cell.shine->setVisible(false);

		cell.shine->setScale(7.0f);

		const auto touchRectOriginX = cell.position.x - CellTouchAreaHeight * 0.5f;
		const auto touchRectOriginY = cell.position.y - CellTouchAreaWidth * 0.5f;
		cell.touchRect = Rect({ touchRectOriginX, touchRectOriginY }, { CellTouchAreaWidth, CellTouchAreaHeight });

		this->addChild(cell.shine, 0);

		createCellConnections(cell);

		cell.state = sCell::State::NotSelected;

		m_cells.push_back(cell);
	}
}

void cBoard::drawBoard()
{
	auto draw = cocos2d::DrawNode::create(3.0f);

	// draw cells rectangles
	for (const auto& cell : m_cells)
	{
		draw->drawRect(cell.touchRect.origin, Vec2(cell.touchRect.origin.x + CellTouchAreaWidth, cell.touchRect.origin.y + CellTouchAreaHeight), Color4F(1.0, 1.0, 1.0, 1.0));
	}

	// draw cells connections
	for (const auto& con : m_config.cellsConnections)
	{
		const auto& c1 = m_cells[con.cell - 1u];
		const auto& c2 = m_cells[con.connectedCell - 1u];

		Vec2 p1, p2, p3, p4;

		if (c1.position.x == c2.position.x)
		{
			if (c1.position.y > c2.position.y)
			{
				p1.y = c1.position.y - CellTouchAreaHeight * 0.5f;
				p2.y = c2.position.y + CellTouchAreaHeight * 0.5f;
			}
			else
			{
				p1.y = c1.position.y + CellTouchAreaHeight * 0.5f;
				p2.y = c2.position.y - CellTouchAreaHeight * 0.5f;
			}

			p3.y = p1.y;
			p4.y = p2.y;

			p1.x = c1.position.x + 10.0f;
			p2.x = c2.position.x + 10.0f;

			p3.x = c1.position.x - 10.0f;
			p4.x = c2.position.x - 10.0f;
		}
		else
		{
			if (c1.position.x > c2.position.x)
			{
				p1.x = c1.position.x - CellTouchAreaHeight * 0.5f;
				p2.x = c2.position.x + CellTouchAreaHeight * 0.5f;
			}
			else
			{
				p1.x = c1.position.x + CellTouchAreaHeight * 0.5f;
				p2.x = c2.position.x - CellTouchAreaHeight * 0.5f;
			}

			p3.x = p1.x;
			p4.x = p2.x;

			p1.y = c1.position.y + 10.0f;
			p2.y = c2.position.y + 10.0f;

			p3.y = c1.position.y - 10.0f;
			p4.y = c2.position.y - 10.0f;
		}

		draw->drawLine(p1, p2, cocos2d::Color4F(1.0, 1.0, 1.0, 1.0));
		draw->drawLine(p3, p4, cocos2d::Color4F(1.0, 1.0, 1.0, 1.0));
	}

	this->addChild(draw, -1);
}

void cBoard::createCellConnections(sCell& cell)
{
	for (const auto& c : m_config.cellsConnections)
	{
		if (c.cell == cell.number)
		{
			cell.connectedCells.push_back(c.connectedCell);
		}

		if (c.connectedCell == cell.number)
		{
			cell.connectedCells.push_back(c.cell);
		}
	}
}

void cBoard::createCups()
{
	m_cups.reserve(m_config.numberOfCups);

	for (auto i = 0u; i < m_config.numberOfCups; ++i)
	{
		sCup cup;

		cup.number = i + 1;
		cup.cellNumber = m_config.startCupsPositions[i];
		cup.position = m_cells[cup.cellNumber - 1u].position;

		m_cells[cup.cellNumber - 1u].cupNumber = cup.number;

		cup.state = sCup::State::Released;

		cup.button = Button::create(CupsSprites[i], CupsSprites[i]);

		cup.button->addTouchEventListener([index = i, button = cup.button, this](Ref* sender, Widget::TouchEventType type) {
				cupButtonCallback(index, button, type);
			});

		cup.button->setPosition(cup.position);

		this->addChild(cup.button, 0);

		m_cups.push_back(cup);
	}
}

void cBoard::cupButtonCallback(uint32_t cupIndex, Button* button, Widget::TouchEventType type)
{
	switch (type)
	{
		case cocos2d::ui::Widget::TouchEventType::BEGAN:
			break;
		case cocos2d::ui::Widget::TouchEventType::ENDED:
			onCupPressEnded(cupIndex, button);
			break;
		default:
			break;
	}
}

void cBoard::onCupPressEnded(uint32_t cupIndex, Button* button)
{
	if (cBoard::State::Locked == m_state)
	{
		return;
	}

	auto& cup = m_cups[cupIndex];

	if (sCup::State::Released == cup.state)
	{
		resetAllSelections();

		cup.state = sCup::State::Pressed;

		auto& cell = m_cells[cup.cellNumber - 1u];
		cell.shine->setVisible(true);

		selectFreeConnections(cell.connectedCells);
	}
	else if (sCup::State::Pressed == cup.state)
	{
		cup.state = sCup::State::Released;
	}
	
}

void cBoard::selectFreeConnections(Uint32Array connections)
{
	for (auto number : connections)
	{
		auto& c = m_cells[number - 1u];

		if (c.state == sCell::State::Selected || c.cupNumber != 0u )
		{
			continue;
		}

		if (c.cupNumber == 0u)
		{
			c.shine->setVisible(true);
			c.state = sCell::State::Selected;
		}

		selectFreeConnections(c.connectedCells);
	}
}

void cBoard::onMouseUp(Event* event)
{
	if (cBoard::State::Locked == m_state)
	{
		return;
	}

	EventMouse* e = static_cast<EventMouse*>(event);

	auto buttonType = e->getMouseButton();

	if (cocos2d::EventMouse::MouseButton::BUTTON_LEFT == buttonType)
	{
		auto pos = e->getLocationInView();

		if (touchBoard(pos))
		{
			resetAllSelections();
		}
	}
}

bool cBoard::touchBoard(const Vec2& pos)
{
	for (auto& cell : m_cells)
	{
		if (cell.touchRect.containsPoint(pos))
		{
			if (sCell::State::Selected == cell.state && cell.cupNumber == 0u)
			{
				moveCupToCell(cell);
				resetAllSelections();
			}

			return false;
		}
	}

	return true;
}

void cBoard::resetAllSelections()
{
	for (auto& cup : m_cups)
	{
		cup.state = sCup::State::Released;
	}

	for (auto& cell : m_cells)
	{
		cell.shine->setVisible(false);
		cell.state = sCell::State::NotSelected;
	}
}

void cBoard::moveCupToCell(sCell& cell)
{
	auto& cup = getPressedCup();

	if (cup.button == nullptr)
	{
		return;
	}

	m_state = cBoard::State::Locked;

	auto& prevCupsCell = m_cells[cup.cellNumber - 1u];
	auto pathPoints = getPathPoints(prevCupsCell.number - 1u, cell.number - 1u);

	cocos2d::Vector<cocos2d::FiniteTimeAction*> arrayOfActions;

	Vec2 prevPoint = pathPoints[0]; 
	for (const auto& p : pathPoints)
	{
		// move cup with constant speed
		const auto pathLegth = prevPoint.distance(p);
		const auto time = pathLegth / CupMoveSpeed;

		prevPoint = p;

		auto moveAction = cocos2d::MoveTo::create(time, cocos2d::Point(p.x, p.y));
		arrayOfActions.pushBack(moveAction);
	}

	auto finalAction = cocos2d::CallFunc::create([this, &cup, &cell]() {
		auto& prevCupsCell = m_cells[cup.cellNumber - 1u];
		prevCupsCell.cupNumber = 0u;

		cup.cellNumber = cell.number;
		cell.cupNumber = cup.number;

		updateGraphOfConnections();

		checkWin();

		m_state = cBoard::State::Unlocked; });

	arrayOfActions.pushBack(finalAction);

	auto sequence = cocos2d::Sequence::create(arrayOfActions);

	cup.button->runAction(sequence);
}

sCup& cBoard::getPressedCup()
{
	for (auto& cup : m_cups)
	{
		if (sCup::State::Pressed == cup.state)
		{
			return cup;
		}
	}

	static auto emtyCup = sCup();
	return emtyCup;
}

void cBoard::checkWin()
{
	assert(m_config.winCupsPositions.size() == m_cups.size());

	for (auto i = 0u; i < m_config.winCupsPositions.size(); ++i)
	{
		const auto cupWinPosition = m_config.winCupsPositions[i];
		const auto& cup = m_cups[i];

		if (cupWinPosition != cup.cellNumber)
		{
			return;
		}
	}

	// if we are here we win
	m_winLabel->setVisible(true);
}

void cBoard::createGraphOfConnections()
{
	assert(static_cast<uint32_t>(m_cells.size()) == m_config.numberOfCells);

	Graph graph(m_config.numberOfCells);

	m_connectionsGraph.reserve(m_config.numberOfCells);

	for (size_t i = 0; i < m_cells.size(); ++i)
	{
		const auto& c = m_cells[i];

		for (auto cellNumber : c.connectedCells)
		{
			const auto& c = m_cells[cellNumber - 1];
			auto cellWeight = 1u;
			

			if (c.cupNumber != 0u)
			{
				cellWeight = 5u;
			}

			graph[i].push_back({ cellNumber - 1u, cellWeight });
		}
	}

	m_connectionsGraph = graph;
}

void cBoard::updateGraphOfConnections() 
{
	// quick ugly decision
	// TODO make update only for two changed cells instead all cells

	m_connectionsGraph.clear();
	assert(static_cast<uint32_t>(m_cells.size()) == m_config.numberOfCells);

	Graph graph(m_config.numberOfCells);

	m_connectionsGraph.reserve(m_config.numberOfCells);

	for (size_t i = 0; i < m_cells.size(); ++i)
	{
		const auto& c = m_cells[i];

		for (auto cellNumber : c.connectedCells)
		{
			const auto& c = m_cells[cellNumber - 1];
			auto cellWeight = 1u;

			if (c.cupNumber != 0u)
			{
				cellWeight = 5u;
			}

			graph[i].push_back({ cellNumber - 1u, cellWeight });
		}
	}

	m_connectionsGraph = graph;
}

Uint32Array cBoard::getPathIndices(uint32_t s, uint32_t t) const
{
	// Dijkstra algorithm
	// using this implementation https://e-maxx.ru/algo/dijkstra

	const uint32_t inf = 1000000000u;
	const auto n = m_config.numberOfCells;

	Uint32Array d(n, inf), p(n);
	d[s] = 0;
	std::vector<char> u(n);

	for (uint32_t i = 0; i < n; ++i)
	{
		uint32_t v = -1;
		for (uint32_t j = 0; j < n; ++j)
		{
			if (!u[j] && (v == -1 || d[j] < d[v]))
			{
				v = j;
			}
		}

		if (d[v] == inf)
		{
			break;
		}

		u[v] = true;

		for (size_t j = 0; j < m_connectionsGraph[v].size(); ++j) 
		{
			uint32_t to = m_connectionsGraph[v][j].first;
			uint32_t len = m_connectionsGraph[v][j].second;

			if (d[v] + len < d[to]) 
			{
				d[to] = d[v] + len;
				p[to] = v;
			}
		}
	}

	Uint32Array path;
	for (uint32_t v = t; v != s; v = p[v])
	{
		path.push_back(v);
	}

	path.push_back(s);
	reverse(path.begin(), path.end());

	return path;
}

PointsArray cBoard::getPathPoints(uint32_t from, uint32_t to) const
{
	auto path = getPathIndices(from, to); 

	PointsArray result;

	for (const auto index : path)
	{
		result.push_back(m_cells[index].position);
	}

	return result;
}

void cBoard::createWinMap()
{
	auto winMap = new cWinMap(m_cells, m_config);
	winMap->setScale(0.25f);
	winMap->setPosition(230.0f, 230.0f);
	this->addChild(winMap);
}