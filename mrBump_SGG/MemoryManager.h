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


public:
	MemoryManager() { };

	HANDLE m_hDriver = nullptr;

	HANDLE processHandle = NULL;
	int processId = 0;

	bool m_bUsingAnotherDriverService = false;
	PSIZE_T NumberOfBytes = nullptr;

	Vector3f _v3;
	Vector2f _v2;
	FTransform _ft;

	bool Init(HANDLE hTargetProcess, DWORD dwTargetProcessPID);

	bool ConnectToDriver_fix(std::string m_strDeviceName);

	bool LoadDriver();
	bool StartDriver();

	bool StopDriver();
	bool UnloadDriver();

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