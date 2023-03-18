#pragma once

#include "Common.h"

#include <iostream>

void checkSpriteLoading(Sprite* sprite, const char* filename)
{
	if (sprite == nullptr)
	{
		std::cerr << "Can't load sprite " << filename << std::endl;
		assert(! "Sprite loading failed");
	}
}