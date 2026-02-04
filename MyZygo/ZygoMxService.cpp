#include "stdafx.h"
#include "ZygoMxClient.h"
#include "ZygoMxService.h"


CZygoMxService::CZygoMxService(void* pParent/*=NULL*/)
{
	m_pParent = pParent;
}

CZygoMxService::~CZygoMxService()
{
}

BOOL CZygoMxService::IsApplicationOpen() // 411
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)1; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	BOOL bRtn = pMxClient->GetReturnBool(pMxClient->Send((char*)packet.GetData(), nLen));
	return bRtn;
}

CString CZygoMxService::GetApplicationPath() // 412
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)1; packet << (byte)2;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

void CZygoMxService::OpenApplication(CString fileName) // 413
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = fileName.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(fileName, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)1; packet << (byte)3;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoMxService::CloseApplication() // 414
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)1; packet << (byte)4;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoMxService::SaveApplicationAs(CString sPath) // 415
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = sPath.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sPath, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)1; packet << (byte)5;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoMxService::LoadSettings(CString sPath) // 416
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = sPath.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sPath, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)1; packet << (byte)6;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoMxService::SaveSettings(CString sPath) // 417
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = sPath.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sPath, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)1; packet << (byte)7;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoMxService::Analyze() // 431
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)3; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoMxService::LoadData(CString sPath) // 432
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = sPath.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sPath, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)3; packet << (byte)2;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

BOOL CZygoMxService::SaveData(CString sPath) // 433
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = sPath.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sPath, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)3; packet << (byte)3;
	packet << pData; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	BOOL bRtn = pMxClient->GetReturnBool(pMxClient->Send((char*)packet.GetData(), nLen));
	return bRtn;
}

void CZygoMxService::LoadSignalData(CString sPath) // 434
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = sPath.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sPath, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)3; packet << (byte)4;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoMxService::SaveSignalData(CString sPath) // 435
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = sPath.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sPath, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)3; packet << (byte)5;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoMxService::ResetData() // 436
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)3; packet << (byte)6;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoMxService::ClearProcessStats() // 451
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)5; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoMxService::StoreProcessStats() // 452
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)5; packet << (byte)2;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoMxService::LogReports() // 453
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)5; packet << (byte)3;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoMxService::CreateAnnotation(CString annotationLabel, CString annotationValue) // 471
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	CString sData;
	sData.Format(_T("%s,%s"), annotationLabel, annotationValue);
	int _nLen = sData.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sData, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)7; packet << (byte)1;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoMxService::LogInfo(CString message) // 481
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = message.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(message, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)8; packet << (byte)1;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoMxService::LogError(CString message) // 482
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = message.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(message, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)8; packet << (byte)2;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoMxService::LogFatal(CString message) // 483
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = message.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(message, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)8; packet << (byte)3;
	packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoMxService::SetUserDefinedWaves(double valueToSet) // 4101
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	DOUBLE _valueToSet = (DOUBLE)valueToSet;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)10; packet << (byte)1;
	packet << _valueToSet; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(DOUBLE);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

BOOL CZygoMxService::GetAutoSaveDataStatus() // 4111
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)11; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	BOOL bRtn = pMxClient->GetReturnBool(pMxClient->Send((char*)packet.GetData(), nLen));
	return bRtn;
}

void CZygoMxService::SetAutoSaveDataStatus(BOOL status) // 4112
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	byte _status = status ? (byte)1 : (byte)0;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)11; packet << (byte)2;
	packet << _status;
	packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoMxService::StartSequence(int count) // 4121
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _count = (INT32)count;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)4; packet << (byte)12; packet << (byte)1;
	packet << _count; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

