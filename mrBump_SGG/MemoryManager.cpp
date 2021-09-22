#include "pch.h"
#include "MemoryManager.h"

MemoryManager* g_pMM = new MemoryManager();

bool MemoryManager::Init(HANDLE hTargetProcess, DWORD dwTargetProcessPID)
{
	std::cout << "[MM]\n";

	bool bStatus = connectToDriver_fix("\\Device\\KProcessHacker2");

	// If not connected yet: exit. 
	if (m_hDriver && bStatus)
	{
		std::cout << "Connected to driver 0x" << std::hex << m_hDriver << ": SUCCESS" << std::endl;
	}
	else
	{
		std::cout << "Can't connect to driver. Please be sure that the driver is loaded!" << std::endl;
		return 0;
	}
	
	processHandle = hTargetProcess;
	processId = dwTargetProcessPID;

	return 1;
}

bool MemoryManager::connectToDriver_fix(std::string m_strDeviceName) {
	std::wstring strObjectName;
	strObjectName.assign(m_strDeviceName.begin(), m_strDeviceName.end());
	UNICODE_STRING objectName;

	HANDLE hNtDriver;
	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK isb;

	RtlInitUnicodeString(&objectName, strObjectName.c_str());
	InitializeObjectAttributes(&objectAttributes, &objectName, FILE_NON_DIRECTORY_FILE, NULL, NULL);

	NTSTATUS status = NtOpenFile(&hNtDriver, FILE_GENERIC_READ | FILE_GENERIC_WRITE, &objectAttributes, &isb, FILE_SHARE_READ, FILE_NON_DIRECTORY_FILE);
	std::cout << "Connect driver status: 0x" << std::hex << status << std::endl;
	if (status == STATUS_SUCCESS)
	{
		m_hDriver = hNtDriver;
		return true;
	}
	else
	{
		return false;
	}	
}


void MemoryManager::readMemory(PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize) {
	struct {
		HANDLE ProcessHandle;
		PVOID BaseAddress;
		PVOID Buffer;
		SIZE_T BufferSize;
		PSIZE_T NumberOfBytesRead;
	} input = { processHandle, BaseAddress, Buffer, BufferSize, NumberOfBytes };

	IO_STATUS_BLOCK ioStatusBlock;

	NtDeviceIoControlFile(m_hDriver, nullptr, nullptr, nullptr, &ioStatusBlock, MM_READVIRTUALMEMORY, &input, sizeof(input), nullptr, 0);
}


bool MemoryManager::search(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, std::vector<DWORD_PTR>& vRet)
{
	MEMORY_BASIC_INFORMATION	mbi;
	std::vector<MEMORY_REGION> m_vMemoryRegion;
	mbi.RegionSize = 0x400;
	DWORD dwAddress = dwStartAddr;

	MEMORY_REGION memSectorList[1000];

	int memSectorIndex = 0;
	while (VirtualQueryEx(processHandle, (LPCVOID)dwAddress, &mbi, sizeof(mbi)) && (dwAddress < dwEndAddr) && ((dwAddress + mbi.RegionSize) > dwAddress)) {
		if (
			(mbi.State == MEM_COMMIT) &&
			((mbi.Protect & PAGE_GUARD) == 0) &&
			(mbi.Protect != PAGE_NOACCESS) &&
			((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE)
			) {
			MEMORY_REGION mData = { 0 };
			mData.dwBaseAddr = (DWORD_PTR)mbi.BaseAddress;
			mData.dwMemorySize = mbi.RegionSize;
			m_vMemoryRegion.push_back(mData);
			memSectorList[memSectorIndex] = mData;
			memSectorIndex++;
		}
		dwAddress = (DWORD)mbi.BaseAddress + mbi.RegionSize;
	}

	int memSectorCount = memSectorIndex;
	memSectorIndex = 0;
	DWORD_PTR curAddr = dwStartAddr;
	while (curAddr < dwEndAddr) {
		VirtualQueryEx(processHandle, (LPCVOID)curAddr, &mbi, sizeof(mbi));
		long regionSizeOrg = mbi.RegionSize;
		long regionSize = mbi.RegionSize;
		if (regionSize > 10) {
			BYTE* pCurrMemoryData = new BYTE[regionSize];
			ZeroMemory(pCurrMemoryData, regionSize);
			readMemory((PVOID)curAddr, (PVOID*)pCurrMemoryData, regionSize);
			DWORD_PTR dwOffset = 0;
			int iOffset = find(pCurrMemoryData, regionSize, bSearchData, nSearchSize);
			while (iOffset != -1) {
				dwOffset += iOffset;
				vRet.push_back(dwOffset + curAddr);
				dwOffset += nSearchSize;
				iOffset = find(pCurrMemoryData + dwOffset, regionSize - dwOffset - nSearchSize, bSearchData, nSearchSize);
			}
			delete[] pCurrMemoryData;
		}
		memSectorIndex++;
		curAddr = curAddr + (DWORD_PTR)regionSizeOrg;
		continue;
	}
	return TRUE;
}


int MemoryManager::find(BYTE* buffer, int dwBufferSize, BYTE* bstr, DWORD dwStrLen) {
	if (dwBufferSize < 0) {
		return -1;
	}
	DWORD  i, j;
	for (i = 0; i < dwBufferSize; i++) {
		for (j = 0; j < dwStrLen; j++) {
			if (buffer[i + j] != bstr[j] && bstr[j] != '?')
				break;
		}
		if (j == dwStrLen)
			return i;
	}
	return -1;
}


FTransform MemoryManager::ftRead(DWORD base) {
	readMemory((PVOID) base, &_ft, sizeof(_ft));
	return _ft;
}