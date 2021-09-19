// TestMouse.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

void MoveMouse(long x, long y)
{
    INPUT Input = { 0 };
    Input.type = INPUT_MOUSE;

    Input.mi.dx = x;
    Input.mi.dy = y;

    Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

    SendInput(1, &Input, sizeof(INPUT));
}


int main()
{
    long x = 0;
    long y = 0;
    while (true)
    {

        std::cout << "x\n";
        std::cin >> x;
        std::cout << "y\n";
        std::cin >> y;
        
        while (true)
        {
            if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000)
            {
                MoveMouse(x, y);

                while (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) {}

                break;
            }
        }
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
