#pragma once
#include "SimpleClient.h"

//#define ZYGO_ADDR _T("100.100.101.234")
//#define ZYGO_PORT _T("10000")
#define ZYGO_ADDR _T("127.0.0.1")
#define ZYGO_PORT _T("12345")

// CZygo

class CZygo : public CWnd
{
	DECLARE_DYNAMIC(CZygo)

	CWnd* m_pParent;
	HWND m_hParent;
	CSimpleClient *m_pClient;

	BOOL m_bIsWait;

	BOOL Start(CString sSvrAddr, CString sSvrPort);
	BOOL End();
	BOOL Send(char* pBuffer);

public:
	CZygo(CWnd* pParent = NULL, CString sSvrAddr = ZYGO_ADDR, CString sSvrPort = ZYGO_PORT);
	virtual ~CZygo();

public:
	BOOL IsConnected();
	void ZygoSelectTurret(INT32 nTurret); // 562


protected:
	afx_msg LRESULT wmClientReceived(WPARAM wParam, LPARAM lParam);

protected:
	DECLARE_MESSAGE_MAP()
};


