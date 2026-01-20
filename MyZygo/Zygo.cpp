// Zygo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MyZygo.h"
#include "Zygo.h"


// CZygo

IMPLEMENT_DYNAMIC(CZygo, CWnd)

CZygo::CZygo(CWnd* pParent, CString sSvrAddr, CString sSvrPort)
{
	m_hParent = NULL;
	m_pParent = pParent;
	m_pClient = NULL;
	m_bIsWait = FALSE;

	if (pParent)
		m_hParent = pParent->GetSafeHwnd();
	RECT rt = { 0,0,0,0 };
	Create(NULL, _T("None"), WS_CHILD, rt, FromHandle(m_hParent), (UINT)this);

	Start(sSvrAddr, sSvrPort);
}

CZygo::~CZygo()
{
	End();
}


BEGIN_MESSAGE_MAP(CZygo, CWnd)
	ON_MESSAGE(WM_CLIENT_RECEIVED, wmClientReceived)
END_MESSAGE_MAP()



// CZygo 메시지 처리기입니다.

LRESULT CZygo::wmClientReceived(WPARAM wParam, LPARAM lParam)
{
	CString sReceived = (LPCTSTR)lParam;

	//CString sDisp;
	//GetDlgItem(IDC_MESSAGE_LIST)->GetWindowText(sDisp);
	//sDisp += sReceived + _T("\r\n");
	//GetDlgItem(IDC_MESSAGE_LIST)->SetWindowText(sDisp);

	return (LRESULT)1;
}

BOOL CZygo::Start(CString sSvrAddr, CString sSvrPort)
{
	if (!m_pClient)
		m_pClient = new CSimpleClient(sSvrAddr, _tstoi(sSvrPort), this);

	return TRUE;
}

BOOL CZygo::End()
{
	if (m_pClient)
	{
		delete m_pClient;
		m_pClient = NULL;
	}

	return TRUE;
}

BOOL CZygo::Send(char* pBuffer)
{
	BOOL bOk = FALSE;
	if (m_pClient)
	{
		bOk = m_pClient->Send(pBuffer);
		if(bOk)
			m_bIsWait = TRUE;
		return bOk;
	}
	return FALSE;
}

BOOL CZygo::IsConnected()
{
	if (m_pClient)
		return m_pClient->IsConnected();
	return FALSE;
}


void CZygo::ZygoSelectTurret(INT32 nTurret) // 562
{
	//서버 정상 여부 확인
	if (IsConnected())
	{
		CPacket packet;
		packet << (byte)5; packet << (byte)6; packet << (byte)2;		
		packet << nTurret;
		Send((char*)packet.GetData()); // , packet.GetDataSize()
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected Yet.\r\n"));
	}
}