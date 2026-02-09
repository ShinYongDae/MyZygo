
// MyZygoDlg.h : 헤더 파일
//

#pragma once
#include "Zygo.h"

#define CHECK_DELAY_ZYGO_STATUS		500

// CMyZygoDlg 대화 상자
class CMyZygoDlg : public CDialog
{
	BOOL m_bThreadAlive;// , m_bThreadStateEnd;
	std::thread t0, t1;
	void ThreadStart();
	void ThreadStop();

	CZygo* m_pZygo;
	BOOL m_bChkConnect;
	BOOL m_bDlg;
	CString m_sLightDN;

	void InitDlg();
	void InitComboTurret();
	CWnd* GetWndItem(int nId);
	CString GetErrorDescription(CException *ex);
	CString GetDetailErrorMessage(DWORD dwErrorCode, CString strErrorDescription, CString &strFile, DWORD &dwLineNo);
	void DetailErrorMessageBox(CException *ex, CString strFile, DWORD dwLineNo, UINT uMB_IconButton= MB_OK);

	CString m_sZygoAddr, m_sZygoPort;
	double m_dZHomePos, m_dZStopPos, m_dZMesurePos;
	void LoadConfig();
	void CheckZygoStatus();

	void DispZoom(double dZoom);

// 생성입니다.
public:
	CMyZygoDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	~CMyZygoDlg();
	static void ProcThrd0(const LPVOID lpContext);
	static void ProcThrd1(const LPVOID lpContext);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYZYGO_DIALOG };
#endif


protected:
	BOOL Proc0();
	BOOL Proc1();
	BOOL ThreadIsAlive();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSelchangeComboZygoLensTurret();
	afx_msg void OnBnClickedButtonAf();
	afx_msg void OnNcDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButtonManualChange();
	afx_msg void OnBnClickedButtonAutolight();
	afx_msg void OnBnClickedButtonStartMeasure();
	afx_msg void OnBnClickedButtonHomming();
	afx_msg void OnBnClickedButtonGoPos();
	afx_msg void OnBnClickedCheckHalfx();
	afx_msg void OnBnClickedCheck1x();
	afx_msg void OnBnClickedCheck2x();
};
