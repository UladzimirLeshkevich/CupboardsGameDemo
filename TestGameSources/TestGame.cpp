#include "TestGame.h"

USING_NS_CC;

Scene* TestGame::createScene()
{
    return TestGame::create();
}

bool TestGame::init()
{
    if ( !Scene::init() )
    {
        return false;
    }

    m_board = std::make_unique<cBoard>();

    this->addChild(m_board.get(), 0);

    return true;
}