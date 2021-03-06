#pragma once
#include "pch.h"

#include "datatype.h"
#include "Utils.h"

#define MM_DEVICE_TYPE 0x9999
#define MM_CTL_CODE(x) CTL_CODE(MM_DEVICE_TYPE, 0x800 + x, METHOD_NEITHER, FILE_ANY_ACCESS)
#define MM_READVIRTUALMEMORY MM_CTL_CODE(56)

#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

typedef struct _MEMORY_REGION
{
	DWORD_PTR dwBaseAddr;
	DWORD_PTR dwMemorySize;
}MEMORY_REGION;

class MemoryManager {
private:
	std::wstring m_DriverPath{};

	bool m_bLoadedService{ false };
	bool m_bStartedService{ false };
	bool m_bStoppedService{ false };
	bool m_bUsingAnotherService{ false };

	DWORD PID{ 0 };

	HANDLE hProcess;

	bool SetPrivilege(LPCWSTR lpszPrivilege, BOOL bEnablePrivilege);
	DWORD GetPID(const std::wstring& processName);


public:
	MemoryManager() { };

	HANDLE m_hDriver = nullptr;

	PSIZE_T NumberOfBytes = nullptr;

	FTransform _ft;

	std::wstring emuProcName;

	bool Init();

	bool ConnectToDriver(std::string m_strDeviceName);

	bool LoadDriver();
	bool StartDriver();
	void StopDriver();
	void UnloadDriver();

	bool search(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, /*BOOL bIsCurrProcess, int iSearchMode,*/ std::vector<DWORD_PTR>& vRet);
	int find(BYTE* buffer, int dwBufferSize, BYTE* bstr, DWORD dwStrLen);
	void readMemory(PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize);

	FTransform ftRead(DWORD base);

	template<typename T> T read(DWORD address)
	{
		T buffer{};
		readMemory(reinterpret_cast<PVOID>(address), reinterpret_cast<PVOID>(&buffer), sizeof(T));
		return buffer;
	}
};

extern MemoryManager* g_pMM;
#endif