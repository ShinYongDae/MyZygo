#pragma once
class CZygoMotionService
{
	void* m_pParent;

public:
	CZygoMotionService(void* pParent = NULL);
	~CZygoMotionService();

public:
	void Home(int nZygoAxis, BOOL bWait); // 811
	double GetZPos(); // 835
	CString MoveZ(double dPos); // 825
	BOOL IsZStopSet(); // 871
};

