#pragma once
#include "pch.h"
#include "SDK_classes.h"
#include "SDK_structs.h"

#ifndef  _UTILS_H_
#define _UTILS_H_

namespace Utils
{
    std::string RandomString(size_t length);

    std::wstring StrToWStr(const std::string& s);
    std::wstring string_utf8_to_wstring(const std::string& str);

    std::string wstring_to_string_utf8(const std::wstring& str);    

    bool isInsideRect(POINT* point, unsigned short left, unsigned short top, unsigned short right, unsigned short bottom);

    void getMousePosInGameScreen(POINT* pMousePos, long gameScreenLeft, long gameScreenTop);

    std::string PCharToString(char* pChar);

    bool is_digits(const std::string& str);

    float DistBetween2Vector2D(SDK::FVector2D v1, SDK::FVector2D v2);
}

#endif // ! _UTILS_H_

