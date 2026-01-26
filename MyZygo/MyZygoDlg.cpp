
// MyZygoDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MyZygo.h"
#include "MyZygoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyZygoDlg 대화 상자

CString CMyZygoDlg::GetErrorDescription(CException *ex)
{
	CString strExceptionDescription;
	TCHAR szException[1024] = { 0, };

	if (ex->GetErrorMessage(szException, 1024))
		strExceptionDescription = szException;

	return strExceptionDescription;
}

CString CMyZygoDlg::GetDetailErrorMessage(DWORD dwErrorCode, CString strErrorDescription, CString &strFile, DWORD &dwLineNo)
{
	CString strErrorMessage;
	COleDateTime datetime = COleDateTime::GetCurrentTime();

	strErrorMessage.Format(
		_T("Date Time: %s\n")
		_T("ErrorCode: %d\n\n")
		_T("%s\n\n")
		_T("File: %s\n")
		_T("Line: %d"),
		datetime.Format(),
		dwErrorCode,
		strErrorDescription,
		strFile,
		dwLineNo);

	return strErrorMessage;
}

void CMyZygoDlg::DetailErrorMessageBox(CException *ex, CString strFile, DWORD dwLineNo, UINT uMB_IconButton /*= MB_OK*/)
{
	CString strErrorDescription = GetErrorDescription(ex);
	CString strDetailErrorMessage = GetDetailErrorMessage(GetLastError(), strErrorDescription, strFile, dwLineNo);

	::AfxMessageBox(strDetailErrorMessage, uMB_IconButton);
}

CMyZygoDlg::CMyZygoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MYZYGO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pZygo = NULL;
	m_bChkConnect = FALSE;
	m_bDlg = FALSE;
	m_sLightDN = _T("70.0");

	m_sZygoAddr = _T("");
	m_sZygoPort = _T("");

	m_dZHomePos = 0.0;
	m_dZStopPos = 0.0;
	m_dZMesurePos = 0.0;

	ThreadStart();
}

CMyZygoDlg::~CMyZygoDlg()
{
	m_bChkConnect = FALSE;

	ThreadStop();
	t0.join();
	t1.join();

	if (m_pZygo)
	{
		delete m_pZygo;
		m_pZygo = NULL;
	}
}

void CMyZygoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyZygoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_ZYGO_LENS_TURRET, &CMyZygoDlg::OnSelchangeComboZygoLensTurret)
	ON_BN_CLICKED(IDC_BUTTON_AF, &CMyZygoDlg::OnBnClickedButtonAf)
	ON_WM_NCDESTROY()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_HALFX, &CMyZygoDlg::OnBnClickedButtonHalfx)
	ON_BN_CLICKED(IDC_BUTTON_1X, &CMyZygoDlg::OnBnClickedButton1x)
	ON_BN_CLICKED(IDC_BUTTON_2X, &CMyZygoDlg::OnBnClickedButton2x)
	ON_BN_CLICKED(IDC_BUTTON_MANUAL_CHANGE, &CMyZygoDlg::OnBnClickedButtonManualChange)
	ON_BN_CLICKED(IDC_BUTTON_AUTOLIGHT, &CMyZygoDlg::OnBnClickedButtonAutolight)
	ON_BN_CLICKED(IDC_BUTTON_START_MEASURE, &CMyZygoDlg::OnBnClickedButtonStartMeasure)
	ON_BN_CLICKED(IDC_BUTTON_HOMMING, &CMyZygoDlg::OnBnClickedButtonHomming)
	ON_BN_CLICKED(IDC_BUTTON_GO_POS, &CMyZygoDlg::OnBnClickedButtonGoPos)
END_MESSAGE_MAP()


// CMyZygoDlg 메시지 처리기

void CMyZygoDlg::ProcThrd0(const LPVOID lpContext)
{
	CMyZygoDlg* pMyZygoDlg = reinterpret_cast<CMyZygoDlg*>(lpContext);

	while (pMyZygoDlg->ThreadIsAlive())
	{
		if (!pMyZygoDlg->Proc0())
			break;
	}
}

BOOL CMyZygoDlg::Proc0()
{
	if (m_bChkConnect)
	{
		try
		{
			if (m_bDlg)
				CheckZygoStatus();
			if (m_bDlg)
				Sleep(CHECK_DELAY_ZYGO_STATUS); //CheckZygoStatus(); 의 주기를 설정함.
		}
		catch (CException *ex)
		{
			DetailErrorMessageBox(ex, _T(__FILE__), __LINE__, MB_ICONERROR | MB_OK);
		}
	}
	else
		Sleep(100);
	return TRUE;
}

void CMyZygoDlg::ProcThrd1(const LPVOID lpContext)
{
	CMyZygoDlg* pMyZygoDlg = reinterpret_cast<CMyZygoDlg*>(lpContext);

	while (pMyZygoDlg->ThreadIsAlive())
	{
		if (!pMyZygoDlg->Proc1())
			break;
	}
}

BOOL CMyZygoDlg::Proc1()
{
	Sleep(100);
	return TRUE;
}

void CMyZygoDlg::ThreadStart()
{
	m_bThreadAlive = TRUE;
	t0 = std::thread(ProcThrd0, this);
	t1 = std::thread(ProcThrd1, this);
}

void CMyZygoDlg::ThreadStop()
{
	m_bThreadAlive = FALSE;
	MSG message;
	const DWORD dwTimeOut = 1000 * 60 * 3; // 3 Minute
	DWORD dwStartTick = GetTickCount();
	Sleep(50);
}

BOOL CMyZygoDlg::ThreadIsAlive()
{
	return m_bThreadAlive;
}

CWnd* CMyZygoDlg::GetWndItem(int nId)
{
	CWnd* pWnd = NULL;
	if (!m_bDlg)
		return pWnd;
	else
		pWnd = (m_bDlg ? GetDlgItem(nId) : NULL);
	return (m_bDlg ? pWnd : NULL);
}

BOOL CMyZygoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	LoadConfig();

	if (!m_pZygo)
	{
		if (m_sZygoAddr.IsEmpty() || m_sZygoPort.IsEmpty())
			m_pZygo = new CZygo(this);
		else
			m_pZygo = new CZygo(this, m_sZygoAddr, m_sZygoPort);
	}

	InitDlg();

	m_bChkConnect = TRUE;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMyZygoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMyZygoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMyZygoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CMyZygoDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_bDlg = TRUE;

	return 0;
}

void CMyZygoDlg::OnNcDestroy()
{
	CDialog::OnNcDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_bDlg = FALSE;
	Sleep(CHECK_DELAY_ZYGO_STATUS+100);
}

void CMyZygoDlg::LoadConfig()
{
	CString sPath = _T("../Config/Init.ini");
	TCHAR szData[MAX_PATH];
	if (0 < ::GetPrivateProfileString(_T("TcpIp"), _T("Address"), NULL, szData, sizeof(szData), sPath))
		m_sZygoAddr = CString(szData);
	if (0 < ::GetPrivateProfileString(_T("TcpIp"), _T("Port"), NULL, szData, sizeof(szData), sPath))
		m_sZygoPort = CString(szData);
}

void CMyZygoDlg::InitDlg()
{
	GetDlgItem(IDC_EDIT_LIGHT_LEVEL)->SetWindowText(m_sLightDN);
	InitComboTurret();
}

void CMyZygoDlg::InitComboTurret()
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_ZYGO_LENS_TURRET);
	pCombo->InsertString(0, _T("20X"));
	pCombo->InsertString(1, _T("UnUsed1"));
	pCombo->InsertString(2, _T("50X"));
	pCombo->InsertString(3, _T("UnUsed2"));
	pCombo->SetCurSel(-1); // Select None 
}

void CMyZygoDlg::CheckZygoStatus()
{
	BOOL bConnectedServer = FALSE, bConnectedZygo = FALSE;
	CString sRtn = _T("");
	CButton* pChk0 = (CButton*)GetWndItem(IDC_CHECK_SERVER_CONNECT_STATE);
	CButton* pChk1 = (CButton*)GetWndItem(IDC_CHECK_ZYGO_CONNECT_STATE);
	CStatic* pText0 = (CStatic*)GetWndItem(IDC_TEXT_LIGHT_LEVEL);
	CStatic* pText1 = (CStatic*)GetWndItem(IDC_TEXT_HOME_POS);
	CStatic* pText2 = (CStatic*)GetWndItem(IDC_TEXT_Z_STOP_POS);
	CStatic* pText3 = (CStatic*)GetWndItem(IDC_TEXT_MEASURE_POS);

	if (m_pZygo)
	{
		if (pChk0)
		{
			if (m_bDlg && (bConnectedServer = m_pZygo->IsConnected()))
				if (m_bDlg) pChk0->SetCheck(TRUE);
			else if (m_bDlg) pChk0->SetCheck(FALSE);
				
		}

		if (pChk1)
		{
			if (m_bDlg && (bConnectedZygo = m_pZygo->IsConnectedMainUI())) // 122
				if (m_bDlg) pChk1->SetCheck(TRUE);
			else if (m_bDlg) pChk1->SetCheck(FALSE);
				
		}

		if (pText0)
		{
			if (m_bDlg && bConnectedZygo)
			{
				double dVal = m_pZygo->GetLightLevel(); // 581
				sRtn.Format(_T("%f", dVal));
				if (m_bDlg) pText0->SetWindowText(sRtn);
			}
			else if (m_bDlg) pText0->SetWindowTextW(_T(""));
		}

		if (pText1)
		{
			if (m_bDlg && bConnectedZygo)
			{
				double dVal = m_dZHomePos; // 835
				sRtn.Format(_T("%f", dVal));
				if (m_bDlg) pText1->SetWindowText(sRtn);
			}
			else if (m_bDlg) pText1->SetWindowTextW(_T(""));
		}

		if (pText2)
		{
			if (m_bDlg && bConnectedZygo)
			{
				double dVal = m_dZStopPos; // 835
				sRtn.Format(_T("%f", dVal));
				if (m_bDlg) pText2->SetWindowText(sRtn);
			}
			else if (m_bDlg) pText2->SetWindowTextW(_T(""));
		}

		if (pText3)
		{
			if (m_bDlg && bConnectedZygo)
			{
				double dVal = m_dZMesurePos; // 835
				sRtn.Format(_T("%f", dVal));
				if (m_bDlg) pText3->SetWindowText(sRtn);
			}
			else if (m_bDlg) pText3->SetWindowTextW(_T(""));
		}
	}
}

void CMyZygoDlg::OnSelchangeComboZygoLensTurret()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_ZYGO_LENS_TURRET);

	int nTypeTurret = pCombo->GetCurSel();
	if (m_pZygo)
		m_pZygo->MoveTurret(nTypeTurret); // 562
}

void CMyZygoDlg::OnBnClickedButtonHalfx()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pZygo->SetZoom(0.5); // 572
}

void CMyZygoDlg::OnBnClickedButton1x()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pZygo->SetZoom(1.0); // 572
}

void CMyZygoDlg::OnBnClickedButton2x()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pZygo->SetZoom(2.0); // 572
}

void CMyZygoDlg::OnBnClickedButtonManualChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strLightDN;
	GetDlgItem(IDC_EDIT_LIGHT_LEVEL)->GetWindowText(strLightDN);
	double dLightDN = _ttof(strLightDN);
	m_pZygo->SetLightLevel(dLightDN);
}

void CMyZygoDlg::OnBnClickedButtonAutolight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pZygo->AutoLightLevel();
}

void CMyZygoDlg::OnBnClickedButtonStartMeasure()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pZygo->IsZStopSet())
	{
		CString sRtn = m_pZygo->Measure(TRUE); // 532
		if (sRtn.IsEmpty())
		{
			AfxMessageBox(_T("Measure Failed."));
		}
		else
		{
			AfxMessageBox(_T("Measure Completed."));
		}
		//m_dZMesurePos = m_pZygo->GetZPos(); // 835
		//m_pZygo->AutoFocus(); // 551
	}
}

void CMyZygoDlg::OnBnClickedButtonHomming()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pZygo->HomeZ(TRUE); // 811 
	m_dZHomePos = m_dZStopPos = m_pZygo->GetZPos(); // 835
}

void CMyZygoDlg::OnBnClickedButtonAf()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_dZStopPos = m_pZygo->GetZPos(); // 835
}

void CMyZygoDlg::OnBnClickedButtonGoPos()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bABS = ((CButton*)GetDlgItem(IDC_CHECK_MOVING_ABS))->GetCheck();
	CString sPos;
	GetDlgItem(IDC_EDIT_MOVE_POS)->GetWindowText(sPos);
	double dPos = _ttof(sPos);
	CString sReturn = m_pZygo->MoveZ(dPos, bABS); // 825
	m_dZStopPos = m_pZygo->GetZPos(); // 835
}
