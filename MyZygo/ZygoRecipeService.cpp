#include "stdafx.h"
#include "ZygoMxClient.h"
#include "ZygoRecipeService.h"


CZygoRecipeService::CZygoRecipeService(void* pParent/*=NULL*/)
{
	m_pParent = pParent;
}


CZygoRecipeService::~CZygoRecipeService()
{
}

void CZygoRecipeService::Load(CString fileName) // 1011
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = fileName.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(fileName, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)10; packet << (byte)1; packet << (byte)1;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoRecipeService::Save(CString fileName) // 1012
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = fileName.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(fileName, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)10; packet << (byte)1; packet << (byte)2;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoRecipeService::Run() // 1021
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)10; packet << (byte)2; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

