#pragma once

#include "../Definitions.h"

void checkSpriteLoading(Sprite* sprite, const char* filename);


template <class T, size_t N>
constexpr size_t CountOf(const T(&)[N])
{
    return N;
}