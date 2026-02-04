#pragma once
class CZygoSystemCommandsService
{
	void* m_pParent;

public:
	CZygoSystemCommandsService(void* pParent = NULL);
	~CZygoSystemCommandsService();

public:
	CString GetComputerName(); // 1111
	CString GetOSName(); // 1112
	long GetRamSize(); // 1113
	CString GetBinDir(); // 1121
	CString GetWorkingDir(); // 1122
	CString GetOpenDir(int fileType); // 1123
	void SetOpenDir(int nFileType, CString dirPath); // 1124
	CString GetSaveDir(int nFileType); // 1125
	void SetSaveDir(int nFileType, CString dirPath); // 1126
};

