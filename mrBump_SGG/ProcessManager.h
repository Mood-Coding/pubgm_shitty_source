#pragma once
#include "pch.h"

#ifndef _PROCESSMANAGER_H_
#define _PROCESSMANAGER_H_

class ProcessManager {
public:
	ProcessManager();
	~ProcessManager();

	bool Init(std::wstring processName);
	bool SetPrivilege(LPCWSTR lpszPrivilege, BOOL bEnablePrivilege);

	DWORD processPID = 0;
	HANDLE hProcess = 0;

	std::wstring emuProcName{};
};

extern ProcessManager* g_pPM;

#endif // !_PROCESSMANAGER_H_


