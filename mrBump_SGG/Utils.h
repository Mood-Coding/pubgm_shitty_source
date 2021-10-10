#pragma once
#include "pch.h"
#include "ESP.h"
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

    float DistBetween2Vector2D(const SDK::FVector2D& v1, const SDK::FVector2D& v2);
    float DistBetween2Vector3D(const SDK::FVector& v1, const SDK::FVector& v2);

    void DumpActorName();

    bool is_file_exist(const char* file_path);

    template<typename T>
    std::string DecToHex(T dec_num)
    {
        std::stringstream ss;

        ss << std::hex << dec_num;

        return ss.str();
    }

    template<typename T>
    T HexToDec(std::string& hex_num)
    {
        T dec_num;
        std::stringstream ss;

        ss << hex_num;
        ss >> std::hex >> dec_num;

        return dec_num;
    }

    template<typename T>
    T HexToDec(const char* hex_num)
    {
        T dec_num;
        std::stringstream ss;

        ss << hex_num;
        ss >> std::hex >> dec_num;

        return dec_num;
    }
}

#endif // ! _UTILS_H_

