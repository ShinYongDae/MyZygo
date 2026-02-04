// ZygoMxClient.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ZygoMxClient.h"
//#include "Zygo.h"

// CZygoMxClient

//IMPLEMENT_DYNAMIC(CZygoMxClient, CWnd)

CZygoMxClient::CZygoMxClient(CString sServerIp, int nPort, CWnd* pParent/*=NULL*/)
{
	m_hParent = NULL;
	m_pParent = pParent;
	if(pParent)
		m_hParent = pParent->GetSafeHwnd();

	m_bConnected = FALSE;
	m_pReadBuffer = new char[BUFSIZE];

	//윈도우 소켓(원속) 초기화
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		AfxMessageBox(_T("실패 - 윈도우 소켓(윈속) 초기화"));
		return;
	}

	//소켓 확인(connect)
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		AfxMessageBox(_T("socket() Error"));
		return;
	}

	//연결 확인
	int nLen = sServerIp.GetLength() + 1; // for '\0'
	char* cServerIp = new char[nLen];
	StringToChar(sServerIp, cServerIp);

	int recval;
	SOCKADDR_IN clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(nPort);
	clientAddr.sin_addr.S_un.S_addr = inet_addr(cServerIp);
	recval = connect(clientSocket, (SOCKADDR *)&clientAddr, sizeof(clientAddr));
	delete cServerIp;
	if (recval == SOCKET_ERROR)
	{
		AfxMessageBox(_T("connect() Error"));
		return;
	}

	m_bConnected = TRUE;
	m_sReturn = _T("");
	//CreateWndForm(WS_CHILD | WS_OVERLAPPED);

	CreateService();

	ThreadStart();
}

CZygoMxClient::~CZygoMxClient()
{
	//소켓 종료
	closesocket(clientSocket);

	ThreadStop();
	Sleep(30);
	t1.join();

	if (Instrument)
	{
		delete Instrument;
		Instrument = NULL;
	}
	if (Masks)
	{
		delete Masks;
		Masks = NULL;
	}
	if (Motion)
	{
		delete Motion;
		Motion = NULL;
	}
	if (Mx)
	{
		delete Mx;
		Mx = NULL;
	}
	if (Pattern)
	{
		delete Pattern;
		Pattern = NULL;
	}
	if (Recipe)
	{
		delete Recipe;
		Recipe = NULL;
	}
	if (SystemCommands)
	{
		delete SystemCommands;
		SystemCommands = NULL;
	}
	if (UserInterface)
	{
		delete UserInterface;
		UserInterface = NULL;
	}
	if (UserFunction)
	{
		delete UserFunction;
		UserFunction = NULL;
	}

	WSACleanup();
	delete m_pReadBuffer;
}


//BEGIN_MESSAGE_MAP(CZygoMxClient, CWnd)
//END_MESSAGE_MAP()



// CZygoMxClient 메시지 처리기입니다.
//BOOL CZygoMxClient::CreateWndForm(DWORD dwStyle)
//{
//	if (!Create(NULL, _T("ZygoMxClient"), dwStyle, CRect(0, 0, 0, 0), m_pParent, (UINT)this))
//	{
//		AfxMessageBox(_T("CZygoMxClient::Create() Failed!!!"));
//		return FALSE;
//	}
//
//	return TRUE;
//}

BOOL CZygoMxClient::CreateService()
{
	Instrument = new CZygoInstrumentService(this);
	Masks = new CZygoMasksService(this);
	Motion = new CZygoMotionService(this);
	Mx = new CZygoMxService(this);
	Pattern = new CZygoPatternService(this);
	Recipe = new CZygoRecipeService(this);
	SystemCommands = new CZygoSystemCommandsService(this);
	UserInterface = new CZygoUserInterfaceService(this);
	UserFunction = new CZygoUserFunctionService(this);

	return TRUE;
}

BOOL CZygoMxClient::IsConnected()
{
	return (m_bThreadAlive && m_bConnected);
}

BOOL CZygoMxClient::Send(CString sSend)
{
	int nLen = sSend.GetLength() + 1; // for '\0'
	char* cSend = new char[nLen];
	StringToChar(sSend, cSend);
	int retval = send(clientSocket, cSend, strlen(cSend), 0);
	delete cSend;
	if (retval == SOCKET_ERROR)
	{
		m_bConnected = FALSE;
		AfxMessageBox(_T("send() Error"));
		return FALSE;
	}
	return TRUE;
}

BOOL CZygoMxClient::Send(char* pBuffer)
{
	int retval = send(clientSocket, pBuffer, strlen(pBuffer), 0);
	if (retval == SOCKET_ERROR)
	{
		m_bConnected = FALSE;
		AfxMessageBox(_T("send() Error"));
		return FALSE;
	}
	return TRUE;
}

BOOL CZygoMxClient::Send(char* pBuffer, int nLen)
{
	int retval = send(clientSocket, pBuffer, nLen, 0);
	if (retval == SOCKET_ERROR)
	{
		m_bConnected = FALSE;
		AfxMessageBox(_T("send() Error"));
		return FALSE;
	}
	return TRUE;
}


void CZygoMxClient::ProcThrd(const LPVOID lpContext)
{
	CZygoMxClient* pZygoMxClient = reinterpret_cast<CZygoMxClient*>(lpContext);

	while (pZygoMxClient->ThreadIsAlive())
	{
		if (!pZygoMxClient->ProcReceive())
			break;
	}

	pZygoMxClient->ThreadEnd();
}

BOOL CZygoMxClient::ProcReceive()
{
	ClearReadBuffer();

	char* readBuffer = m_pReadBuffer;
	fd_set	fdRead = { 0 };
	TIMEVAL	*pstTime = NULL;
	TIMEVAL	stTime;
	DWORD dwTimeout = 0; // 0 second
	int nRecSize = -1;

	if (INFINITE != dwTimeout)
	{
		stTime.tv_sec = dwTimeout / 1000;
		stTime.tv_usec = (dwTimeout * 1000) % 1000000;
		pstTime = &stTime;
	}

	// Set Descriptor
	if (!FD_ISSET(clientSocket, &fdRead))
		FD_SET(clientSocket, &fdRead);

	// Select function set read timeout
	DWORD dwBytesRead = 0L;
	int res = select(clientSocket + 1, &fdRead, NULL, NULL, pstTime);
	if (res > 0)
	{
		char buffer[BUFSIZE] = { 0, };

		nRecSize = recv(clientSocket, buffer, BUFSIZE, 0);
		if (nRecSize > 0)
		{
			//((CZygo*)m_pParent)->OnDataReceived((BYTE*)buffer, nRecSize);
			OnDataReceived((BYTE*)buffer, nRecSize);
			memset((BYTE*)buffer, 0, BUFSIZE);
			//memcpy(readBuffer, buffer, nRecSize);
			//readBuffer[nRecSize] = _T('\0');
			//CString sMsg = CharToString(readBuffer);
			//if (m_pParent && m_hParent)
			//{
			//	//CZygo* pZygo = (CZygo*)m_pParent;
			//	//pZygo->wmClientReceived((WPARAM)0, (LPARAM)(LPCTSTR)sMsg);
			//	//m_pParent->PostMessage(WM_CLIENT_RECEIVED, (WPARAM)0, (LPARAM)(LPCTSTR)sMsg);
			//
			//	::SendMessage(m_hParent, WM_CLIENT_RECEIVED, (WPARAM)0, (LPARAM)(LPCTSTR)sMsg);
			//}
		}
		else if (nRecSize == 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CZygoMxClient::ThreadStart()
{
	m_bThreadStateEnd = FALSE;
	m_bThreadAlive = TRUE;
	t1 = std::thread(ProcThrd, this);
}

void CZygoMxClient::ThreadStop()
{
	m_bThreadAlive = FALSE;
	MSG message;
	const DWORD dwTimeOut = 1000*60*3; // 3 Minute
	DWORD dwStartTick = GetTickCount();
	Sleep(30);
	while (!m_bThreadStateEnd)
	{
		if (GetTickCount() >= (dwStartTick + dwTimeOut))
		{
			AfxMessageBox(_T("WaitUntilThreadEnd() Time Out!!!", NULL, MB_OK | MB_ICONSTOP));
			return;
		}
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
		Sleep(30);
	}
}

void CZygoMxClient::ThreadEnd()
{
	m_bThreadStateEnd = TRUE;
}

BOOL CZygoMxClient::ThreadIsAlive()
{
	return m_bThreadAlive;
}

void CZygoMxClient::ClearReadBuffer()
{
	ZeroMemory(m_pReadBuffer, sizeof(m_pReadBuffer));
}

void CZygoMxClient::SetReturn(CString str)
{
	m_sReturn = str;
}

void CZygoMxClient::ClearReturn()
{
	m_sReturn = _T("");
	//CZygo* pZygo = (CZygo*)m_pParent;
	//pZygo->ClearReturn();
}

BOOL CZygoMxClient::IsReturn()
{
	if (m_sReturn.GetLength() > 0)
		return TRUE;
	return FALSE;
}

void CZygoMxClient::StringToChar(CString str, char* szStr)  // char* returned must be deleted... 
{
	int nLen = str.GetLength();
	strcpy(szStr, CT2A(str));
	szStr[nLen] = _T('\0');
}

void CZygoMxClient::StringToTChar(CString str, TCHAR* tszStr) // TCHAR* returned must be deleted... 
{
	int nLen = str.GetLength() + 1;
	memset(tszStr, 0x00, nLen * sizeof(TCHAR));
	_tcscpy(tszStr, str);
}

CString CZygoMxClient::CharToString(char *szStr)
{
	CString strRet;

	int nLen = strlen(szStr) + sizeof(char);
	wchar_t *tszTemp = NULL;
	tszTemp = new WCHAR[nLen];

	MultiByteToWideChar(CP_ACP, 0, szStr, -1, tszTemp, nLen * sizeof(WCHAR));

	strRet.Format(_T("%s"), (CString)tszTemp);
	if (tszTemp)
	{
		delete[] tszTemp;
		tszTemp = NULL;
	}
	return strRet;
}

BOOL CZygoMxClient::GetReturn(BOOL bWait)
{
	DWORD CurTimer, StartTimer;
	MSG msg;
	StartTimer = GetTickCount();
	CurTimer = GetTickCount();

	while (bWait)
	{
		if (IsReturn())
			return TRUE;

		CurTimer = GetTickCount();
		if (MAX_DELAY_TCP < int(CurTimer - StartTimer))
		{
			//AfxMessageBox(_T("No response from Zygo Server.\r\n"));
			break;
		}
		Sleep(10);
	}
	return FALSE;
}

BOOL CZygoMxClient::GetReturnBool(BOOL bWait)
{
	if (!bWait)	return TRUE;

	BOOL bRtn = FALSE;
	if (GetReturn(bWait))
		bRtn = _ttoi(m_sReturn) ? TRUE : FALSE;
	return bRtn;
}

CString CZygoMxClient::GetReturnString(BOOL bWait)
{
	CString sRtn = _T("");
	if (!bWait)	return sRtn;

	if (GetReturn(bWait))
		sRtn = m_sReturn;
	return sRtn;
}

float CZygoMxClient::GetReturnFloat(BOOL bWait)
{
	float fRtn = 0.0;
	if (!bWait)	return fRtn;

	if (GetReturn(bWait))
		fRtn = _ttof(m_sReturn);
	return fRtn;
}

double CZygoMxClient::GetReturnDouble(BOOL bWait)
{
	double dRtn = 0.0;
	if (!bWait)	return dRtn;

	if (GetReturn(bWait))
		dRtn = _ttof(m_sReturn);
	return dRtn;
}

int CZygoMxClient::GetReturnInt(BOOL bWait)
{
	int nRtn = -1;
	if (!bWait)	return nRtn;

	if (GetReturn(bWait))
		nRtn = _ttoi(m_sReturn);
	return nRtn;
}

long CZygoMxClient::GetReturnLong(BOOL bWait)
{
	long lRtn = -1;
	if (!bWait)	return lRtn;

	if (GetReturn(bWait))
		lRtn = _ttol(m_sReturn);
	return lRtn;
}

BOOL CZygoMxClient::ZygoConnected() // 122
{
	ClearReturn();
	CPacket packet;
	packet << (byte)1; packet << (byte)2; packet << (byte)2;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	BOOL bRtn = GetReturnBool(Send((char*)packet.GetData(), nLen));
	return bRtn;
}

void CZygoMxClient::OnDataReceived(BYTE* pBuffer, DWORD dwSize)
{
	CPacket packet;
	packet.Append(pBuffer, dwSize);
	PacketParsing(packet, dwSize);
}

void CZygoMxClient::PacketParsing(CPacket packet, int nSize)
{
	if (nSize <= 3) return;

	byte nClass, nGroup, nItem;
	packet >> nClass;
	packet >> nGroup;
	packet >> nItem;

	//시간 측정

	//통신에 따른 구동 개시
	switch (nClass)
	{
	case 1: // MxClient Class
		ClassMxClient(nGroup, nItem, packet, nSize);
		break;
	case 2: // Core Class
		ClassCore(nGroup, nItem, packet, nSize);
		break;
	case 3: // Enumerations
		ClassEnumerations(nGroup, nItem, packet, nSize);
		break;
	case 4: // MxService Class 
		ClassMxService(nGroup, nItem, packet, nSize);
		break;
	case 5: // InstrumentService Class
		ClassInstrumentService(nGroup, nItem, packet, nSize);
		break;
	case 6: // MasksService Class
		ClassMasksService(nGroup, nItem, packet, nSize);
		break;
	case 7: // FiducialsServiceClass
		ClassFiducialsService(nGroup, nItem, packet, nSize);
		break;
	case 8: // MotionService Class 
		ClassMotionService(nGroup, nItem, packet, nSize);
		break;
	case 9: // PatternService Class
		ClassPatternService(nGroup, nItem, packet, nSize);
		break;
	case 10: // RecipeService Class
		ClassRecipeService(nGroup, nItem, packet, nSize);
		break;
	case 11: // SystemCommandsService Class
		ClassSystemCommandsService(nGroup, nItem, packet, nSize);
		break;
	case 12: // UserInterfaceService Class 
		ClassUserInterfaceService(nGroup, nItem, packet, nSize);
		break;
	case 13: // UserFunctionService Class 
		ClassUserFunctionService(nGroup, nItem, packet, nSize);
		break;
	}
}

void CZygoMxClient::ClassMxClient(int nGroup, int nItem, CPacket packet, int nSize)					// Class 1
{
	byte nByte;
	BOOL bReturn;
	CString sReturn;

	switch (nGroup)
	{
	case 1: // Client Initialization
		switch (nItem)
		{
		case 1: // public MxClient();
			break;
		case 2: // public MxClient(string host);
			break;
		}
		break;
	case 2: // Client Connection
		switch (nItem)
		{
		case 1: // public void Connect(bool forceIfActive);
				//bool forceIfActive = GetPacketBool(packetData);
				//ZygoConnect(forceIfActive); // 121
			break;
		case 2: // public bool Connected; // 122
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		}
		break;
	case 3: // Client Termination
		switch (nItem)
		{
		case 1: // public void Terminate();
			break;
		}
		break;
	case 4: // Service Class Members
		switch (nItem)
		{
		case 1: // public MxService Mx;
			break;
		case 2: // public InstrumentService Instrument;
			break;
		case 3: // public MasksService Masks;
			break;
		case 4: // public MotionService Motion; 
			break;
		case 5: // public PatternService Pattern;
			break;
		case 6: // public RecipeService Recipe;
			break;
		case 7: // public SystemCommandsService SystemCommands; 
			break;
		case 8: // public UserInterfaceService UserInterface; 
			break;
		}
		break;
	}
}

void CZygoMxClient::ClassCore(int nGroup, int nItem, CPacket packet, int nSize)						// Class 2
{
	switch (nGroup)
	{
	case 1: // ZygoError Class
		switch (nItem)
		{
		case 1: // public ZygoError();
			break;
		case 2: // public ZygoError(string message);
			break;
		case 3: // public ZygoError(string message, Exception inner);
			break;
		}
		break;
	case 2: // Point2D Class
		switch (nItem)
		{
		case 1: // public double x;
			break;
		case 2: // public double y;
			break;
		case 3: // public Point2D(double x, double y);
			break;
		}
		break;
	}
}

void CZygoMxClient::ClassEnumerations(int nGroup, int nItem, CPacket packet, int nSize) 			// Class 3
{
	switch (nGroup)
	{
	case 1: // Unit Enum
		switch (nItem)
		{
		case 1: // public enum Unit
			break;
		}
		break;
	case 2: // AxisType Enum
		switch (nItem)
		{
		case 1: // public enum AxisType
			break;
		}
		break;
	case 3: // FileTypes Enum
		switch (nItem)
		{
		case 1: // public enum FileTypes
			break;
		}
		break;
	case 4: // AlignViewMode Enum
		switch (nItem)
		{
		case 1: // public enum AlignViewMode
			break;
		}
		break;
	case 5: // RingSpotMode Enum
		switch (nItem)
		{
		case 1: // public enum RingSpotMode
			break;
		}
		break;
	case 6: // SequenceOperation Enum
		switch (nItem)
		{
		case 1: // public enum AxisType
			break;
		}
		break;
	case 7: // DialogMode Enum
		switch (nItem)
		{
		case 1: // public enum DialogMode
			break;
		}
		break;
	case 8: // DmiStatus Enum
		switch (nItem)
		{
		case 1: // public enum DmiStatus
			break;
		}
		break;
	}
}

void CZygoMxClient::ClassMxService(int nGroup, int nItem, CPacket packet, int nSize) 				// Class 4
{
	std::string sBuffer = "";
	byte nByte;
	BOOL bReturn;
	CString sReturn;
	double dReturn;

	switch (nGroup)
	{
	case 1: // Application Methods
		switch (nItem)
		{
		case 1: // public bool IsApplicationOpen(); // 411
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		case 2: // public string GetApplicationPath(); // 412
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 3: // public void OpenApplication(string fileName); // 413
			break;
		case 4: // public void CloseApplication(); // 414
			break;
		case 5: // public void SaveApplicationAs(string fileName); // 415
			break;
		case 6: // public void LoadSettings(string fileName); // 416
			break;
		case 7: // public void SaveSettings(string fileName); // 417
			break;
		}
		break;
	case 2: // Settings Methods
		switch (nItem)
		{
		case 1: // public void LoadSettings(string fileName); // 421
			break;
		case 2: // public void SaveSettings(string fileName); // 422
			break;
		}
		break;
	case 3: // Data Methods
		switch (nItem)
		{
		case 1: // public void Analyze(); // 431
			break;
		case 2: // public void LoadData(string fileName); // 432
			break;
		case 3: // public void SaveData(string fileName); // 433
			break;
		case 4: // public void LoadSignalData(string fileName); // 434
			break;
		case 5: // public void SaveSignalData(string fileName); // 435
			break;
		case 6: // public void ResetData(); // 436
			break;
		}
		break;
	case 4: // Results, Attributes, and Controls Methods
		switch (nItem)
		{
		case 1: // public double GetAttributeNumber(string[] path, Unit units); // 441
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 2: // public double GetAttributeNumber(string[] path, string units); // 442
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 3: // public string GetAttributeString(string[] path); // 443
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 4: // public double GetControlNumber(string[] path, Unit units); // 444
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 5: // public double GetControlNumber(string[] path, string units); // 445
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 6: // public string GetControlString(string[] path); // 446
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 7: // public bool GetControlBool(string[] path); // 447
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		case 8: // public double GetResultNumber(string[] path, Unit units); // 448
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 9: // public double GetResultNumber(string[] path, string units); // 449
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 10: // public string GetResultString(string[] path); // 4410
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 11: // public void SetControlNumber(string[] path, double numberValue, Unit units); // 4411
			break;
		case 12: // public void SetControlNumber(string[] path, double numberValue, string units); // 4412
			break;
		case 13: // public void SetControlString(string[] path, string stringValue); // 4413
			break;
		case 14: // public void SetControlBool(string[] path, bool value); // 4414
			break;
		case 15: // public void SetResultNumber(string[] path, double numberValue, Unit units); // 4415
			break;
		case 16: // public void SetResultNumber(string[] path, double numberValue, string units); // 4416
			break;
		case 17: // public void SetResultString(string[] path, string stringValue); // 4417
			break;
		}
		break;
	case 5: // Other Results Methods
		switch (nItem)
		{
		case 1: // public void ClearProcessStats(); // 451
			break;
		case 2: // public void StoreProcessStats(); // 452
			break;
		case 3: // public void LogReports(); // 453
			break;
		}
		break;
	case 6: // Data Matrix Methods
		switch (nItem)
		{
		case 1: // public double GetDataCenterX(Control control, Unit units); // 461
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 2: // public double GetDataCenterX(Control control, string units); // 462
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 3: // public double GetDataCenterY(Control control, Unit units); // 463
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 4: // public double GetDataCenterY(Control control, string units); // 464
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 5: // public double GetDataOriginX(Control control, Unit units); // 465
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 6: // public double GetDataOriginX(Control control, string units); // 466
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 7: // public double GetDataOriginY(Control control, Unit units); // 467
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 8: // public double GetDataOriginY(Control control, string units); // 468
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 9: // public double GetDataSizeX(Control control, Unit units); // 469
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 10: // public double GetDataSizeX(Control control, string units); // 4610
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 11: // public double GetDataSizeY(Control control, Unit units); // 4611
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 12: // public double GetDataSizeY(Control control, string units); // 4612
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		}
		break;
	case 7: // Annotations Grid Methods
		switch (nItem)
		{
		case 1: // public void CreateAnnotation(string annotationLabel, string annotationValue); // 471
			break;
		case 2: // public void DeleteAnnotation(string[] path); // 472
			break;
		case 3: // public void SetAnnotation(string[] path, string value); // 473
			break;
		case 4: // public string GetAnnotation(string[] path); // 474
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		}
		break;
	case 8: // Logging Methods
		switch (nItem)
		{
		case 1: // public void LogInfo(string message); // 481
			break;
		case 2: // public void LogError(string message); // 482
			break;
		case 3: // public void LogFatal(string message); // 483
			break;
		}
		break;
	case 9: // Script Methods
		switch (nItem)
		{
		case 1: // public void RunScript(string fileName); // 491
			break;
		}
		break;
	case 10: // User Defined Waves Methods
		switch (nItem)
		{
		case 1: // public void SetUserDefinedWaves(double valueToSet); // 4101
			break;
		}
		break;
	case 11: // Auto Save Methods
		switch (nItem)
		{
		case 1: // public bool GetAutoSaveDataStatus(); // 4111
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		case 2: // public void SetAutoSaveDataStatus(bool status); // 4112
			break;
		}
		break;
	case 12: // Auto Sequence Methods
		switch (nItem)
		{
		case 1: // public void StartSequence(int count) // 4121
			break;
		case 2: // public void AutoSequence(SequenceOperation sequenceOperation, int startDelay, int interval, string scriptName) // 4122
			break;
		}
		break;
	}
}

void CZygoMxClient::ClassInstrumentService(int nGroup, int nItem, CPacket packet, int nSize) 		// Class 5
{
	std::string sBuffer = "";
	CString sReturn;
	double dReturn;
	float fReturn;
	int nByte;
	BOOL bReturn;

	switch (nGroup)
	{
	case 1: // Align-View Mode Methods
		switch (nItem)
		{
		case 1: // public AlignViewMode GetAlignViewMode(); // 511
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		case 2: // public void SetAlignViewMode(AlignViewMode mode); // 512
			break;
		}
		break;
	case 2: // Ring-Spot Mode Methods
		switch (nItem)
		{
		case 1: // public RingSpotMode GetRingSpotMode(); // 521
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		case 2: // public void SetRingSpotMode(RingSpotMode mode); // 522
			break;
		}
		break;
	case 3: // Acquisition Methods
		switch (nItem)
		{
		case 1: // public string Acquire(bool wait = true); // 531
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 2: // public string Measure(bool wait = true); // 532
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 3: // public int[][] AcquireFrame(int align_view); // 533
			break;
		case 4: // public AcquireFrame(int align_view); // 534
			break;
		}
		break;
	case 4: // Asynchronous Acquisition Methods
		switch (nItem)
		{
		case 1: // public bool IsFrameGrabComplete(string taskId); // 541
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		case 2: // public void WaitForFrameGrabComplete(string taskId, double timeout); // 542
			break;
		case 3: // public bool IsAcquisitionComplete(string taskId); // 543
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		case 4: // public void WaitForAcquisitionComplete(string taskId, double timeout); // 544
			break;
		case 5: // public bool IsMeasureComplete(string taskId); // 545
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		case 6: // public void WaitForMeasureComplete(string taskId, double timeout); // 546
			break;
		}
		break;
	case 5: // Optimization Methods
		switch (nItem)
		{
		case 1: // public void AutoFocus(); // 551
			break;
		case 2: // public void AutoTilt(); // 552
			break;
		case 3: // public void AutoFocusTilt(); // 553
			break;
		case 4: // public void AutoLightLevel(); // 554
			break;
		case 5: // public void AutoLateralCalibration(double length, Unit units); // 555
			break;
		case 6: // public void AutoCenter(); // 556
			break;
		}
		break;
	case 6: // Turret Methods
		switch (nItem)
		{
		case 1: // public int GetTurret(); // 561
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		case 2: // public void MoveTurret(int position); // 562
			break;
		}
		break;
	case 7: // Zoom Methods
		switch (nItem)
		{
		case 1: // public double GetZoom(); // 571
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 2: // public void SetZoom(double zoom); // 572
			break;
		case 3: // public double GetMaximumZoom(); // 573
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 4: // public double GetMinimumZoom(); // 574
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 5: // public void LockZoom(); // 575
			break;
		case 6: // public void UnlockZoom(); // 576
			break;
		}
		break;
	case 8: // Light Level Methods
		switch (nItem)
		{
		case 1: // public double GetLightLevel(); // 581
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 2: // public void SetLightLevel(double lightLevel); // 582
			break;
		}
		break;
	case 9: // Wand Status Method
		switch (nItem)
		{
		case 1: // public void SetWandEnabled(bool enabled); // 591
			break;
		}
		break;
	case 10: // Camera Information Methods
		switch (nItem)
		{
		case 1: // public double GetCameraResolution(Unit units); // 5101
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 2: // public double GetCameraResolution(string units); // 5102
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 3: // public double GetCameraSizeX(Unit units); // 5103
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 4: // public double GetCameraSizeX(string units); // 5104
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 5: // public double GetCameraSizeY(Unit units); // 5105
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 6: // public double GetCameraSizeY(string units); // 5106
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 7: // public void SetCameraResolution(double value, string units); // 5107
			break;
		}
		break;
	case 11: // Instrument Hardware Methods
		switch (nItem)
		{
		case 1: // public void SetSleepModeEnabled(bool enabled); // 5111
			break;
		case 2: // public void GetSystemTypeName(); // 5112
			break;
		case 3: // public int ReadSdoInt8(byte nodeid, int index, int subindex); // 5113
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		case 4: // public void WriteSdoInt8(byte nodeid, int index, int subindex, int val); // 5114
			break;
		case 5: // public int ReadSdoInt16(byte nodeid, int index, int subindex); // 5115
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		case 6: // public void WriteSdoInt16(byte nodeid, int index, int subindex, int val); // 5116
			break;
		case 7: // public int ReadSdoInt32(byte nodeid, int index, int subindex); // 5117
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		case 8: // public void WriteSdoInt32(byte nodeid, int index, int subindex, int val); // 5118
			break;
		case 9: // public float ReadSdoReal32(byte nodeid, int index, int subindex); // 5119
			packet >> fReturn;
			sReturn.Format(_T("%0.6f"), fReturn);
			SetReturn(sReturn);
			break;
		case 10: // public void WriteSdoReal32(byte nodeid, int index, int subindex, float val); // 51110
			break;
		case 11: // public double ReadSdoReal64(byte nodeid, int index, int subindex); // 51111
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 12: // public void WriteSdoReal64(byte nodeid, int index, int subindex, double val); // 51112
			break;
		case 13: // public string ReadSdoStr(byte nodeid, int index, int subindex); // 51113
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 14: // public void WriteSdoStr(byte nodeid, int index, int subindex, string val); // 51114
			break;
		}
		break;
	case 12: // MST Methods
		switch (nItem)
		{
		case 1: // public void CalculateGeometry(); // 5121
			break;
		case 2: // public void CalculateHomogeneity(); // 5122
			break;
		}
		break;
	case 13: // DMI Methods
		switch (nItem)
		{
		case 1: // public bool IsDmiActive(); // 5131
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		case 2: // public DmiStatus GetDmiStatus(); // 5132
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		case 3: // public double GetDmiPosition(string units); // 5133
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 4: // public void ResetDmi(); // 5134
			break;
		}
		break;
	case 14: // DynaPhase Methods
		switch (nItem)
		{
		case 1: // public bool IsCalibrationRequired; // 5141
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		case 2: // public bool IsWizardActive; // 5142
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		case 3: // public DynaPhaseSingle DynaPhaseSingle; // 5143
			break;
		case 4: // public DynaPhaseProduction DynaPhaseProduction; // 5144
			break;
		case 5: // public DynaPhaseMovie DynaPhaseMovie; // 5145
			break;
		case 6: // public System.Drawing.Bitmap CurrentWizardImage; // 5146
			break;
		case 7: // public string PromptText; // 5147
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 8: // public void Start(); // 5148
			break;
		case 9: // public void Next(); // 5149
			break;
		case 10: // public void Cancel(); // 51410
			break;
		case 11: // public System.Drawing.Bitmap CurrentWizardImage; // 51411
			break;
		case 12: // public string PromptText; // 51412
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 13: // public void Start(); // 51413
			break;
		case 14: // public void Next(); // 51414
			break;
		case 15: // public void AbortMeasurement() // 51415
			break;
		case 16: // public void Initialize(); // 51416
			break;
		case 17: // public void StartRecording(); // 51417
			break;
		case 18: // public void Stop(); // 51418
			break;
		case 19: // public void AbortRecording(); // 51419
			break;
		case 20: // public System.Drawing.Bitmap CurrentWizardImage; // 51420
			break;
		case 21: // public string PromptText; // 51421
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 22: // public void Shutdown(); // 51422
			break;
		case 23: // public string[] GetAvailableMovieTypes(); // 51423
			break;
		case 24: // public string[] GetAvailableTriggers(); // 51424
			break;
		case 25: // public string[] GetAvailableSampleTypes(); // 51425
			break;
		case 26: // public int Frames; // 51426
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		case 27: // public int CameraRate; // 51427
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		case 28: // public string TriggerType; // 51428
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 29: // public int TriggerDelay; // 51429
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		case 30: // public string MovieFolder; // 51430
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 31: // public string MovieType; // 51431
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 32: // public string MovieName; // 51432
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 33: // public string SampleType; // 51433
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 34: // public double SampleTime; // 51434
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 35: // public int MovieFileRate; // 51435
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		}
		break;
	}
}

void CZygoMxClient::ClassMasksService(int nGroup, int nItem, CPacket packet, int nSize) 			// Class 6
{
	std::string sBuffer = "";
	CString sReturn;
	double dReturn;
	int nByte;

	switch (nGroup)
	{
	case 1: // Base Class
		switch (nItem)
		{
		case 1: // public Masks GetMasks(); // 611
			break;
		case 2: // public double GetRotation(string maskId, string units); // 612
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		}
		break;
	case 2: // Mask Class
		switch (nItem)
		{
		case 1: // public void MoveAbsolute(double x, double y, Unit unit = Unit.Pixels); // 621
			break;
		case 2: // public void MoveRelative(double x, double y, Unit unit = Unit.Pixels); // 622
			break;
		case 3: // public void Resize(double height, double width, Unit unit = Unit.Pixels); // 623
			break;
		case 4: // public void Rotate(double angle, Unit units); // 624
			break;
		case 5: // public Point2D Center; // 625
			break;
		case 6: // public Point2D GetCenter(Unit units = Unit.Pixels) // 626
			break;
		case 7: // public double Height; // 627
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 8: // public double GetHeight(Unit units = Unit.Pixels) // 628
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 9: // public double Width; // 629
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 10: // public double GetWidth(Unit units = Unit.Pixels) // 6210
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 11: // public string Type; // 6211
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		}
		break;
	case 3: // Masks Class
		switch (nItem)
		{
		case 1: // public void Clear(string maskType = null); // 631
			break;
		case 2: // public void Delete(Mask mask); // 632
			break;
		case 3: // public Mask GetMaskClosestTo(double x, double y, string maskType = null, Unit unit=Unit.Pixels); // 633
			break;
		case 4: // public int GetNumMasks(string maskType = null); // 634
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		case 5: // public void Load(string fileName); // 635
			break;
		case 6: // public void Save(string fileName); // 636
			break;
		}
		break;
	}
}

void CZygoMxClient::ClassFiducialsService(int nGroup, int nItem, CPacket packet, int nSize) 		// Class 7
{
	std::string sBuffer = "";
	CString sReturn;
	double dReturn;
	int nByte;

	switch (nGroup)
	{
	case 1: // Fiducial Class
		switch (nItem)
		{
		case 1: // public void MoveAbsolute(double x, double y, Unit unit = Unit.Pixels); // 711
			break;
		case 2: // public void MoveRelative(double x, double y, Unit unit = Unit.Pixels); // 712
			break;
		case 3: // public void Resize(double height, double width, Unit unit = Unit.Pixels); // 713
			break;
		case 4: // public void Rotate(double angle, Unit units); // 714
			break;
		case 5: // public Point2D Center; // 715
			break;
		case 6: // public Point2D GetCenter(Unit unit = Unit.Pixels); // 716
			break;
		case 7: // public double Height; // 717
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 8: // public double GetHeight(Unit unit = Unit.Pixels); // 718
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 9: // public double Width; // 719
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 10: // public double GetWidth(Unit unit = Unit.Pixels); // 7110
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 11: // public string Type; // 7111
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		}
		break;
	case 2: // Fiducials Class
		switch (nItem)
		{
		case 1: // public int GetNumSets() // 721
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		case 2: // public void AddSet() // 722
			break;
		case 3: // public void Delete(Fiducial fiducial); // 723
			break;
		case 4: // public void ClearSet(int fiducialSet=0); // 724
			break;
		case 5: // public void DeleteSet(int fiducialSet=0); // 725
			break;
		case 6: // public Fiducial GetFiducialClosestTo(double x, double y, int workingSet=0, Unit units = Unit.Pixels); // 726
			break;
		case 7: // public int GetNumFiducials(int workingSet = null); // 727
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		case 8: // public void Load(string fileName); // 728
			break;
		case 9: // public void Save(string fileName); // 729
			break;
		}
		break;
	}
}

void CZygoMxClient::ClassMotionService(int nGroup, int nItem, CPacket packet, int nSize) 			// Class 8
{
	std::string sBuffer = "";
	CString sReturn;
	double dReturn;
	int nByte;
	BOOL bReturn;

	switch (nGroup)
	{
	case 1: // Home Axes Methods
		switch (nItem)
		{
		case 1: // public string Home(AxisType[] axes, bool wait); // 811
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 2: // public bool IsHomed(AxisType axis); // 812
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		}
		break;
	case 2: // Move Axes Methods
		switch (nItem)
		{
		case 1: // public string MoveX(double xPosition, Unit unit, bool wait); // 821
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 2: // public string MoveX(double xPosition, string unit, bool wait); // 822
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 3: // public string MoveY(double yPosition, Unit unit, bool wait); // 823
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 4: // public string MoveY(double yPosition, string unit, bool wait); // 824
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 5: // public string MoveZ(double zPosition, Unit unit, bool wait); // 825 // unit = Unit.MilliMeters
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 6: // public string MoveZ(double zPosition, string unit, bool wait); // 826
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 7: // public string MoveXY(double xPosition, double yPosition, Unit unit, bool wait); // 827
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 8: // public string MoveXY(double xPosition, double yPosition, string unit, bool wait); // 828
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 9: // public string MoveXYZ(double xPosition, double yPosition, double zPosition, Unit unit, bool wait); // 829
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 10: // public string MoveXYZ(double xPosition, double yPosition, double zPosition, string unit, bool wait); // 8210
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 11: // public string MoveP(double pPosition, Unit unit, bool wait, bool isParcentric=false); // 8211
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 12: // public string MoveP(double pPosition, string unit, bool wait, bool isParcentric=false); // 8212
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 13: // public string MoveR(double rPosition, Unit unit, bool wait, bool isParcentric=false); // 8213
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 14: // public string MoveR(double rPosition, string unit, bool wait, bool isParcentric=false); // 8214
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 15: // public string MoveRP(double rPosition, double pPosition, Unit unit, bool wait, bool isParcentric=false); // 8215
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 16: // public string MoveRP(double rPosition, double pPosition, string unit, bool wait, bool isParcentric=false); // 8216
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 17: // public string MoveT(double tPosition, Unit unit, bool wait); // 8217
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 18: // public string MoveT(double tPosition, string unit, bool wait); // 8218
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		}
		break;
	case 3: // Retrieve Current Position Methods
		switch (nItem)
		{
		case 1: // public double GetXPos(Unit unit); // 831
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 2: // public double GetXPos(string unit); // 832
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 3: // public double GetYPos(Unit unit); // 833
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 4: // public double GetYPos(string unit); // 834
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 5: // public double GetZPos(Unit unit); // 835
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 6: // public double GetZPos(string unit); // 836
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 7: // public double GetPPos(Unit unit); // 837
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 8: // public double GetPPos(string unit); // 838
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 9: // public double GetRPos(Unit unit); // 839
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 10: // public double GetRPos(string unit); // 8310
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 11: // public double GetTPos(Unit unit); // 8311
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 12: // public double GetTPos(string unit); // 8312
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		}
		break;
	case 4: // Wait On Axes Methods
		switch (nItem)
		{
		case 1: // public void Wait(AxisType[] axes, double timeout); // 841
			break;
		case 2: // public void WaitForStageTaskComplete(string taskId, double timeout); // 842
			break;
		}
		break;
	case 5: // Axis Availability Methods
		switch (nItem)
		{
		case 1: // public bool IsActive(AxisType axis); // 851
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		}
		break;
	case 6: // Pendant Status Method
		switch (nItem)
		{
		case 1: // public void SetPendantEnabled(bool enabled); // 861
			break;
		}
		break;
	case 7: // Z-Stop Status Method
		switch (nItem)
		{
		case 1: // public bool IsZStopSet(); // 871
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		}
		break;
	}
}

void CZygoMxClient::ClassPatternService(int nGroup, int nItem, CPacket packet, int nSize)			// Class 9
{
	std::string sBuffer = "";

	switch (nGroup)
	{
	case 1: // Save/Load Pattern Methods
		switch (nItem)
		{
		case 1: // public void Load(string fileName); // 911
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 2: // public void Save(string fileName); // 912
			break;
		}
		break;
	case 2: // Run Pattern Methods
		switch (nItem)
		{
		case 1: // public void Align(); // 921
			break;
		case 2: // public void PreAlign(); // 922
			break;
		case 3: // public void Run(); // 923
			break;
		}
		break;
	}
}

void CZygoMxClient::ClassRecipeService(int nGroup, int nItem, CPacket packet, int nSize)			// Class 10
{
	switch (nGroup)
	{
	case 1: // Save/Load Recipe Methods
		switch (nItem)
		{
		case 1: // public void Load(string fileName); // 1011
			break;
		case 2: // public void Save(string fileName); // 1012
			break;
		}
		break;
	case 2: // Run Recipe Method
		switch (nItem)
		{
		case 1: // public void Run(); // 1021
			break;
		}
		break;
	}
}

void CZygoMxClient::ClassSystemCommandsService(int nGroup, int nItem, CPacket packet, int nSize)	// Class 11
{
	std::string sBuffer = "";
	int nByte;
	CString sReturn;

	switch (nGroup)
	{
	case 1: // Host Information Methods
		switch (nItem)
		{
		case 1: // public string GetComputerName(); // 1111
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 2: // public string GetOSName(); // 1112
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 3: // public long GetRamSize(); // 1113
			packet >> nByte;
			sReturn.Format(_T("%d"), nByte);
			SetReturn(sReturn);
			break;
		}
		break;
	case 2: // Directory Information Methods
		switch (nItem)
		{
		case 1: // public string GetBinDir(); // 1121
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 2: // public string GetWorkingDir(); // 1122
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 3: // public string GetOpenDir(FileTypes fileType); // 1123				
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 4: // public void SetOpenDir(FileTypes fileType, string dirPath); // 1124
			break;
		case 5: // public string GetSaveDir(FileTypes fileType); // 1125
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 6: // public void SetSaveDir(FileTypes fileType, string dirPath); // 1126
			break;
		}
		break;
	}
}

void CZygoMxClient::ClassUserInterfaceService(int nGroup, int nItem, CPacket packet, int nSize)		// Class 12
{
	std::string sBuffer = "";
	int nByte;
	CString sReturn;
	BOOL bReturn;
	double dReturn;

	switch (nGroup)
	{
	case 1: // Mx GUI Component
		switch (nItem)
		{
		case 1: // Not Exist // 1211
			break;
		}
		break;
	case 2: // Modal Dialogs
		switch (nItem)
		{
		case 1: // public bool ShowDialog(string text, DialogMode mode); // 1221
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		case 2: // public string ShowInputDialog(string text, string defaultValue, Dialog-Mode mode, int maxLength); // 1222
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 3: // public void ShowTimedDialog(string text, DialogMode mode, int seconds); // 1223
			break;
		}
		break;
	case 3: // Toolbar Click Methods
		switch (nItem)
		{
		case 1: // public void ClickToolbarButton(string[] buttonPath); // 1231
			break;
		}
		break;
	case 4: // Image Grid Method
		switch (nItem)
		{
		case 1: // public void SetImageGrid(Control control, string imagePath); // 1241
			break;
		}
		break;
	case 5: // Tab Class Method
		switch (nItem)
		{
		case 1: // public Tab GetTab(string name); // 1251
			break;
		case 2: // public List<Tab> GetTabs(); // 1252
			break;
		case 3: // public DockPanel GetDockPanel(string name); // 1253
			break;
		case 4: // public Group GetGroup(string name); // 1254
			break;
		case 5: // public void Show(); // 1255
			break;
		case 6: // public string Name; // 1256
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 7: // public Navigator Navigator; // 1257
			break;
		case 8: // public List<DockPanel> DockPanels; // 1258
			break;
		case 9: // public List<Group> Groups; // 1259
			break;
		}
		break;
	case 6: // Navigator Class
		switch (nItem)
		{
		case 1: // public void Pin(bool doPin); // 1261
			break;
		}
		break;
	case 7: // DockPanel Class
		switch (nItem)
		{
		case 1: // public void Pin(bool doPin); // 1271
			break;
		case 2: // public string Name; // 1272
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		}
		break;
	case 8: // Group Class
		switch (nItem)
		{
		case 1: // public Container GetContainer(string containerName); // 1281
			break;
		case 2: // public string Name; // 1282
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 3: // public List<Container> Containers; // 1283
			break;
		}
		break;
	case 9: // Container Class
		switch (nItem)
		{
		case 1: // public void Show(); // 1291
			break;
		case 2: // public string Name; // 1292
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 3: // public List<Control> Controls; // 1293
			break;
		}
		break;
	case 10: // ContainerWindow Class
		switch (nItem)
		{
		case 1: // public void Show(); // 12101
			break;
		case 2: // public void Close(); // 12102
			break;
		case 3: // public void ToFront(); // 12103
			break;
		case 4: // public void ToBack(); // 12104
			break;
		case 5: // public string Name; // 12105
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 6: // public bool Open; // 12106
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		case 7: // public List<Control> Controls; // 12107
			break;
		}
		break;
	case 11: // Window Class
		switch (nItem)
		{
		case 1: // public Window ShowMaskEditor(); // 12111
			break;
		case 2: // public Window ShowFiducialEditor(); // 12112
			break;
		case 3: // public void Close(); // 12113
			break;
		case 4: // public void SaveData(string filePath); // 12114
			break;
		case 5: // public void SaveImage(string filePath); // 12115
			break;
		case 6: // public void ToBack(); // 12116
			break;
		case 7: // public void ToFront(); // 12117
			break;
		case 8: // public string Name; // 12118
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 9: // public bool Open; // 12119
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		case 10: // public List<Control> Controls; // 121110
			break;
		case 11: // public void SetChartLimits(string controlId, string axis, double low, double high, string unit); // 121111
			break;
		case 12: // public void SetChartLimits(string controlId, string axis, double low, double high, string unit); // 121112
			break;
		case 13: // public void ClearChartLimits(string controlId, string axis, string limit); // 121113
			break;
		}
		break;
	case 12: // Control Class
		switch (nItem)
		{
		case 1: // public Control GetControl(string[] path); // 12121
			break;
		case 2: // public void SaveData(string filePath, ISaveDataParameters saveDataParameters = null); // 12122
			break;
		case 3: // public void SaveImage(string filePath); // 12123
			break;
		case 4: // public string Name; // 12124
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 5: // public List<Control> Controls; // 12125
			break;
		}
		break;
	case 13: // Saving Control Data
		switch (nItem)
		{
		case 1: // public bool SimpleMode; // 12131
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		case 2: // public bool StandardFormat; // 12132
			packet >> nByte;
			bReturn = nByte ? TRUE : FALSE;
			sReturn.Format(_T("%d"), bReturn ? 1 : 0);
			SetReturn(sReturn);
			break;
		case 3: // public string CodeVTitle; // 12133
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 4: // public string CodeVType; // 12134
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 5: // public string CodeVComment; // 12135
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 6: // public string SdfManufacturer; // 12136
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		case 7: // public DateTime SdfCreateDate; // 12137
			break;
		case 8: // public DateTime SdfModificationDate; // 12138
			break;
		case 9: // public double SdfWavelength; // 12139
			packet >> dReturn;
			sReturn.Format(_T("%0.6f"), dReturn);
			SetReturn(sReturn);
			break;
		case 10: // public string SdfDataType; // 121310
			packet >> sBuffer;
			SetReturn((CString)sBuffer.c_str());
			break;
		}
		break;
	case 14: // Print Methods
		switch (nItem)
		{
		case 1: // public void PrintMainWindow(int typeOfPrint, string path); // 12141
			break;
		}
		break;
	case 15: // Plot Methods
		switch (nItem)
		{
		case 1: // public void SetPlotPaletteScale(string controlId, string scaleMode, double peak, double valley, string unit); // 12151
			break;
		case 2: // public void SetPlotPaletteScale(string controlId, string scaleMode, double peak, double valley, string unit); // 12152
			break;
		case 3: // public void SetPlotPalette(string controlId, string paletteName); // 12153
			break;
		}
		break;
	case 16: // Sequence Method
		switch (nItem)
		{
		case 1: // public void SetSequenceStepState(string sequenceId, string sequenceStepDesc, bool onOff); // 12161
			break;
		}
		break;
	}
}

void CZygoMxClient::ClassUserFunctionService(int nGroup, int nItem, CPacket packet, int nSize)		// Class 13
{
	switch (nGroup)
	{
	case 1: // 
		switch (nItem)
		{
		case 1: // 1311
			break;
		}
		break;
	}
}
