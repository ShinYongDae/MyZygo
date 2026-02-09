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

	//ClearReturn();//m_sReturn = _T("");

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
	//m_sReturn += sReceived;
	//CString sDisp;
	//GetDlgItem(IDC_MESSAGE_LIST)->GetWindowText(sDisp);
	//sDisp += sReceived + _T("\r\n");
	//GetDlgItem(IDC_MESSAGE_LIST)->SetWindowText(sDisp);

	return (LRESULT)1;
}

//void CZygo::OnDataReceived(BYTE* pBuffer, DWORD dwSize)
//{
//	CPacket packet;
//	packet.Append(pBuffer, dwSize);
//	PacketParsing(packet, dwSize);
//}

BOOL CZygo::Send(char* pBuffer)
{
	BOOL bOk = FALSE;
	if (m_pClient)
	{
		bOk = m_pClient->Send(pBuffer);
		if (bOk)
			m_bIsWait = TRUE;
		return bOk;
	}
	return FALSE;
}

BOOL CZygo::Send(char* pBuffer, int nLen)
{
	BOOL bOk = FALSE;
	if (m_pClient)
	{
		bOk = m_pClient->Send(pBuffer, nLen);
		if (bOk)
			m_bIsWait = TRUE;
		return bOk;
	}
	return FALSE;
}

BOOL CZygo::Start(CString sSvrAddr, CString sSvrPort)
{
	if (!m_pClient)
		m_pClient = new CZygoMxClient(sSvrAddr, _tstoi(sSvrPort), this);

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

BOOL CZygo::IsConnected()
{
	if (m_pClient)
	{
		return m_pClient->IsConnected();
	}
	return FALSE;
}

BOOL CZygo::IsConnectedMainUI() // 122
{
	BOOL bRtn = FALSE;
	if (IsConnected())
	{
		bRtn = m_pClient->ZygoConnected(); // 122
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected.\r\n"));
	}
	return bRtn;
}

double CZygo::GetLightLevel()
{
	double dRtn = 0.0;
	if (IsConnected())
	{
		dRtn = m_pClient->Instrument->GetLightLevel(); // 581
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected.\r\n"));
	}
	return dRtn;
}

CString CZygo::Measure(BOOL bWait) // 532
{
	CString sReturn = _T("");
	if (IsConnected())
	{
		sReturn = m_pClient->Instrument->Measure(bWait); // 532
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected."));
	}
	return sReturn;
}

void CZygo::AutoFocus() // 551
{
	if (IsConnected())
	{
		m_pClient->Instrument->AutoFocus(); // 551
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected."));
	}
}

void CZygo::AutoLightLevel() // 554
{
	if (IsConnected())
	{
		m_pClient->Instrument->AutoLightLevel(); // 554
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected."));
	}
}

int CZygo::GetTurret() // 561
{
	int nTurret = -1;
	if (IsConnected())
	{
		nTurret = m_pClient->Instrument->GetTurret(); // 561
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected.\r\n"));
	}

	return nTurret;
}

void CZygo::MoveTurret(int nTurret) // 562
{
	//서버 정상 여부 확인
	if (IsConnected())
	{
		m_pClient->Instrument->MoveTurret(nTurret); // 562
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected.\r\n"));
	}
}

double CZygo::GetZoom() // 571
{
	double dZoom = 0.0;
	if (IsConnected())
	{
		dZoom = m_pClient->Instrument->GetZoom(); // 571
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected."));
	}
	return dZoom;
}

void CZygo::SetZoom(double dZoom) // 572
{
	if (IsConnected())
	{
		m_pClient->Instrument->SetZoom(dZoom); // 572
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected."));
	}
}

void CZygo::SetLightLevel(double dlightLevel) // 582
{
	if (IsConnected())
	{
		m_pClient->Instrument->SetLightLevel(dlightLevel); // 582
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected."));
	}
}

void CZygo::HomeX(BOOL bWait) // 811 
{
	if (IsConnected())
	{
		m_pClient->Motion->Home(1, bWait); // 811
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected."));
	}
}

void CZygo::HomeY(BOOL bWait) // 811 
{
	if (IsConnected())
	{
		m_pClient->Motion->Home(2, bWait); // 811
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected."));
	}
}

void CZygo::HomeZ(BOOL bWait) // 811 
{
	if (IsConnected())
	{
		m_pClient->Motion->Home(3, bWait); // 811
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected."));
	}
}

double CZygo::GetZPos() // 835
{
	double dPosZ = 0.0;
	if (IsConnected())
	{
		dPosZ = m_pClient->Motion->GetZPos(); // 811
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected."));
	}
	return dPosZ;
}

CString CZygo::MoveZ(double dPos, BOOL bAbs) // 825
{
	CString sReturn = _T("");
	double dPosZ = 0.0;
	if (IsConnected())
	{
		if (bAbs)
		{
			sReturn = m_pClient->Motion->MoveZ(dPos); // 825
		}
		else
		{
			double dPosZcurr = m_pClient->Motion->GetZPos(); // 811
			double dPosZdest = dPosZcurr + dPos;
			sReturn = m_pClient->Motion->MoveZ(dPosZdest); // 825
		}
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected."));
	}
	return sReturn;
}

BOOL CZygo::IsZStopSet()    // 871
{
	BOOL bStop = FALSE;
	if (IsConnected())
	{
		bStop = m_pClient->Motion->IsZStopSet(); // 871
	}
	else
	{
		AfxMessageBox(_T("Zygo not Connected."));
	}
	return bStop;
}

// for Zygo Connection ..................................................
//void CZygo::SetReturn(CString str)
//{
//	m_sReturn = str;
//	m_pClient->SetReturn(str);
//}
//
//void CZygo::ClearReturn()
//{
//	m_sReturn = _T("");
//}
//
//BOOL CZygo::IsReturn()
//{
//	if (m_sReturn.GetLength() > 0)
//		return TRUE;
//	return FALSE;
//}
