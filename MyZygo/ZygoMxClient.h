#pragma once

#include <thread>
#include <WinSock2.h>

#include "Packet.h"

#include "ZygoEnumerations.h"
#include "ZygoInstrumentService.h"
#include "ZygoMasksService.h"
#include "ZygoMotionService.h"
#include "ZygoMxService.h"
#include "ZygoPatternService.h"
#include "ZygoRecipeService.h"
#include "ZygoSystemCommandsService.h"
#include "ZygoUserInterfaceService.h"

using namespace ZygoEnumeration;
using namespace ZygoEnumerations;

#define WM_CLIENT_RECEIVED	(WM_USER + 0x73FF)	// 0x7FFF (Range of WM_USER is 0x400 ~ 0x7FFF)

#define BUFSIZE 16384
#define MAX_DELAY_TCP (60*1000)

// CZygoMxClient
class CZygoMxClient //: public CWnd
{
	//DECLARE_DYNAMIC(CZygoMxClient)

	CWnd* m_pParent;
	HWND m_hParent;
	//SOCKET clientSocket;
	BOOL m_bThreadAlive, m_bThreadStateEnd;
	std::thread t1;

	BOOL m_bConnected;
	CString m_sReturn;

	//BOOL CreateWndForm(DWORD dwStyle);
	BOOL CreateService();

	void StringToChar(CString str, char* szStr);
	void StringToTChar(CString str, TCHAR* tszStr);
	CString CharToString(char *szStr);
	void ThreadStart();
	void ThreadStop();
	void ClearReadBuffer();

private:
	void OnDataReceived(BYTE* pBuffer, DWORD dwCount);
	void PacketParsing(CPacket packet, int nSize);

public:
	CZygoMxClient(CString sServerIp, int nPort, CWnd* pParent = NULL);
	virtual ~CZygoMxClient();

	CZygoInstrumentService* Instrument;
	CZygoMasksService* Masks;
	CZygoMotionService* Motion;
	CZygoMxService* Mx;
	CZygoPatternService* Pattern;
	CZygoRecipeService* Recipe;
	CZygoSystemCommandsService* SystemCommands;
	CZygoUserInterfaceService* UserInterface;
	
	void SetReturn(CString str);
	BOOL IsReturn();
	void ClearReturn();
	BOOL GetReturn(BOOL bWait);	// bWait : 응답이 있을 때 까지 대기
	BOOL GetReturnBool(BOOL bWait);	// bWait : 응답이 있을 때 까지 대기
	double GetReturnDouble(BOOL bWait);	// bWait : 응답이 있을 때 까지 대기
	CString GetReturnString(BOOL bWait);	// bWait : 응답이 있을 때 까지 대기
	BOOL ZygoConnected(); // 122

public:
	char* m_pReadBuffer;
	SOCKET clientSocket;

	static void ProcThrd(const LPVOID lpContext);
	BOOL ProcReceive();
	BOOL Send(CString sSend);
	BOOL Send(char* pBuffer);
	BOOL Send(char* pBuffer, int nLen);
	BOOL ThreadIsAlive();
	BOOL IsConnected();

protected:
	void ThreadEnd();

protected:
	//DECLARE_MESSAGE_MAP()
};


