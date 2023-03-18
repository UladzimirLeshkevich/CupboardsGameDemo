#pragma once

#include "Definitions.h"
#include "Board/Board.h"
#include <memory>

class TestGame : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();

    CREATE_FUNC(TestGame);

private:

    std::unique_ptr<cBoard> m_board;
};
