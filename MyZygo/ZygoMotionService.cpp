#include "stdafx.h"
#include "ZygoMxClient.h"
#include "ZygoMotionService.h"


CZygoMotionService::CZygoMotionService(void* pParent/*=NULL*/)
{
	m_pParent = pParent;
}


CZygoMotionService::~CZygoMotionService()
{
}

void CZygoMotionService::Home(int nZygoAxis, BOOL bWait) // 811
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _nZygoAxis = (INT32)nZygoAxis;
	byte _wait = bWait ? (byte)1 : (byte)0;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)1; packet << (byte)1;
	packet << _nZygoAxis; packet << _wait; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + sizeof(INT32);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

double CZygoMotionService::GetZPos() // 835
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)3; packet << (byte)5;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32);
	double dPosZ = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dPosZ;
}

CString CZygoMotionService::MoveZ(double dPos) // 825
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)2; packet << (byte)5;
	packet << dPos; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(double);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

BOOL CZygoMotionService::IsZStopSet() // 871
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)7; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	BOOL bRtn = pMxClient->GetReturnBool(pMxClient->Send((char*)packet.GetData(), nLen));
	return bRtn;
}


BOOL CZygoMotionService::IsHomed(int nAxisType) // 812
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _nAxisType = (INT32)nAxisType;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)1; packet << (byte)2;
	packet << _nAxisType; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32);
	BOOL bRtn = pMxClient->GetReturnBool(pMxClient->Send((char*)packet.GetData(), nLen));
	return bRtn;
}

CString CZygoMotionService::MoveX(double dPosMove, BOOL bMoveWait) // 821, 822
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	DOUBLE _dPosMove = (DOUBLE)dPosMove;
	byte _wait = bMoveWait ? (byte)1 : (byte)0;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)2; packet << (byte)1;
	packet << _dPosMove; packet << _wait; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + sizeof(DOUBLE);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

CString CZygoMotionService::MoveY(double dPosMove, BOOL bMoveWait) // 823, 824
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	DOUBLE _dPosMove = (DOUBLE)dPosMove;
	byte _wait = bMoveWait ? (byte)1 : (byte)0;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)2; packet << (byte)3;
	packet << _dPosMove; packet << _wait; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + sizeof(DOUBLE);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

CString CZygoMotionService::MoveXY(double dPosX, double dPosY, BOOL bMoveWait) // 827, 828
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	DOUBLE _dPosX = (DOUBLE)dPosX;
	DOUBLE _dPosY = (DOUBLE)dPosY;
	byte _wait = bMoveWait ? (byte)1 : (byte)0;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)2; packet << (byte)7;
	packet << _dPosX; packet << _dPosY; packet << _wait; 
	packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 2 * sizeof(DOUBLE);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

CString CZygoMotionService::MoveXYZ(double dPosX, double dPosY, double dPosZ, BOOL bMoveWait) // 829, 8210
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	DOUBLE _dPosX = (DOUBLE)dPosX;
	DOUBLE _dPosY = (DOUBLE)dPosY;
	DOUBLE _dPosZ = (DOUBLE)dPosZ;
	byte _wait = bMoveWait ? (byte)1 : (byte)0;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)2; packet << (byte)9;
	packet << _dPosX; packet << _dPosY; packet << _dPosZ; packet << _wait;
	packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 3 * sizeof(DOUBLE);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

CString CZygoMotionService::MoveP(double dPos, BOOL bMoveWait, BOOL isParcentric) // 8211, 8112
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	DOUBLE _dPos = (DOUBLE)dPos;
	byte _wait = bMoveWait ? (byte)1 : (byte)0;
	byte _isParcentric = isParcentric ? (byte)1 : (byte)0;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)2; packet << (byte)11;
	packet << _dPos; packet << _wait; packet << _isParcentric;
	packet << (byte)0; // NULL(for end)
	int nLen = 6 * sizeof(byte) + sizeof(DOUBLE);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

CString CZygoMotionService::MoveR(double dPos, BOOL bMoveWait, BOOL isParcentric) // 8213, 8114
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	DOUBLE _dPos = (DOUBLE)dPos;
	byte _wait = bMoveWait ? (byte)1 : (byte)0;
	byte _isParcentric = isParcentric ? (byte)1 : (byte)0;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)2; packet << (byte)13;
	packet << _dPos; packet << _wait; packet << _isParcentric;
	packet << (byte)0; // NULL(for end)
	int nLen = 6 * sizeof(byte) + sizeof(DOUBLE);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

CString CZygoMotionService::MoveRP(double dPosR, double dPosP, BOOL bMoveWait, BOOL isParcentric) // 8215, 8116
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	DOUBLE _dPosR = (DOUBLE)dPosR;
	DOUBLE _dPosP = (DOUBLE)dPosP;
	byte _wait = bMoveWait ? (byte)1 : (byte)0;
	byte _isParcentric = isParcentric ? (byte)1 : (byte)0;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)2; packet << (byte)15;
	packet << _dPosR; packet << _dPosP; packet << _wait; packet << _isParcentric;
	packet << (byte)0; // NULL(for end)
	int nLen = 6 * sizeof(byte) + 2 * sizeof(DOUBLE);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

CString CZygoMotionService::MoveT(double dPos, BOOL bMoveWait) // 8217, 8119
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	DOUBLE _dPos = (DOUBLE)dPos;
	byte _wait = bMoveWait ? (byte)1 : (byte)0;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)2; packet << (byte)17;
	packet << _dPos; packet << _wait;
	packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + sizeof(DOUBLE);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

double CZygoMotionService::GetXPos() // 831, 832
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)3; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32);
	double dPos = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dPos;
}

double CZygoMotionService::GetYPos() // 833, 834
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)3; packet << (byte)3;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32);
	double dPos = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dPos;
}

double CZygoMotionService::GetPPos() // 837, 838
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)3; packet << (byte)7;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32);
	double dPos = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dPos;
}

double CZygoMotionService::GetRPos() // 839, 8310
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)3; packet << (byte)9;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32);
	double dPos = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dPos;
}

double CZygoMotionService::GetTPos() // 8311, 8312
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)3; packet << (byte)11;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32);
	double dPos = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dPos;
}

void CZygoMotionService::WaitForStageTaskComplete(CString taskId, double timeout) // 842
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = taskId.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(taskId, pData);
	DOUBLE _timeout = (DOUBLE)timeout;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)4; packet << (byte)2;
	packet << pData; packet << _timeout;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1) + sizeof(DOUBLE);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

BOOL CZygoMotionService::IsActive(int nAxisType) // 851
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _nAxisType = (INT32)nAxisType;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)5; packet << (byte)1;
	packet << _nAxisType; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32);
	BOOL bRtn = pMxClient->GetReturnBool(pMxClient->Send((char*)packet.GetData(), nLen));
	return bRtn;
}

void CZygoMotionService::SetPendantEnabled(BOOL enable) // 861
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	byte _enable = enable ? (byte)1 : (byte)0;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)6; packet << (byte)1;
	packet << _enable; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

