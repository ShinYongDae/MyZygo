#include "stdafx.h"
#include "ZygoMxClient.h"
#include "ZygoUserInterfaceService.h"


CZygoUserInterfaceService::CZygoUserInterfaceService(void* pParent/*=NULL*/)
{
	m_pParent = pParent;
}

CZygoUserInterfaceService::~CZygoUserInterfaceService()
{
	if (!m_myList.IsEmpty())
		m_myList.RemoveAll();
}

BOOL CZygoUserInterfaceService::ShowDialog(CString text, int nMode) // 1221
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = text.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(text, pData);
	INT32 _nMode = (INT32)nMode;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)12; packet << (byte)2; packet << (byte)1;
	packet << pData; packet << _nMode; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1) + sizeof(INT32);
	BOOL bRtn = pMxClient->GetReturnBool(pMxClient->Send((char*)packet.GetData(), nLen));
	delete pData;
	return bRtn;
}

CString CZygoUserInterfaceService::ShowInputDialog(CString text, CString defaultValue, int nMode, int nMaxLength) // 1222
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	CString sData;
	sData.Format(_T("%s,%s"), text, defaultValue);
	int _nLen = sData.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sData, pData);
	INT32 _nMode = (INT32)nMode;
	INT32 _nMaxLength = (INT32)nMaxLength;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)12; packet << (byte)2; packet << (byte)2;
	packet << pData; packet << _nMode; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1) + sizeof(INT32);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	delete pData;
	return sRtn;
}

void CZygoUserInterfaceService::ShowTimedDialog(CString text, int nMode, int seconds) // 1223
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = text.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(text, pData);
	INT32 _nMode = (INT32)nMode;
	INT32 _seconds = (INT32)seconds;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)12; packet << (byte)2; packet << (byte)3;
	packet << pData; packet << _nMode; packet << _seconds; 
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1) + 2 * sizeof(INT32);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

CString CZygoUserInterfaceService::GetTab(CString name) // 1251
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = name.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(name, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)12; packet << (byte)5; packet << (byte)1;
	packet << pData; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	delete pData;
	return sRtn;
}

CList<CString>& CZygoUserInterfaceService::GetTabs() // 1252
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)12; packet << (byte)5; packet << (byte)2;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));

	int _nLen = sRtn.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sRtn, pData);
	char* pData2 = new char[_nLen + 1]; // for '\0'
	memset((char*)pData2, 0, _nLen + 1);

	if (!m_myList.IsEmpty())
		m_myList.RemoveAll();

	int nPos = 0;
	CString str;
	for (int i = 0; i <= _nLen; i++)
	{
		if (pData[i] == ',' || pData[i] == '\0')
		{
			pData2[nPos] = '\0';
			str = pMxClient->CharToString(pData2);
			m_myList.AddTail(str);
			memset((char*)pData2, 0, _nLen + 1);
			nPos = 0;

			if (pData[i] == '\0')
				break;
		}
		else
		{
			pData2[nPos] = pData[i];
			nPos++;
		}
	}
	delete pData2;
	delete pData;
	return m_myList;
}

void CZygoUserInterfaceService::TabShow(CString name) // 1255
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = name.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(name, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)12; packet << (byte)5; packet << (byte)5;
	packet << pData; 
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

CList<CString>& CZygoUserInterfaceService::TabDockPanels(CString name) // 1258
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = name.GetLength();
	char* pData3 = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(name, pData3);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)12; packet << (byte)5; packet << (byte)8;
	packet << pData3;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));

	_nLen = sRtn.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sRtn, pData);
	char* pData2 = new char[_nLen + 1]; // for '\0'
	memset((char*)pData2, 0, _nLen + 1);

	if (!m_myList.IsEmpty())
		m_myList.RemoveAll();

	int nPos = 0;
	CString str;
	for (int i = 0; i <= _nLen; i++)
	{
		if (pData[i] == ',' || pData[i] == '\0')
		{
			pData2[nPos] = '\0';
			str = pMxClient->CharToString(pData2);
			m_myList.AddTail(str);
			memset((char*)pData2, 0, _nLen + 1);
			nPos = 0;

			if (pData[i] == '\0')
				break;
		}
		else
		{
			pData2[nPos] = pData[i];
			nPos++;
		}
	}
	delete pData3;
	delete pData2;
	delete pData;
	return m_myList;
}

CList<CString>& CZygoUserInterfaceService::TabGroups(CString name) // 1259
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = name.GetLength();
	char* pData3 = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(name, pData3);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)12; packet << (byte)5; packet << (byte)9;
	packet << pData3;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));

	_nLen = sRtn.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sRtn, pData);
	char* pData2 = new char[_nLen + 1]; // for '\0'
	memset((char*)pData2, 0, _nLen + 1);

	if (!m_myList.IsEmpty())
		m_myList.RemoveAll();

	int nPos = 0;
	CString str;
	for (int i = 0; i <= _nLen; i++)
	{
		if (pData[i] == ',' || pData[i] == '\0')
		{
			pData2[nPos] = '\0';
			str = pMxClient->CharToString(pData2);
			m_myList.AddTail(str);
			memset((char*)pData2, 0, _nLen + 1);
			nPos = 0;

			if (pData[i] == '\0')
				break;
		}
		else
		{
			pData2[nPos] = pData[i];
			nPos++;
		}
	}
	delete pData3;
	delete pData2;
	delete pData;
	return m_myList;
}

void CZygoUserInterfaceService::Pin(BOOL doPin) // 1261
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	byte _doPin = doPin ? (byte)1 : (byte)0;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)12; packet << (byte)6; packet << (byte)1;
	packet << _doPin; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

CList<CString>& CZygoUserInterfaceService::TabGroupContainers(CString sTab, CString sGroup) // 1283
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	CString sData;
	sData.Format(_T("%s,%s"), sTab, sGroup);
	int _nLen = sData.GetLength();
	char* pData3 = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sData, pData3);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)12; packet << (byte)8; packet << (byte)3;
	packet << pData3;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));

	_nLen = sRtn.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sRtn, pData);
	char* pData2 = new char[_nLen + 1]; // for '\0'
	memset((char*)pData2, 0, _nLen + 1);

	if (!m_myList.IsEmpty())
		m_myList.RemoveAll();

	int nPos = 0;
	CString str;
	for (int i = 0; i <= _nLen; i++)
	{
		if (pData[i] == ',' || pData[i] == '\0')
		{
			pData2[nPos] = '\0';
			str = pMxClient->CharToString(pData2);
			m_myList.AddTail(str);
			memset((char*)pData2, 0, _nLen + 1);
			nPos = 0;

			if (pData[i] == '\0')
				break;
		}
		else
		{
			pData2[nPos] = pData[i];
			nPos++;
		}
	}
	delete pData3;
	delete pData2;
	delete pData;
	return m_myList;
}

CList<CString>& CZygoUserInterfaceService::TabGroupContainerControls(CString sTab, CString sGroup, CString sContainer) // 1293
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	CString sData;
	sData.Format(_T("%s,%s,%s"), sTab, sGroup, sContainer);
	int _nLen = sData.GetLength();
	char* pData3 = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sData, pData3);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)12; packet << (byte)9; packet << (byte)3;
	packet << pData3;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));

	_nLen = sRtn.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sRtn, pData);
	char* pData2 = new char[_nLen + 1]; // for '\0'
	memset((char*)pData2, 0, _nLen + 1);

	if (!m_myList.IsEmpty())
		m_myList.RemoveAll();

	int nPos = 0;
	CString str;
	for (int i = 0; i <= _nLen; i++)
	{
		if (pData[i] == ',' || pData[i] == '\0')
		{
			pData2[nPos] = '\0';
			str = pMxClient->CharToString(pData2);
			m_myList.AddTail(str);
			memset((char*)pData2, 0, _nLen + 1);
			nPos = 0;

			if (pData[i] == '\0')
				break;
		}
		else
		{
			pData2[nPos] = pData[i];
			nPos++;
		}
	}
	delete pData3;
	delete pData2;
	delete pData;
	return m_myList;
}

