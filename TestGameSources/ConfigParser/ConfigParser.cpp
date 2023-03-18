#include "ConfigParser.h"
#include "../Common/Common.h"

#include <iostream>
#include <fstream>

Strings split(const std::string& s, char delim)
{
    Strings result;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim))
    {
        result.push_back(item);
    }

    return result;
}

Vec2 stringToVec2(const std::string& string)
{
    auto strings = split(string, ',');
    Vec2 result;

    result.x = std::stof(strings[0]);
    result.y = std::stof(strings[1]);

    return result;
}

sCellConnection stringToCellConnection(const std::string& string)
{
    auto strings = split(string, ',');
    sCellConnection result;

    result.cell = std::stoi(strings[0]);
    result.connectedCell = std::stoi(strings[1]);

    return result;
}

Uint32Array stringToUint32Array(const std::string& string)
{
    auto strings = split(string, ',');
    Uint32Array result;

    for (const auto& s : strings)
    {
        result.push_back(std::stoi(s));
    }

    return result;
}

sGameProperties ÒConfigParser::parse(const char* file)
{
    sGameProperties result;

    std::ifstream inputFile(file);
    if (!inputFile)
    {
        assert(!"Config loading failed");
    }

    Strings parsedStrings;
    std::string stringFromFile;

    while (std::getline(inputFile, stringFromFile))
    {
        parsedStrings.push_back(stringFromFile);
    }

    inputFile.close();

    assert(parsedStrings.empty() == false);

    result.numberOfCups = std::stoi(parsedStrings[0]);
    result.numberOfCells = std::stoi(parsedStrings[1]);

    for (auto i = 2u; i < result.numberOfCells + 2u; ++i)
    {
        result.cellsPositions.push_back(stringToVec2(parsedStrings[i]));
    }

    result.startCupsPositions = stringToUint32Array(parsedStrings[result.numberOfCells + 2u]);
    result.winCupsPositions = stringToUint32Array(parsedStrings[result.numberOfCells + 3u]);

    auto numberOfConnections = std::stoi(parsedStrings[result.numberOfCells + 4u]);

    for (auto i = result.numberOfCells + 5u; i < result.numberOfCells + 5u + numberOfConnections; ++i)
    {
        result.cellsConnections.push_back(stringToCellConnection(parsedStrings[i]));
    }

    return result;
}