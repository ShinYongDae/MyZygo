#pragma once
#include "ZygoClient.h"

#define MAX_DELAY_TCP (60*1000)

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
	CZygoClient *m_pClient;

	BOOL m_bIsWait;

	BOOL Start(CString sSvrAddr, CString sSvrPort);
	BOOL End();
	BOOL Send(char* pBuffer);
	BOOL Send(char* pBuffer, int nLen);

private:
	CString m_sReturn;
	void PacketParsing(CPacket packet, int nSize);
	BOOL GetReturn(BOOL bWait);
	BOOL GetReturnBool(BOOL bWait);

	// for Zygo Connection ..................................................
	void ClearReturn();
	BOOL IsReturn();
	BOOL ZygoConnected(); // 122
	void Instrument_MoveTurret(int position); // 562
	void Instrument_SetZoom(double nZoom); // 572

public:
	CZygo(CWnd* pParent = NULL, CString sSvrAddr = ZYGO_ADDR, CString sSvrPort = ZYGO_PORT);
	virtual ~CZygo();

public:
	BOOL IsConnected();
	BOOL IsConnectedMainUI();

	void SelectTurret(int nTurret); // 562
	void SelectZoom(double dZoom);

public:
	void OnDataReceived(BYTE* pBuffer, DWORD dwCount);

protected:
	afx_msg LRESULT wmClientReceived(WPARAM wParam, LPARAM lParam);

protected:
	DECLARE_MESSAGE_MAP()
};


