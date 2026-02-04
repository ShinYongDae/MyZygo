#include "stdafx.h"
#include "ZygoMxClient.h"
#include "ZygoPatternService.h"


CZygoPatternService::CZygoPatternService(void* pParent/*=NULL*/)
{
	m_pParent = pParent;
}

CZygoPatternService::~CZygoPatternService()
{
}

void CZygoPatternService::Load(CString fileName) // 911
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = fileName.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(fileName, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)9; packet << (byte)1; packet << (byte)1;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoPatternService::Save(CString fileName) // 912
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = fileName.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(fileName, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)9; packet << (byte)1; packet << (byte)2;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoPatternService::Align() // 921
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)9; packet << (byte)2; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoPatternService::PreAlign() // 922
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)9; packet << (byte)2; packet << (byte)2;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoPatternService::Run() // 923
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)9; packet << (byte)2; packet << (byte)3;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

