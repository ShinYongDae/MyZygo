#pragma once
#include "ZygoClient.h"

#define MAX_DELAY_TCP (60*1000)

//#define ZYGO_ADDR _T("100.100.100.125")
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
	BOOL GetReturn(BOOL bWait);	// bWait : 응답이 있을 때 까지 대기
	BOOL GetReturnBool(BOOL bWait);	// bWait : 응답이 있을 때 까지 대기
	double GetReturnDouble(BOOL bWait);	// bWait : 응답이 있을 때 까지 대기
	CString GetReturnString(BOOL bWait);	// bWait : 응답이 있을 때 까지 대기

	// for Zygo Connection ..................................................
	void ClearReturn();
	BOOL IsReturn();

	BOOL ZygoConnected(); // 122
	void Instrument_AutoFocus(); // 551
	void Instrument_MoveTurret(int position); // 562
	void Instrument_SetZoom(double nZoom); // 572
	void Instrument_SetLightLevel(double dlightLevel); // 582
	void Instrument_AutoLightLevel(); // 554
	double Instrument_GetLightLevel(); // 581
	void Motion_Home(int nZygoAxis, BOOL bWait); // 811
	double Motion_GetZPos(); // 835
	CString Motion_MoveZ(double dPos); // 825
	BOOL Motion_IsZStopSet(); // 871

public:
	CZygo(CWnd* pParent = NULL, CString sSvrAddr = ZYGO_ADDR, CString sSvrPort = ZYGO_PORT);
	virtual ~CZygo();

public:
	BOOL IsConnected();

	BOOL IsConnectedMainUI();  // 122
	void AutoFocus(); // 551
	void MoveTurret(int nTurret); // 562
	void SetZoom(double dZoom); // 572
	double GetLightLevel(); // 581
	void SetLightLevel(double dLightDN); // 582
	void AutoLightLevel(); // 554
	void HomeX(BOOL bWait); // 811 
	void HomeY(BOOL bWait); // 811 
	void HomeZ(BOOL bWait); // 811 
	double GetZPos(); // 835
	CString MoveZ(double dPos, BOOL bAbs = TRUE); // 825
	BOOL IsZStopSet();    // 871

public:
	void OnDataReceived(BYTE* pBuffer, DWORD dwCount);

protected:
	afx_msg LRESULT wmClientReceived(WPARAM wParam, LPARAM lParam);

protected:
	DECLARE_MESSAGE_MAP()
};


