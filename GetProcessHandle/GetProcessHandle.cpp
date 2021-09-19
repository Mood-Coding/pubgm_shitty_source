// GetProcessHandle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "ProcessManager.h"

int main()
{
    std::cout << "Hello World!\n";

    if (!g_pPM->Init(L"AndroidProcess.exe"))
    {
        system("pause");
        return 0;
    }

    std::cout << "PID: " << std::dec << g_pPM->processPID << " " << std::hex << g_pPM->processPID << "\n";
    std::cout << "Handle: " << std::hex << g_pPM->hProcess << std::endl;

    while (true)
    {
        Sleep(1);
    }
}

