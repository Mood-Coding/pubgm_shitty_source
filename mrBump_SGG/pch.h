// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "d3d9.lib") 
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "ntdll.lib")

#pragma warning( push )
#pragma warning( disable : 26495)
#include <d3d9.h>
#include <d3dx9.h>
#pragma warning( pop ) 

#include <unordered_map>
#include <sstream>
#include <array>
#include <WinUser.h>
#include <ctime>
#include <thread>
#include <time.h>
#include <codecvt>
#include <ntstatus.h>
#include <Windows.h>
#include <iostream>
#include <dwmapi.h>
#include <random>
#include <string>
#include <cstdio>
#include <TlHelp32.h>
#include <tchar.h>
#include <algorithm>
#include <iterator>
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
#include <unordered_set>
#include <codecvt>


#endif //PCH_H

