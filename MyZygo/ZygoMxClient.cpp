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

double CZygoMxClient::GetReturnDouble(BOOL bWait)
{
	double dRtn = 0.0;
	if (!bWait)	return dRtn;

	if (GetReturn(bWait))
		dRtn = _ttof(m_sReturn);
	return dRtn;
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
	int arraySize = nSize;

	byte nClass, nGroup, nItem, nByte;
	packet >> nClass;
	packet >> nGroup;
	packet >> nItem;

	//시간 측정

	CString sTab, /*sPanel,*/ sGroup, sContainer/*, sControl*/;
	CString sTabs, sPanels, sGroups, /*sContainers,*/ sControls;
	CString sName, sResult, sReturn, sId, sPath;
	BOOL bReturn, bWait;
	double dReturn;
	std::string sBuffer = "";

	//통신에 따른 구동 개시
	switch (nClass)
	{
	case 1: // MxClient Class
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
			case 2: // public bool Connected;
				packet >> nByte;
				bReturn = nByte ? TRUE : FALSE; //bReturn = ZygoConnected(); // 122
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
		break;
	case 2: // Core Class
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
		break;
	case 3: // Enumerations
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
		break;
	case 4: // MxService Class 
		switch (nGroup)
		{
		case 1: // Application Methods
			switch (nItem)
			{
			case 1: // public bool IsApplicationOpen();
					//bReturn = Mx_IsApplicationOpen(); // 411
				break;
			case 2: // public string GetApplicationPath();
					//sPath = Mx_GetApplicationPath(); // 412
				break;
			case 3: // public void OpenApplication(string fileName);
					//sPath = GetPacketString(packetData);
					//Mx_OpenApplication(sPath);// 413
				break;
			case 4: // public void CloseApplication();
					//Mx_CloseApplication();// 414
				break;
			case 5: // public void SaveApplicationAs(string fileName);
					//sPath = GetPacketString(packetData);
					//Mx_SaveApplicationAs(sPath); // 415
				break;
			case 6: // public void LoadSettings(string fileName);
				break;
			case 7: // public void SaveSettings(string fileName);
				break;
			}
			break;
		case 2: // Settings Methods
			switch (nItem)
			{
			case 1: // public void LoadSettings(string fileName);
				break;
			case 2: // public void SaveSettings(string fileName);
				break;
			}
			break;
		case 3: // Data Methods
			switch (nItem)
			{
			case 1: // public void Analyze();
					//Mx_Analyze(); // 431
				break;
			case 2: // public void LoadData(string fileName);
				break;
			case 3: // public void SaveData(string fileName);
					//string sMeasureDataDir = "";    // Client가 정해줌
					//double dPosX = 0.0;             // Client가 정해줌
					//double dPosY = 0.0;             // Client가 정해줌
					//int nDataType = 0;              // Client가 정해줌
					//Mx_SaveData(sMeasureDataDir, dPosX, dPosY, nDataType); // 433
				break;
			case 4: // public void LoadSignalData(string fileName);
				break;
			case 5: // public void SaveSignalData(string fileName);
				break;
			case 6: // public void ResetData();
				break;
			}
			break;
		case 4: // Results, Attributes, and Controls Methods
			switch (nItem)
			{
			case 1: // public double GetAttributeNumber(string[] path, Unit units);
				break;
			case 2: // public double GetAttributeNumber(string[] path, string units);
				break;
			case 3: // public string GetAttributeString(string[] path);
				break;
			case 4: // public double GetControlNumber(string[] path, Unit units);
				break;
			case 5: // public double GetControlNumber(string[] path, string units);
				break;
			case 6: // public string GetControlString(string[] path);
				break;
			case 7: // public bool GetControlBool(string[] path);
				break;
			case 8: // public double GetResultNumber(string[] path, Unit units);
				break;
			case 9: // public double GetResultNumber(string[] path, string units);
				break;
			case 10: // public string GetResultString(string[] path);
				break;
			case 11: // public void SetControlNumber(string[] path, double numberValue, Unit units);
				break;
			case 12: // public void SetControlNumber(string[] path, double numberValue, string units);
				break;
			case 13: // public void SetControlString(string[] path, string stringValue);
				break;
			case 14: // public void SetControlBool(string[] path, bool value);
				break;
			case 15: // public void SetResultNumber(string[] path, double numberValue, Unit units);
				break;
			case 16: // public void SetResultNumber(string[] path, double numberValue, string units);
				break;
			case 17: // public void SetResultString(string[] path, string stringValue);
				break;
			}
			break;
		case 5: // Other Results Methods
			switch (nItem)
			{
			case 1: // public void ClearProcessStats();
				break;
			case 2: // public void StoreProcessStats();
				break;
			case 3: // public void LogReports();
				break;
			}
			break;
		case 6: // Data Matrix Methods
			switch (nItem)
			{
			case 1: // public double GetDataCenterX(Control control, Unit units);
				break;
			case 2: // public double GetDataCenterX(Control control, string units);
				break;
			case 3: // public double GetDataCenterY(Control control, Unit units);
				break;
			case 4: // public double GetDataCenterY(Control control, string units);
				break;
			case 5: // public double GetDataOriginX(Control control, Unit units);
				break;
			case 6: // public double GetDataOriginX(Control control, string units);
				break;
			case 7: // public double GetDataOriginY(Control control, Unit units);
				break;
			case 8: // public double GetDataOriginY(Control control, string units);
				break;
			case 9: // public double GetDataSizeX(Control control, Unit units);
				break;
			case 10: // public double GetDataSizeX(Control control, string units);
				break;
			case 11: // public double GetDataSizeY(Control control, Unit units);
				break;
			case 12: // public double GetDataSizeY(Control control, string units);
				break;
			}
			break;
		case 7: // Annotations Grid Methods
			switch (nItem)
			{
			case 1: // public void CreateAnnotation(string annotationLabel, string annotationValue);
				break;
			case 2: // public void DeleteAnnotation(string[] path);
				break;
			case 3: // public void SetAnnotation(string[] path, string value);
				break;
			case 4: // public string GetAnnotation(string[] path);
				break;
			}
			break;
		case 8: // Logging Methods
			switch (nItem)
			{
			case 1: // public void LogInfo(string message);
				break;
			case 2: // public void LogError(string message); 
				break;
			case 3: // public void LogFatal(string message);
				break;
			}
			break;
		case 9: // Script Methods
			switch (nItem)
			{
			case 1: // public void RunScript(string fileName);
				break;
			}
			break;
		case 10: // User Defined Waves Methods
			switch (nItem)
			{
			case 1: // public void SetUserDefinedWaves(double valueToSet);
				break;
			}
			break;
		case 11: // Auto Save Methods
			switch (nItem)
			{
			case 1: // public bool GetAutoSaveDataStatus();
				break;
			case 2: // public void SetAutoSaveDataStatus(bool status);
				break;
			}
			break;
		case 12: // Auto Sequence Methods
			switch (nItem)
			{
			case 1: // public void StartSequence(int count)
				break;
			case 2: // public void AutoSequence(SequenceOperation sequenceOperation, int startDelay, int interval, string scriptName)
				break;
			}
			break;
		}
		break;
	case 5: // InstrumentService Class
		switch (nGroup)
		{
		case 1: // Align-View Mode Methods
			switch (nItem)
			{
			case 1: // public AlignViewMode GetAlignViewMode();
				break;
			case 2: // public void SetAlignViewMode(AlignViewMode mode);
				break;
			}
			break;
		case 2: // Ring-Spot Mode Methods
			switch (nItem)
			{
			case 1: // public RingSpotMode GetRingSpotMode();
				break;
			case 2: // public void SetRingSpotMode(RingSpotMode mode);
				break;
			}
			break;
		case 3: // Acquisition Methods
			switch (nItem)
			{
			case 1: // public string Acquire(bool wait = true);
					//bWait = GetPacketBool(packetData);
					//sReturn = Instrument_Acquire(bWait); // 531
				break;
			case 2: // public string Measure(bool wait = true);
					//bWait = GetPacketBool(packetData);
					//sReturn = Instrument_Measure(bWait); // 532
				packet >> sBuffer;
				SetReturn((CString)sBuffer.c_str());
				break;
			case 3: // public int[][] AcquireFrame(int align_view);
				break;
			case 4: // public AcquireFrame(int align_view);
				break;
			}
			break;
		case 4: // Asynchronous Acquisition Methods
			switch (nItem)
			{
			case 1: // public bool IsFrameGrabComplete(string taskId);
				break;
			case 2: // public void WaitForFrameGrabComplete(string taskId, double timeout);
				break;
			case 3: // public bool IsAcquisitionComplete(string taskId);
					//sId = GetPacketString(packetData);
					//bReturn = Instrument_IsAcquisitionComplete(sId); // 543
				break;
			case 4: // public void WaitForAcquisitionComplete(string taskId, double timeout);
				break;
			case 5: // public bool IsMeasureComplete(string taskId);
					//sId = GetPacketString(packetData);
					//bReturn = Instrument_IsMeasureComplete(sId); // 545
				break;
			case 6: // public void WaitForMeasureComplete(string taskId, double timeout);
				break;
			}
			break;
		case 5: // Optimization Methods
			switch (nItem)
			{
			case 1: // public void AutoFocus();
					//Instrument_AutoFocus(); // 551
				break;
			case 2: // public void AutoTilt();
				break;
			case 3: // public void AutoFocusTilt();
					//Instrument_AutoFocusTilt(); // 553
				break;
			case 4: // public void AutoLightLevel();
					//Instrument_AutoLightLevel(); // 554
				break;
			case 5: // public void AutoLateralCalibration(double length, Unit units);
				break;
			case 6: // public void AutoCenter();
				break;
			}
			break;
		case 6: // Turret Methods
			switch (nItem)
			{
			case 1: // public int GetTurret();
					// 561
				break;
			case 2: // public void MoveTurret(int position);
					//int posTurret = BitConverter.ToInt32(packetData, 0);
					//Instrument_MoveTurret(posTurret); // 562
				break;
			}
			break;
		case 7: // Zoom Methods
			switch (nItem)
			{
			case 1: // public double GetZoom();
					//double dGetZoom = Instrument_GetZoom(); // 571
				break;
			case 2: // public void SetZoom(double zoom);
					//double dSetZoom = BitConverter.ToDouble(packetData, 0);
					//Instrument_SetZoom(dSetZoom); // 572
				break;
			case 3: // public double GetMaximumZoom();
				break;
			case 4: // public double GetMinimumZoom();
				break;
			case 5: // public void LockZoom();
				break;
			case 6: // public void UnlockZoom();
				break;
			}
			break;
		case 8: // Light Level Methods
			switch (nItem)
			{
			case 1: // public double GetLightLevel();
					//double dGetLightLevel = Instrument_GetLightLevel(); // 581
				packet >> dReturn;
				sReturn.Format(_T("%0.6f"), dReturn);
				SetReturn(sReturn);
				break;
			case 2: // public void SetLightLevel(double lightLevel);
					//double lightLevel = BitConverter.ToDouble(packetData, 0);
					//Instrument_SetLightLevel(lightLevel); // 582
				break;
			}
			break;
		case 9: // Wand Status Method
			switch (nItem)
			{
			case 1: // public void SetWandEnabled(bool enabled);
				break;
			}
			break;
		case 10: // Camera Information Methods
			switch (nItem)
			{
			case 1: // public double GetCameraResolution(Unit units);
				break;
			case 2: // public double GetCameraResolution(string units);
				break;
			case 3: // public double GetCameraSizeX(Unit units);
				break;
			case 4: // public double GetCameraSizeX(string units);
				break;
			case 5: // public double GetCameraSizeY(Unit units);
				break;
			case 6: // public double GetCameraSizeY(string units);
				break;
			case 7: // public void SetCameraResolution(double value, string units);
				break;
			}
			break;
		case 11: // Instrument Hardware Methods
			switch (nItem)
			{
			case 1: // public void SetSleepModeEnabled(bool enabled);
				break;
			case 2: // public void GetSystemTypeName();
				break;
			case 3: // public int ReadSdoInt8(byte nodeid, int index, int subindex);
				break;
			case 4: // public void WriteSdoInt8(byte nodeid, int index, int subindex, int val);
				break;
			case 5: // public int ReadSdoInt16(byte nodeid, int index, int subindex);
				break;
			case 6: // public void WriteSdoInt16(byte nodeid, int index, int subindex, int val);
				break;
			case 7: // public int ReadSdoInt32(byte nodeid, int index, int subindex);
				break;
			case 8: // public void WriteSdoInt32(byte nodeid, int index, int subindex, int val);
				break;
			case 9: // public float ReadSdoReal32(byte nodeid, int index, int subindex);
				break;
			case 10: // public void WriteSdoReal32(byte nodeid, int index, int subindex, float val);
				break;
			case 11: // public double ReadSdoReal64(byte nodeid, int index, int subindex);
				break;
			case 12: // public void WriteSdoReal64(byte nodeid, int index, int subindex, double val);
				break;
			case 13: // public string ReadSdoStr(byte nodeid, int index, int subindex);
				break;
			case 14: // public void WriteSdoStr(byte nodeid, int index, int subindex, string val);
				break;
			}
			break;
		case 12: // MST Methods
			switch (nItem)
			{
			case 1: // public void CalculateGeometry();
				break;
			case 2: // public void CalculateHomogeneity();
				break;
			}
			break;
		case 13: // DMI Methods
			switch (nItem)
			{
			case 1: // public bool IsDmiActive();
				break;
			case 2: // public DmiStatus GetDmiStatus();
				break;
			case 3: // public double GetDmiPosition(string units);
				break;
			case 4: // public void ResetDmi();
				break;
			}
			break;
		case 14: // DynaPhase Methods
			switch (nItem)
			{
			case 1: // public bool IsCalibrationRequired;
				break;
			case 2: // public bool IsWizardActive;
				break;
			case 3: // public DynaPhaseSingle DynaPhaseSingle;
				break;
			case 4: // public DynaPhaseProduction DynaPhaseProduction;
				break;
			case 5: // public DynaPhaseMovie DynaPhaseMovie;
				break;
			case 6: // public System.Drawing.Bitmap CurrentWizardImage;
				break;
			case 7: // public string PromptText;
				break;
			case 8: // public void Start();
				break;
			case 9: // public void Next();
				break;
			case 10: // public void Cancel();
				break;
			case 11: // public System.Drawing.Bitmap CurrentWizardImage;
				break;
			case 12: // public string PromptText;
				break;
			case 13: // public void Start();
				break;
			case 14: // public void Next();
				break;
			case 15: // public void AbortMeasurement()
				break;
			case 16: // public void Initialize();
				break;
			case 17: // public void StartRecording();
				break;
			case 18: // public void Stop();
				break;
			case 19: // public void AbortRecording();
				break;
			case 20: // public System.Drawing.Bitmap CurrentWizardImage;
				break;
			case 21: // public string PromptText;
				break;
			case 22: // public void Shutdown();
				break;
			case 23: // public string[] GetAvailableMovieTypes();
				break;
			case 24: // public string[] GetAvailableTriggers();
				break;
			case 25: // public string[] GetAvailableSampleTypes();
				break;
			case 26: // public int Frames;
				break;
			case 27: // public int CameraRate;
				break;
			case 28: // public string TriggerType;
				break;
			case 29: // public int TriggerDelay;
				break;
			case 30: // public string MovieFolder;
				break;
			case 31: // public string MovieType;
				break;
			case 32: // public string MovieName;
				break;
			case 33: // public string SampleType;
				break;
			case 34: // public double SampleTime;
				break;
			case 35: // public int MovieFileRate;
				break;
			}
			break;
		}
		break;
	case 6: // MasksService Class
		switch (nGroup)
		{
		case 1: // Base Class
			switch (nItem)
			{
			case 1: // public Masks GetMasks();
				break;
			case 2: // public double GetRotation(string maskId, string units);
				break;
			}
			break;
		case 2: // Mask Class
			switch (nItem)
			{
			case 1: // public void MoveAbsolute(double x, double y, Unit unit = Unit.Pixels);
				break;
			case 2: // public void MoveRelative(double x, double y, Unit unit = Unit.Pixels);
				break;
			case 3: // public void Resize(double height, double width, Unit unit = Unit.Pixels);
				break;
			case 4: // public void Rotate(double angle, Unit units);
				break;
			case 5: // public Point2D Center;
				break;
			case 6: // public Point2D GetCenter(Unit units = Unit.Pixels)
				break;
			case 7: // public double Height;
				break;
			case 8: // public double GetHeight(Unit units = Unit.Pixels)
				break;
			case 9: // public double Width;
				break;
			case 10: // public double GetWidth(Unit units = Unit.Pixels)
				break;
			case 11: // public string Type;

				break;
			}
			break;
		case 3: // Masks Class
			switch (nItem)
			{
			case 1: // public void Clear(string maskType = null);
				break;
			case 2: // public void Delete(Mask mask);
				break;
			case 3: // public Mask GetMaskClosestTo(double x, double y, string maskType = null, Unit unit=Unit.Pixels);
				break;
			case 4: // public int GetNumMasks(string maskType = null);
				break;
			case 5: // public void Load(string fileName);
				break;
			case 6: // public void Save(string fileName);
				break;
			}
			break;
		}
		break;
	case 7: // FiducialsServiceClass
		switch (nGroup)
		{
		case 1: // Fiducial Class
			switch (nItem)
			{
			case 1: // public void MoveAbsolute(double x, double y, Unit unit = Unit.Pixels);
				break;
			case 2: // public void MoveRelative(double x, double y, Unit unit = Unit.Pixels);
				break;
			case 3: // public void Resize(double height, double width, Unit unit = Unit.Pixels);
				break;
			case 4: // public void Rotate(double angle, Unit units);
				break;
			case 5: // public Point2D Center;
				break;
			case 6: // public Point2D GetCenter(Unit unit = Unit.Pixels);
				break;
			case 7: // public double Height;
				break;
			case 8: // public double GetHeight(Unit unit = Unit.Pixels);
				break;
			case 9: // public double Width;
				break;
			case 10: // public double GetWidth(Unit unit = Unit.Pixels);
				break;
			case 11: // public string Type;
				break;
			}
			break;
		case 2: // Fiducials Class
			switch (nItem)
			{
			case 1: // public int GetNumSets()
				break;
			case 2: // public void AddSet()
				break;
			case 3: // public void Delete(Fiducial fiducial);
				break;
			case 4: // public void ClearSet(int fiducialSet=0);
				break;
			case 5: // public void DeleteSet(int fiducialSet=0);
				break;
			case 6: // public Fiducial GetFiducialClosestTo(double x, double y, int workingSet=0, Unit units = Unit.Pixels);
				break;
			case 7: // public int GetNumFiducials(int workingSet = null);
				break;
			case 8: // public void Load(string fileName);
				break;
			case 9: // public void Save(string fileName);
				break;
			}
			break;
		}
		break;
	case 8: // MotionService Class 
		switch (nGroup)
		{
		case 1: // Home Axes Methods
			switch (nItem)
			{
			case 1: // public string Home(AxisType[] axes, bool wait);
					//int nHomeAxis = GetPacketInt(packetData);
					//arraySize -= sizeof(int);
					//packetData = SplitPacket(packetData, sizeof(int), arraySize);
					//bool bHomeWait = GetPacketBool(packetData);
					//Motion_Home(nHomeAxis, bHomeWait); // 811
				break;
			case 2: // public bool IsHomed(AxisType axis);
					//int nIsHomeAxis = BitConverter.ToInt32(packetData, 0);
					//Motion_IsHomed(nIsHomeAxis); // 812
				break;
			}
			break;
		case 2: // Move Axes Methods
			switch (nItem)
			{
			case 1: // public string MoveX(double xPosition, Unit unit, bool wait);
				break;
			case 2: // public string MoveX(double xPosition, string unit, bool wait);
				break;
			case 3: // public string MoveY(double yPosition, Unit unit, bool wait);
				break;
			case 4: // public string MoveY(double yPosition, string unit, bool wait);
				break;
			case 5: // public string MoveZ(double zPosition, Unit unit, bool wait); // unit = Unit.MilliMeters
					//double dPosMoveZ = GetPacketDouble(packetData);
					//arraySize -= sizeof(double);
					//packetData = SplitPacket(packetData, sizeof(double), arraySize);
					//bool bMoveZWait = GetPacketBool(packetData);
					//sResult = Motion_MoveZ(dPosMoveZ, bMoveZWait); // 825
				packet >> sBuffer;
				SetReturn((CString)sBuffer.c_str());
				break;
			case 6: // public string MoveZ(double zPosition, string unit, bool wait);
				break;
			case 7: // public string MoveXY(double xPosition, double yPosition, Unit unit, bool wait);
				break;
			case 8: // public string MoveXY(double xPosition, double yPosition, string unit, bool wait);
				break;
			case 9: // public string MoveXYZ(double xPosition, double yPosition, double zPosition, Unit unit, bool wait);
				break;
			case 10: // public string MoveXYZ(double xPosition, double yPosition, double zPosition, string unit, bool wait);
				break;
			case 11: // public string MoveP(double pPosition, Unit unit, bool wait, bool isParcentric=false);
				break;
			case 12: // public string MoveP(double pPosition, string unit, bool wait, bool isParcentric=false);
				break;
			case 13: // public string MoveR(double rPosition, Unit unit, bool wait, bool isParcentric=false);
				break;
			case 14: // public string MoveR(double rPosition, string unit, bool wait, bool isParcentric=false);
				break;
			case 15: // public string MoveRP(double rPosition, double pPosition, Unit unit, bool wait, bool isParcentric=false);
				break;
			case 16: // public string MoveRP(double rPosition, double pPosition, string unit, bool wait, bool isParcentric=false);
				break;
			case 17: // public string MoveT(double tPosition, Unit unit, bool wait);
				break;
			case 18: // public string MoveT(double tPosition, string unit, bool wait);
				break;
			}
			break;
		case 3: // Retrieve Current Position Methods
			switch (nItem)
			{
			case 1: // public double GetXPos(Unit unit);
				break;
			case 2: // public double GetXPos(string unit);
				break;
			case 3: // public double GetYPos(Unit unit);
				break;
			case 4: // public double GetYPos(string unit);
				break;
			case 5: // public double GetZPos(Unit unit);
					//double dPosMoveZ = Motion_GetZPos(); // 835
				packet >> dReturn;
				sReturn.Format(_T("%0.6f"), dReturn);
				SetReturn(sReturn);
				break;
			case 6: // public double GetZPos(string unit);
				break;
			case 7: // public double GetPPos(Unit unit);
				break;
			case 8: // public double GetPPos(string unit);
				break;
			case 9: // public double GetRPos(Unit unit);
				break;
			case 10: // public double GetRPos(string unit);
				break;
			case 11: // public double GetTPos(Unit unit);
				break;
			case 12: // public double GetTPos(string unit);
				break;
			}
			break;
		case 4: // Wait On Axes Methods
			switch (nItem)
			{
			case 1: // public void Wait(AxisType[] axes, double timeout);
				break;
			case 2: // public void WaitForStageTaskComplete(string taskId, double timeout);
				break;
			}
			break;
		case 5: // Axis Availability Methods
			switch (nItem)
			{
			case 1: // public bool IsActive(AxisType axis);
					//int nAxisType = GetPacketInt(packetData);
					//bool bActive = Motion_IsActive(nAxisType);    // 851
				break;
			}
			break;
		case 6: // Pendant Status Method
			switch (nItem)
			{
			case 1: // public void SetPendantEnabled(bool enabled);
					//bool enabled = GetPacketBool(packetData);
					//Motion_SetPendantEnabled(enabled);    // 861
				break;
			}
			break;
		case 7: // Z-Stop Status Method
			switch (nItem)
			{
			case 1: // public bool IsZStopSet();
					//bool bStop = Motion_IsZStopSet();    // 871
				packet >> nByte;
				bReturn = nByte ? TRUE : FALSE;
				sReturn.Format(_T("%d"), bReturn ? 1 : 0);
				SetReturn(sReturn);
				break;
			}
			break;
		}
		break;
	case 9: // PatternService Class
		switch (nGroup)
		{
		case 1: // Save/Load Pattern Methods
			switch (nItem)
			{
			case 1: // public void Load(string fileName);
				break;
			case 2: // public void Save(string fileName);
				break;
			}
			break;
		case 2: // Run Pattern Methods
			switch (nItem)
			{
			case 1: // public void Align();
				break;
			case 2: // public void PreAlign();
				break;
			case 3: // public void Run();
				break;
			}
			break;
		}
		break;
	case 10: // RecipeService Class
		switch (nGroup)
		{
		case 1: // Save/Load Recipe Methods
			switch (nItem)
			{
			case 1: // public void Load(string fileName);
				break;
			case 2: // public void Save(string fileName);
				break;
			}
			break;
		case 2: // Run Recipe Method
			switch (nItem)
			{
			case 1: // public void Run();
				break;
			}
			break;
		}
		break;
	case 11: // SystemCommandsService Class
		switch (nGroup)
		{
		case 1: // Host Information Methods
			switch (nItem)
			{
			case 1: // public string GetComputerName();
				break;
			case 2: // public string GetOSName();
				break;
			case 3: // public long GetRamSize();
				break;
			}
			break;
		case 2: // Directory Information Methods
			switch (nItem)
			{
			case 1: // public string GetBinDir();
				break;
			case 2: // public string GetWorkingDir();
				break;
			case 3: // public string GetOpenDir(FileTypes fileType);
					// 1123
				break;
			case 4: // public void SetOpenDir(FileTypes fileType, string dirPath);
				break;
			case 5: // public string GetSaveDir(FileTypes fileType);
				break;
			case 6: // public void SetSaveDir(FileTypes fileType, string dirPath);
				break;
			}
			break;
		}
		break;
	case 12: // UserInterfaceService Class 
		switch (nGroup)
		{
		case 1: // Mx GUI Component
			switch (nItem)
			{
			case 1: // Not Exist
				break;
			}
			break;
		case 2: // Modal Dialogs
			switch (nItem)
			{
			case 1: // public bool ShowDialog(string text, DialogMode mode);
				break;
			case 2: // public string ShowInputDialog(string text, string defaultValue, Dialog-Mode mode, int maxLength);
				break;
			case 3: // public void ShowTimedDialog(string text, DialogMode mode, int seconds);
				break;
			}
			break;
		case 3: // Toolbar Click Methods
			switch (nItem)
			{
			case 1: // public void ClickToolbarButton(string[] buttonPath);
				break;
			}
			break;
		case 4: // Image Grid Method
			switch (nItem)
			{
			case 1: // public void SetImageGrid(Control control, string imagePath);
				break;
			}
			break;
		case 5: // Tab Class Method
			switch (nItem)
			{
			case 1: // public Tab GetTab(string name);
					//sName = GetPacketString(packetData);
					//sTab = UserInterface_GetTab(sName); // 1251
				break;
			case 2: // public List<Tab> GetTabs();
					//sTabs = UserInterface_GetTabs(); // 1252
				break;
			case 3: // public DockPanel GetDockPanel(string name);
				break;
			case 4: // public Group GetGroup(string name);
				break;
			case 5: // public void Show();
					//sTab = GetPacketString(packetData);
					//UserInterface_Tab_Show(sTab); // 1255
				break;
			case 6: // public string Name;
				break;
			case 7: // public Navigator Navigator;
				break;
			case 8: // public List<DockPanel> DockPanels;
					//sTab = GetPacketString(packetData);
					//sPanels = UserInterface_Tab_DockPanels(sTab); // 1258
				break;
			case 9: // public List<Group> Groups;
					//sTab = GetPacketString(packetData);
					//sGroups = UserInterface_Tab_Groups(sTab); // 1259
				break;
			}
			break;
		case 6: // Navigator Class
			switch (nItem)
			{
			case 1: // public void Pin(bool doPin);
				break;
			}
			break;
		case 7: // DockPanel Class
			switch (nItem)
			{
			case 1: // public void Pin(bool doPin);
				break;
			case 2: // public string Name;
				break;
			}
			break;
		case 8: // Group Class
			switch (nItem)
			{
			case 1: // public Container GetContainer(string containerName);
				break;
			case 2: // public string Name;
				break;
			case 3: // public List<Container> Containers;
					//sTab = GetPacketString(packetData);
					//arraySize -= (sTab.Length + 1);
					//packetData = SplitPacket(packetData, (sTab.Length + 1), arraySize);
					//sGroup = GetPacketString(packetData);
					//sGroups = UserInterface_Tab_Group_Containers(sTab, sGroup); // 1283
				break;
			}
			break;
		case 9: // Container Class
			switch (nItem)
			{
			case 1: // public void Show();
				break;
			case 2: // public string Name;
				break;
			case 3: // public List<Control> Controls;
					//sTab = GetPacketString(packetData);
					//arraySize -= (sTab.Length + 1);
					//packetData = SplitPacket(packetData, (sTab.Length + 1), arraySize);
					//sGroup = GetPacketString(packetData);
					//arraySize -= (sGroup.Length + 1);
					//packetData = SplitPacket(packetData, (sGroup.Length + 1), arraySize);
					//sContainer = GetPacketString(packetData);
					//sControls = UserInterface_Tab_Group_Container_Controls(sTab, sGroup, sContainer); // 1293
				break;
			}
			break;
		case 10: // ContainerWindow Class
			switch (nItem)
			{
			case 1: // public void Show();
				break;
			case 2: // public void Close();
				break;
			case 3: // public void ToFront();
				break;
			case 4: // public void ToBack();
				break;
			case 5: // public string Name;
				break;
			case 6: // public bool Open;
				break;
			case 7: // public List<Control> Controls;
				break;
			}
			break;
		case 11: // Window Class
			switch (nItem)
			{
			case 1: // public Window ShowMaskEditor();
				break;
			case 2: // public Window ShowFiducialEditor();
				break;
			case 3: // public void Close();
				break;
			case 4: // public void SaveData(string filePath);
				break;
			case 5: // public void SaveImage(string filePath);
				break;
			case 6: // public void ToBack();
				break;
			case 7: // public void ToFront();
				break;
			case 8: // public string Name;
				break;
			case 9: // public bool Open;
				break;
			case 10: // public List<Control> Controls;
				break;
			case 11: // public void SetChartLimits(string controlId, string axis, double low, double high, string unit);
				break;
			case 12: // public void SetChartLimits(string controlId, string axis, double low, double high, string unit);
				break;
			case 13: // public void ClearChartLimits(string controlId, string axis, string limit);
				break;
			}
			break;
		case 12: // Control Class
			switch (nItem)
			{
			case 1: // public Control GetControl(string[] path);
				break;
			case 2: // public void SaveData(string filePath, ISaveDataParameters saveDataParameters = null);
				break;
			case 3: // public void SaveImage(string filePath);
				break;
			case 4: // public string Name;
				break;
			case 5: // public List<Control> Controls;
				break;
			}
			break;
		case 13: // Saving Control Data
			switch (nItem)
			{
			case 1: // public bool SimpleMode;
				break;
			case 2: // public bool StandardFormat;
				break;
			case 3: // public string CodeVTitle;
				break;
			case 4: // public string CodeVType;
				break;
			case 5: // public string CodeVComment;
				break;
			case 6: // public string SdfManufacturer;
				break;
			case 7: // public DateTime SdfCreateDate;
				break;
			case 8: // public DateTime SdfModificationDate;
				break;
			case 9: // public double SdfWavelength;
				break;
			case 10: // public string SdfDataType;
				break;
			}
			break;
		case 14: // Print Methods
			switch (nItem)
			{
			case 1: // public void PrintMainWindow(int typeOfPrint, string path);
				break;
			}
			break;
		case 15: // Plot Methods
			switch (nItem)
			{
			case 1: // public void SetPlotPaletteScale(string controlId, string scaleMode, double peak, double valley, string unit);
				break;
			case 2: // public void SetPlotPaletteScale(string controlId, string scaleMode, double peak, double valley, string unit);
				break;
			case 3: // public void SetPlotPalette(string controlId, string paletteName);
				break;
			}
			break;
		case 16: // Sequence Method
			switch (nItem)
			{
			case 1: // public void SetSequenceStepState(string sequenceId, string sequenceStepDesc, bool onOff);
				break;
			}
			break;
		}
		break;
	}
}
