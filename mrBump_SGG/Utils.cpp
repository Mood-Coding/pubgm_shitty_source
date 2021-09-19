#include "pch.h"
#include "Utils.h"

std::string Utils::RandomString(size_t length) {
    //generate new seed for random function
    srand((unsigned)time(NULL));

    auto randchar = []() -> char
    {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

std::wstring Utils::StrToWStr(const std::string &s) {
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

bool Utils::isInsideRect(POINT* point, unsigned short left, unsigned short top, unsigned short right, unsigned short bottom)
{
    if ((point->x >= left && point->x <= right) && (point->y >= top && point->y <= bottom))
    {
        return true;
    }

    return false;
}

void Utils::getMousePosInGameScreen(POINT* pMousePos, long gameScreenLeft, long gameScreenTop)
{
    GetCursorPos(pMousePos);

    pMousePos->x -= gameScreenLeft;
    pMousePos->y -= gameScreenTop;
}

std::string Utils::PCharToString(char* pChar)
{
    return std::string(pChar);
}

bool Utils::is_digits(const std::string& str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}

float Utils::DistBetween2Vector2D(SDK::FVector2D v1, SDK::FVector2D v2)
{
    //float distance = sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point1.y, 2));
    return sqrt(pow(v2.X - v1.X, 2) + pow(v2.Y - v1.Y, 2));
}

// convert UTF-8 string to wstring
std::wstring Utils::string_utf8_to_wstring(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.from_bytes(str);
}

std::string Utils::wstring_to_string_utf8(const std::wstring& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.to_bytes(str);
}