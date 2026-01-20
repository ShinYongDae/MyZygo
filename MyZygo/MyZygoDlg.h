
// MyZygoDlg.h : 헤더 파일
//

#pragma once
#include "Zygo.h"


// CMyZygoDlg 대화 상자
class CMyZygoDlg : public CDialog
{
	CZygo* m_pZygo;

	void InitComboTurret();

// 생성입니다.
public:
	CMyZygoDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	~CMyZygoDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYZYGO_DIALOG };
#endif

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
};
