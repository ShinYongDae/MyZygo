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
	pMxClient->ClearReturn();
	INT32 _nZygoAxis = (INT32)nZygoAxis;
	byte _wait = bWait ? (byte)1 : (byte)0;
	CPacket packet;
	packet << (byte)8; packet << (byte)1; packet << (byte)1;
	packet << _nZygoAxis; packet << _wait; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 1 * sizeof(INT32);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

double CZygoMotionService::GetZPos() // 835
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)8; packet << (byte)3; packet << (byte)5;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + 1 * sizeof(INT32);
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
	int nLen = 4 * sizeof(byte) + 1 * sizeof(double);
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

