// ZygoClient.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ZygoClient.h"
#include "Zygo.h"

// CZygoClient

//IMPLEMENT_DYNAMIC(CZygoClient, CWnd)

CZygoClient::CZygoClient(CString sServerIp, int nPort, CWnd* pParent/*=NULL*/)
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
	//CreateWndForm(WS_CHILD | WS_OVERLAPPED);

	ThreadStart();
}

CZygoClient::~CZygoClient()
{
	//소켓 종료
	closesocket(clientSocket);

	ThreadStop();
	Sleep(30);
	t1.join();

	WSACleanup();
	delete m_pReadBuffer;
}


//BEGIN_MESSAGE_MAP(CZygoClient, CWnd)
//END_MESSAGE_MAP()



// CZygoClient 메시지 처리기입니다.
//BOOL CZygoClient::CreateWndForm(DWORD dwStyle)
//{
//	if (!Create(NULL, _T("ZygoClient"), dwStyle, CRect(0, 0, 0, 0), m_pParent, (UINT)this))
//	{
//		AfxMessageBox(_T("CZygoClient::Create() Failed!!!"));
//		return FALSE;
//	}
//
//	return TRUE;
//}

BOOL CZygoClient::IsConnected()
{
	return (m_bThreadAlive && m_bConnected);
}

BOOL CZygoClient::Send(CString sSend)
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

BOOL CZygoClient::Send(char* pBuffer)
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

BOOL CZygoClient::Send(char* pBuffer, int nLen)
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


void CZygoClient::ProcThrd(const LPVOID lpContext)
{
	CZygoClient* pZygoClient = reinterpret_cast<CZygoClient*>(lpContext);

	while (pZygoClient->ThreadIsAlive())
	{
		if (!pZygoClient->ProcReceive())
			break;
	}

	pZygoClient->ThreadEnd();
}

BOOL CZygoClient::ProcReceive()
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
			((CZygo*)m_pParent)->OnDataReceived((BYTE*)buffer, nRecSize);
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

void CZygoClient::ThreadStart()
{
	m_bThreadStateEnd = FALSE;
	m_bThreadAlive = TRUE;
	t1 = std::thread(ProcThrd, this);
}

void CZygoClient::ThreadStop()
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

void CZygoClient::ThreadEnd()
{
	m_bThreadStateEnd = TRUE;
}

BOOL CZygoClient::ThreadIsAlive()
{
	return m_bThreadAlive;
}

void CZygoClient::ClearReadBuffer()
{
	ZeroMemory(m_pReadBuffer, sizeof(m_pReadBuffer));
}

void CZygoClient::StringToChar(CString str, char* szStr)  // char* returned must be deleted... 
{
	int nLen = str.GetLength();
	strcpy(szStr, CT2A(str));
	szStr[nLen] = _T('\0');
}

void CZygoClient::StringToTChar(CString str, TCHAR* tszStr) // TCHAR* returned must be deleted... 
{
	int nLen = str.GetLength() + 1;
	memset(tszStr, 0x00, nLen * sizeof(TCHAR));
	_tcscpy(tszStr, str);
}

CString CZygoClient::CharToString(char *szStr)
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


