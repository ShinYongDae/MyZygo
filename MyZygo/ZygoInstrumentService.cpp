#include "stdafx.h"
#include "ZygoMxClient.h"
#include "ZygoInstrumentService.h"


CZygoInstrumentService::CZygoInstrumentService(void* pParent/*=NULL*/)
{
	m_pParent = pParent;
}


CZygoInstrumentService::~CZygoInstrumentService()
{
}

void CZygoInstrumentService::AutoLightLevel() // 554
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)5; packet << (byte)4;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

double CZygoInstrumentService::GetLightLevel() // 581
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)8; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	double dLightLevel = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dLightLevel;
}

CString CZygoInstrumentService::Measure(BOOL bWait) // 532
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	byte _wait = bWait ? (byte)1 : (byte)0;
	CPacket packet;
	packet << (byte)5; packet << (byte)3; packet << (byte)2;
	packet << _wait; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

void CZygoInstrumentService::AutoFocus() // 551
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)5; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoInstrumentService::MoveTurret(int position) // 562
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	INT32 _nTurret = (INT32)position;
	CPacket packet;
	packet << (byte)5; packet << (byte)6; packet << (byte)2;
	packet << _nTurret; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + 1 * sizeof(INT32);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoInstrumentService::SetZoom(double dZoom) // 572
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)7; packet << (byte)2;
	packet << dZoom; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + 1 * sizeof(double);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoInstrumentService::SetLightLevel(double dlightLevel) // 582
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)8; packet << (byte)2;
	packet << dlightLevel; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + 1 * sizeof(double);
	pMxClient->Send((char*)packet.GetData(), nLen);
}
