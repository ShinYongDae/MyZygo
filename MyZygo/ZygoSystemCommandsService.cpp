#include "stdafx.h"
#include "ZygoMxClient.h"
#include "ZygoSystemCommandsService.h"


CZygoSystemCommandsService::CZygoSystemCommandsService(void* pParent/*=NULL*/)
{
	m_pParent = pParent;
}

CZygoSystemCommandsService::~CZygoSystemCommandsService()
{
}

CString CZygoSystemCommandsService::GetComputerName() // 1111
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)11; packet << (byte)1; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

CString CZygoSystemCommandsService::GetOSName() // 1112
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)11; packet << (byte)1; packet << (byte)2;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

long CZygoSystemCommandsService::GetRamSize() // 1113
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)11; packet << (byte)1; packet << (byte)3;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	long lRtn = pMxClient->GetReturnLong(pMxClient->Send((char*)packet.GetData(), nLen));
	return lRtn;
}

CString CZygoSystemCommandsService::GetBinDir() // 1121
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)11; packet << (byte)2; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

CString CZygoSystemCommandsService::GetWorkingDir() // 1122
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)11; packet << (byte)2; packet << (byte)2;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

CString CZygoSystemCommandsService::GetOpenDir(int fileType) // 1123
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _fileType = (INT32)fileType;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)11; packet << (byte)2; packet << (byte)3;
	packet << _fileType; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

void CZygoSystemCommandsService::SetOpenDir(int nFileType, CString dirPath) // 1124
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _fileType = (INT32)nFileType;
	int _nLen = dirPath.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(dirPath, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)11; packet << (byte)2; packet << (byte)4;
	packet << _fileType; packet << pData; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

CString CZygoSystemCommandsService::GetSaveDir(int nFileType) // 1125
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _fileType = (INT32)nFileType;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)11; packet << (byte)2; packet << (byte)5;
	packet << _fileType; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

void CZygoSystemCommandsService::SetSaveDir(int nFileType, CString dirPath) // 1126
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _fileType = (INT32)nFileType;
	int _nLen = dirPath.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(dirPath, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)11; packet << (byte)2; packet << (byte)6;
	packet << _fileType; packet << pData; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

