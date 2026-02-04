#pragma once
class CZygoMotionService
{
	void* m_pParent;

public:
	CZygoMotionService(void* pParent = NULL);
	~CZygoMotionService();

public:
	void Home(int nZygoAxis, BOOL bWait); // 811
	double GetZPos(); // 835, 836
	CString MoveZ(double dPos); // 825, 826
	BOOL IsZStopSet(); // 871

public:
	BOOL IsHomed(int nAxisType); // 812
	CString MoveX(double dPosMove, BOOL bMoveWait); // 821, 822
	CString MoveY(double dPosMove, BOOL bMoveWait); // 823, 824
	CString MoveXY(double dPosX, double dPosY, BOOL bMoveWait); // 827, 828
	CString MoveXYZ(double dPosX, double dPosY, double dPosZ, BOOL bMoveWait); // 829, 8210
	CString MoveP(double dPos, BOOL bMoveWait, BOOL isParcentric = false); // 8211, 8112
	CString MoveR(double dPos, BOOL bMoveWait, BOOL isParcentric = false); // 8213, 8114
	CString MoveRP(double dPosR, double dPosP, BOOL bMoveWait, BOOL isParcentric = false); // 8215, 8116
	CString MoveT(double dPos, BOOL bMoveWait); // 8217, 8119
	double GetXPos(); // 831, 832
	double GetYPos(); // 833, 834
	double GetPPos(); // 837, 838
	double GetRPos(); // 839, 8310
	double GetTPos(); // 8311, 8312
	void WaitForStageTaskComplete(CString taskId, double timeout); // 842
	BOOL IsActive(int nAxisType); // 851
	void SetPendantEnabled(BOOL enable); // 861
};

