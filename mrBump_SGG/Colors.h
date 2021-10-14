#pragma once
#include "ImGui/imgui.h"

#ifndef _COLORS_H_
#define _COLORS_H_

#define WHITE(alpha)		 IM_COL32(255, 255, 255, alpha)
#define DARKWHITE(alpha)	 IM_COL32(235, 230, 22, alpha)
#define BLACK(alpha)		 IM_COL32(0, 0, 0, alpha)
#define DARKGRAY(alpha)      IM_COL32(73, 73, 73, alpha)
#define DARKERGRAY(alpha)    IM_COL32(128, 128, 128, alpha)
#define GRAY(alpha)          IM_COL32(128, 128, 128, alpha)

#define GREEN(alpha)         IM_COL32(0, 255, 0, alpha)
#define LIMEGREEN(alpha)	 IM_COL32(33, 100, 33, alpha)
#define LAWNGREEN(alpha)     IM_COL32(124, 252, 0, alpha)

#define AQUA(alpha)			 IM_COL32(85, 255, 25, alpha)
#define BLUE(alpha)          IM_COL32(0, 0, 255, alpha)
#define DEEPSKYBLUE(alpha)   IM_COL32(0, 191, 25, alpha)
#define SKYBLUE(alpha)       IM_COL32(0, 122, 204, alpha)
#define LIGHTCYAN(alpha)	 IM_COL32(33, 100, 10, alpha)
#define CYAN(alpha)          IM_COL32(0, 255, 255, alpha)

#define YELLOW(alpha)        IM_COL32(255, 255, 0, alpha)
#define ORANGE(alpha)        IM_COL32(255, 165, 0, alpha)
#define DARKORANGE(alpha)    IM_COL32(100, 55, 0, alpha)
#define RED(alpha)           IM_COL32(255, 0, 0, alpha)
#define DARKRED(alpha)		 IM_COL32(139, 0, 0, alpha)

#define PURPLE(alpha)        IM_COL32(125, 0, 25, alpha)
#define PINK(alpha)          IM_COL32(255, 192, 203, alpha)

extern std::array<unsigned int, 50> TeamIDColor;

#endif