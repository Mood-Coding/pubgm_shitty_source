// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "ntdll.lib")

#include <ctime>
#include <thread>
#include <time.h>
#include <ntstatus.h>
#include <Windows.h>
#include <iostream>
#include <dwmapi.h>
#include <random>
#include <string>
#include <cstdio>
#include <TlHelp32.h>
#include <tchar.h>
#include <vector>
#include <list>
#include <winternl.h>
#include <stdio.h>
#include <iomanip>
#include <fstream>
#include <map>
#include <functional>
#include <mutex>
#include <future>
#include <numeric>
#include <math.h>


#endif //PCH_H
