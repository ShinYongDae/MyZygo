#pragma once
class CZygoUserInterfaceService
{
	void* m_pParent;
	CList<CString> m_myList;

public:
	CZygoUserInterfaceService(void* pParent = NULL);
	~CZygoUserInterfaceService();

public:
	BOOL ShowDialog(CString text, int nMode); // 1221
	CString ShowInputDialog(CString text, CString defaultValue, int nMode, int nMaxLength); // 1222
	void ShowTimedDialog(CString text, int nMode, int seconds); // 1223
	CString GetTab(CString name); // 1251
	CList<CString>& GetTabs(); // 1252
	void TabShow(CString name); // 1255
	CList<CString>& TabDockPanels(CString name); // 1258
	CList<CString>& TabGroups(CString name); // 1259
	void Pin(BOOL doPin); // 1261
	CList<CString>& TabGroupContainers(CString sTab, CString sGroup); // 1283
	CList<CString>& TabGroupContainerControls(CString sTab, CString sGroup, CString sContainer); // 1293
};

