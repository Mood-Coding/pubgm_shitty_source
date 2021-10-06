#include "pch.h"
#include "MemoryManager.h"

MemoryManager* g_pMM = new MemoryManager();

bool MemoryManager::Init()
{
	std::cout << "[MM]\n";

	if (LoadDriver())
	{
		if (!StartDriver())
			return false;
	}
	else
	{
		return false;
	}
	
	bool bStatus = ConnectToDriver_fix("\\Device\\KProcessHacker2");
	if (m_hDriver && bStatus)
	{
		std::cout << "Connected to driver 0x" << std::hex << m_hDriver << '\n';
	}
	else
	{
		std::cout << "Can't connect to driver" << std::endl;
		return false;
	}
	
	processHandle = g_pPM->hProcess;
	processId = g_pPM->PID;

	return true;
}

bool MemoryManager::ConnectToDriver_fix(std::string m_strDeviceName)
{
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


bool MemoryManager::search(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr,/* BOOL bIsCurrProcess, int iSearchMode,*/ std::vector<DWORD_PTR>& vRet)
{
	MEMORY_BASIC_INFORMATION	mbi;
	std::vector<MEMORY_REGION> m_vMemoryRegion;
	mbi.RegionSize = 0x400;
	DWORD dwAddress = dwStartAddr;

	MEMORY_REGION memSectorList[1000];

	int memSectorIndex = 0;
	while (VirtualQueryEx(processHandle, (LPCVOID)dwAddress, &mbi, sizeof(mbi)) && (dwAddress < dwEndAddr) && ((dwAddress + mbi.RegionSize) > dwAddress)) {
		if ((mbi.State == MEM_COMMIT) &&
			((mbi.Protect & PAGE_GUARD) == 0) &&
			(mbi.Protect != PAGE_NOACCESS) &&
			((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE)
			)
		{
			MEMORY_REGION mData = { 0 };
			mData.dwBaseAddr = (DWORD_PTR)mbi.BaseAddress;
			mData.dwMemorySize = mbi.RegionSize;
			m_vMemoryRegion.push_back(mData);
			memSectorList[memSectorIndex] = mData;
			memSectorIndex++;
		}
		dwAddress = (DWORD)mbi.BaseAddress + mbi.RegionSize;
	}

	//int memSectorCount = memSectorIndex;
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
			int iOffset{ find(pCurrMemoryData, regionSize, bSearchData, nSearchSize) };
			while (iOffset != -1) {
				dwOffset += iOffset;
				vRet.push_back(dwOffset + curAddr);
				dwOffset += nSearchSize;
				iOffset = find(pCurrMemoryData + dwOffset, regionSize - dwOffset - nSearchSize, bSearchData, nSearchSize);
			}
			delete[] pCurrMemoryData;
		}
		++memSectorIndex;
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
	for (i = 0; i < dwBufferSize; i++)
	{
		for (j = 0; j < dwStrLen; j++)
		{
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

bool MemoryManager::LoadDriver()
{
	bool bLoad = false;
	SC_HANDLE hSCManager{ 0 };
	SC_HANDLE hService{ 0 };

	// Establishes a connection to the service control manager on the specified computer.
	// Opens the specified service control manager database.
	hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		DWORD error{ GetLastError() };

		if (error == ERROR_ACCESS_DENIED)
		{
			std::cout << "<OpenSCManager> The requested access was denied\n";
			goto CLEANUP;
		}

		if (error == ERROR_DATABASE_DOES_NOT_EXIST)
		{
			std::cout << "<OpenSCManager> The specified database does not exist\n";
			goto CLEANUP;
		}

		std::cout << "<OpenSCManager> Error: " << error << '\n';
		goto CLEANUP;
	}

	// Get driver path
	if (Utils::is_file_exist("kph.sys"))
	{
		// Get executable path
		wchar_t buffer_exe_path[512];
		GetModuleFileNameW(NULL, buffer_exe_path, 512);
		std::wstring exe_path(buffer_exe_path);

		// Remove executable name and append driver name
		m_DriverPath = exe_path.substr(0, exe_path.find_last_of('\\')) + L"\\kph.sys";
		std::wcout << L"Driver path: " << m_DriverPath << std::endl;
	}
	else
	{
		std::cout << "<!> Can't file kph.sys file!\n";
		goto CLEANUP;
	}

	// Trying to get the handle of the service if it exist
	hService = OpenServiceW(hSCManager, L"KPH", STANDARD_RIGHTS_REQUIRED | SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS | SERVICE_START | SERVICE_STOP);
	// The service of the driver isn't exist so we are trying to create a new one
	if (hService == NULL)
	{
		hService = CreateServiceW(
			hSCManager,
			L"KPH",
			L"KPH", 
			SERVICE_ALL_ACCESS | SERVICE_START | DELETE | SERVICE_STOP,
			SERVICE_KERNEL_DRIVER, 
			SERVICE_DEMAND_START, 
			SERVICE_ERROR_IGNORE,
			m_DriverPath.c_str(), 
			NULL,
			NULL, 
			NULL, 
			NULL, 
			NULL
		);

		// Can't create service for the driver
		if (hService == NULL)
		{
			DWORD err{ GetLastError() };

			if (err == ERROR_ACCESS_DENIED)
				std::cout << "<!> The handle to the SCM database does not have the SC_MANAGER_CREATE_SERVICE access right\n";

			if (err == ERROR_DUPLICATE_SERVICE_NAME)
				std::cout << "<!> The display name already exists in the service control manager database either as a service name or as another display name\n";

			if (err == ERROR_INVALID_HANDLE)
				std::cout << "<!> The handle to the specified service control manager database is invalid\n";

			if (err == ERROR_SERVICE_EXISTS)
				std::cout << "<!> The specified service already exists in this database\n";

			if (err == ERROR_SERVICE_MARKED_FOR_DELETE)
				std::cout << "<!> The specified service already exists in this database and has been marked for deletion\n";

			goto CLEANUP;
		}
		else
		{
			bLoad = true;
			std::cout << "Created driver service\n";
		}
	}
	// The driver service í created before.
	else
	{
		bLoad = true;
		std::cout << "The driver service is already created!\n";
	}
	
CLEANUP:
	if (hService != NULL)
		CloseServiceHandle(hService);

	if (hSCManager != NULL)
		CloseServiceHandle(hSCManager);

	return bLoad;
}

bool MemoryManager::StartDriver()
{
	bool bStart = false;
	SC_HANDLE hSCManager{ 0 };
	SC_HANDLE hService{ 0 };

	// Establishes a connection to the service control manager on the specified computer.
	// Opens the specified service control manager database.
	hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		DWORD error{ GetLastError() };

		if (error == ERROR_ACCESS_DENIED)
		{
			std::cout << "<OpenSCManager> The requested access was denied\n";
			goto CLEANUP;
		}

		if (error == ERROR_DATABASE_DOES_NOT_EXIST)
		{
			std::cout << "<OpenSCManager> The specified database does not exist\n";
			goto CLEANUP;
		}

		std::cout << "<OpenSCManager> Error: " << error << '\n';
		goto CLEANUP;
	}

	// Open an existing driver service
	hService = OpenServiceW(hSCManager, L"KPH", SERVICE_ALL_ACCESS | SERVICE_START | DELETE | SERVICE_STOP);
	if (!hService)
	{
		DWORD error = GetLastError();

		if (error == 1060)
		{
			std::cout << "<OpenService> No service found by name";
			goto CLEANUP;
		}

		std::cout << "<OpenService> Error: " << error << '\n';
		goto CLEANUP;
	}

	if (StartServiceW(hService, NULL, NULL))
	{
		bStart = true;
		std::cout << "Started the driver service\n";
	}
	else
	{
		DWORD err{ GetLastError() };

		if (err == ERROR_SERVICE_ALREADY_RUNNING)
		{
			bStart = true;
			std::cout << "The driver service is already running!\n";
			goto CLEANUP;
		}

		if (err == ERROR_INVALID_IMAGE_HASH)
		{
			std::cout << "<StartService> Cannot Verify Digital Signature\n";
			goto CLEANUP;
		}

		// Error 193
		if (err == 0xC1)
		{
			std::cout << "<StartService> The driver path is invalid!\n";
			goto CLEANUP;
		}

		// Error 183
		// Cannot create a file when that file already exists.
		if (err == 0xb7)
		{
			bStart = true;
			m_bUsingAnotherDriverService = true;
			std::cout << "There is a running driver service with different name.\nNo need to use created service!\n";
			goto CLEANUP;
		}

		// Can't start the driver service because it's disabled so we will change the service config
		if (err == ERROR_SERVICE_DISABLED)
		{
			std::cout << "Driver service was created but disabled. Trying to active it\n";

			bool bStatus = ChangeServiceConfigW(
				hService,
				SERVICE_NO_CHANGE,
				SERVICE_DEMAND_START,
				SERVICE_NO_CHANGE,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL
			);

			if (!bStatus)
			{
				std::cout << "<StartService> Can't change driver service state! Error: " << GetLastError() << '\n';
				goto CLEANUP;
			}
			else
			{
				std::cout << "Changed driver service state!\n";

				if (StartServiceW(hService, NULL, NULL))
				{
					bStart = true;
					std::cout << "Started the driver service!\n";
				}
				else
				{
					std::cout << "<StartService> Still can't start the driver service!\n";
				}

				goto CLEANUP;
			}
		}

		// There is an another error
		std::cout << "<StartService> Error: " << err << '\n';
		goto CLEANUP;
	}

CLEANUP:
	if (hService != NULL)
		CloseServiceHandle(hService);

	if (hSCManager != NULL)
		CloseServiceHandle(hSCManager);

	return bStart;
}

bool MemoryManager::StopDriver()
{
	bool bStopped = false;
	SC_HANDLE hSCManager{ 0 };
	SC_HANDLE hService{ 0 };
	SERVICE_STATUS proc;

	// Establishes a connection to the service control manager on the specified computer.
	// Opens the specified service control manager database.
	hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		DWORD error{ GetLastError() };

		if (error == ERROR_ACCESS_DENIED)
		{
			std::cout << "<OpenSCManager> The requested access was denied\n";
			goto CLEANUP;
		}

		if (error == ERROR_DATABASE_DOES_NOT_EXIST)
		{
			std::cout << "<OpenSCManager> The specified database does not exist\n";
			goto CLEANUP;
		}

		std::cout << "<OpenSCManager> Error: " << error << '\n';
		goto CLEANUP;
	}

	// Open an existing driver service
	hService = OpenServiceW(hSCManager, L"KPH", SERVICE_ALL_ACCESS | SERVICE_START | DELETE | SERVICE_STOP);
	if (!hService)
	{
		DWORD error = GetLastError();

		if (error == 1060)
		{
			std::cout << "<OpenService> No service found by name";
			goto CLEANUP;
		}

		std::cout << "<OpenService> Error: " << error << '\n';
		goto CLEANUP;
	}

	// Sends a control code to a service
	if (ControlService(hService, SERVICE_CONTROL_STOP, &proc))
	{
		bStopped = true;
		std::cout << "Stopped driver service\n";
	}
	else
	{
		DWORD error{ GetLastError() };
		std::cout << "<ControlService> Error: " << error << '\n';
	}

CLEANUP:
	if (hSCManager != NULL)
		CloseServiceHandle(hSCManager);

	if (hService != NULL)
		CloseServiceHandle(hService);

	return bStopped;
}

bool MemoryManager::UnloadDriver()
{
	bool bUnload = false;
	SC_HANDLE hSCManager{ 0 };
	SC_HANDLE hService{ 0 };

	// Establishes a connection to the service control manager on the specified computer.
	// Opens the specified service control manager database.
	hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		DWORD error{ GetLastError() };

		if (error == ERROR_ACCESS_DENIED)
		{
			std::cout << "<OpenSCManager> The requested access was denied\n";
			goto CLEANUP;
		}

		if (error == ERROR_DATABASE_DOES_NOT_EXIST)
		{
			std::cout << "<OpenSCManager> The specified database does not exist\n";
			goto CLEANUP;
		}

		std::cout << "<OpenSCManager> Error: " << error << '\n';
		goto CLEANUP;
	}

	// Open an existing driver service
	hService = OpenServiceW(hSCManager, L"KPH", SERVICE_ALL_ACCESS | SERVICE_START | DELETE | SERVICE_STOP);
	if (!hService)
	{
		DWORD error = GetLastError();

		if (error == 1060)
		{
			std::cout << "<!> No service found by name";
			goto CLEANUP;
		}

		std::cout << "<OpenService> Error: " << error << '\n';
		goto CLEANUP;
	}

	if (DeleteService(hService))
	{
		bUnload = true;
		std::cout << "Deleted driver service\n";
	}
	else
	{
		std::cout << "<DeleteService> Error: " << GetLastError << '\n';
	}

CLEANUP:
	if (hSCManager != NULL)
		CloseServiceHandle(hSCManager);

	if (hService != NULL)
		CloseServiceHandle(hService);

	return bUnload;
}