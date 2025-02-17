#ifndef UTILITY_H
#define UTILITY_H

#include "global.h"
std::string typeToString(PlayerType&);
std::string typeToString(ButtonType&);
std::string typeToString(ObstacleType&);
std::string typeToString(ItemType);
ItemType StringTotype(std::string);
double getDis(double x1, double y1, double x2, double y2);

#endif