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
	INT32 _nTurret = (INT32)position;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)6; packet << (byte)2;
	packet << _nTurret; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoInstrumentService::SetZoom(double dZoom) // 572
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)7; packet << (byte)2;
	packet << dZoom; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(double);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoInstrumentService::SetLightLevel(double dlightLevel) // 582
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)8; packet << (byte)2;
	packet << dlightLevel; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(double);
	pMxClient->Send((char*)packet.GetData(), nLen);
}



AlignViewMode CZygoInstrumentService::GetAlignViewMode() // 511
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)1; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	int nRtn = pMxClient->GetReturnInt(pMxClient->Send((char*)packet.GetData(), nLen));
	AlignViewMode rtn = (AlignViewMode)nRtn;
	return rtn;
}

void CZygoInstrumentService::SetAlignViewMode(int mode) // 512
{
	INT32 nMode = (INT32)mode;
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)1; packet << (byte)2;
	packet << nMode; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

RingSpotMode CZygoInstrumentService::GetRingSpotMode() // 521
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)2; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	int nRtn = pMxClient->GetReturnInt(pMxClient->Send((char*)packet.GetData(), nLen));
	RingSpotMode rtn = (RingSpotMode)nRtn;
	return rtn;
}

void CZygoInstrumentService::SetRingSpotMode(int mode) // 522
{
	INT32 nMode = (INT32)mode;
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)2; packet << (byte)2;
	packet << nMode; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(INT32);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

CString CZygoInstrumentService::Acquire(BOOL bWait) // 531
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	byte _wait = bWait ? (byte)1 : (byte)0;
	CPacket packet;
	packet << (byte)5; packet << (byte)3; packet << (byte)1;
	packet << _wait; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

BOOL CZygoInstrumentService::IsFrameGrabComplete(CString sId) // 541
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = sId.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sId, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)4; packet << (byte)1;
	packet << pData; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	BOOL rtn = pMxClient->GetReturnBool(pMxClient->Send((char*)packet.GetData(), nLen));
	delete pData;
	return rtn;
}

void CZygoInstrumentService::WaitForFrameGrabComplete(CString taskId, double timeout) // 542
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = taskId.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(taskId, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)4; packet << (byte)2;
	packet << pData; packet << timeout; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1) + sizeof(double);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

BOOL CZygoInstrumentService::IsAcquisitionComplete(CString sId) // 543
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = sId.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sId, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)4; packet << (byte)3;
	packet << pData; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	BOOL rtn = pMxClient->GetReturnBool(pMxClient->Send((char*)packet.GetData(), nLen));
	delete pData;
	return rtn;
}

void CZygoInstrumentService::WaitForAcquisitionComplete(CString taskId, double timeout) // 544
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = taskId.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(taskId, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)4; packet << (byte)4;
	packet << pData; packet << timeout; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1) + sizeof(double);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

BOOL CZygoInstrumentService::IsMeasureComplete(CString sId) // 545
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = sId.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(sId, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)4; packet << (byte)5;
	packet << pData; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1);
	BOOL rtn = pMxClient->GetReturnBool(pMxClient->Send((char*)packet.GetData(), nLen));
	delete pData;
	return rtn;
}

void CZygoInstrumentService::WaitForMeasureComplete(CString taskId, double timeout) // 546
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	int _nLen = taskId.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(taskId, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)4; packet << (byte)6;
	packet << pData; packet << timeout; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + (_nLen + 1) + sizeof(double);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoInstrumentService::AutoFocusTilt() // 553
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)5; packet << (byte)3;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoInstrumentService::AutoLateralCalibration(double length) // 555
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)5; packet << (byte)5;
	packet << length; packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte) + sizeof(double);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoInstrumentService::AutoCenter() // 556
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)5; packet << (byte)6;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

int CZygoInstrumentService::GetTurret() // 561
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)6; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	int nRtn = pMxClient->GetReturnInt(pMxClient->Send((char*)packet.GetData(), nLen));
	return nRtn;
}

double CZygoInstrumentService::GetZoom() // 571
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)7; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	double dRtn = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dRtn;
}

double CZygoInstrumentService::GetMaximumZoom() // 573
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)7; packet << (byte)3;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	double dRtn = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dRtn;
}

double CZygoInstrumentService::GetMinimumZoom() // 574
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)7; packet << (byte)4;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	double dRtn = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dRtn;
}

void CZygoInstrumentService::LockZoom() // 575
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)7; packet << (byte)5;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoInstrumentService::UnlockZoom() // 576
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)7; packet << (byte)6;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoInstrumentService::SetWandEnabled(BOOL enabled) // 591
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	byte _enabled = enabled ? (byte)1 : (byte)0;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)9; packet << (byte)1;
	packet << _enabled; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

double CZygoInstrumentService::GetCameraResolution() // 5101, 5102
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)10; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	double dRtn = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dRtn;
}

double CZygoInstrumentService::GetCameraSizeX() // 5103, 5104
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)10; packet << (byte)3;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	double dRtn = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dRtn;
}

double CZygoInstrumentService::GetCameraSizeY() // 5105, 5106
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)10; packet << (byte)5;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	double dRtn = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dRtn;
}

void CZygoInstrumentService::SetCameraResolution(double value) // 5107
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)10; packet << (byte)7;
	packet << value; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + sizeof(double);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoInstrumentService::SetSleepModeEnabled(BOOL enabled) // 5111
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	byte _enabled = enabled ? (byte)1 : (byte)0;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)11; packet << (byte)1;
	packet << _enabled; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

CString CZygoInstrumentService::GetSystemTypeName() // 5112
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)11; packet << (byte)2;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

int CZygoInstrumentService::ReadSdoInt8(byte nodeid, int index, int subindex) // 5113
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _index = (INT32)index;
	INT32 _subindex = (INT32)subindex;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)11; packet << (byte)3;
	packet << nodeid; packet << _index; packet << _subindex; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 2 * sizeof(INT32);
	int nRtn = pMxClient->GetReturnInt(pMxClient->Send((char*)packet.GetData(), nLen));
	return nRtn;
}

void CZygoInstrumentService::WriteSdoInt8(byte nodeid, int index, int subindex, int val) // 5114
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _index = (INT32)index;
	INT32 _subindex = (INT32)subindex;
	INT32 _val = (INT32)val;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)11; packet << (byte)4;
	packet << nodeid; packet << _index; packet << _subindex; packet << _val;
	packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 3 * sizeof(INT32);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

int CZygoInstrumentService::ReadSdoInt16(byte nodeid, int index, int subindex) // 5115
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _index = (INT32)index;
	INT32 _subindex = (INT32)subindex;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)11; packet << (byte)5;
	packet << nodeid; packet << _index; packet << _subindex; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 2 * sizeof(INT32);
	int nRtn = pMxClient->GetReturnInt(pMxClient->Send((char*)packet.GetData(), nLen));
	return nRtn;
}

void CZygoInstrumentService::WriteSdoInt16(byte nodeid, int index, int subindex, int val) // 5116
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _index = (INT32)index;
	INT32 _subindex = (INT32)subindex;
	INT32 _val = (INT32)val;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)11; packet << (byte)6;
	packet << nodeid; packet << _index; packet << _subindex; packet << _val;
	packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 3 * sizeof(INT32);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

int CZygoInstrumentService::ReadSdoInt32(byte nodeid, int index, int subindex) // 5117
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _index = (INT32)index;
	INT32 _subindex = (INT32)subindex;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)11; packet << (byte)7;
	packet << nodeid; packet << _index; packet << _subindex; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 2 * sizeof(INT32);
	int nRtn = pMxClient->GetReturnInt(pMxClient->Send((char*)packet.GetData(), nLen));
	return nRtn;
}

void CZygoInstrumentService::WriteSdoInt32(byte nodeid, int index, int subindex, int val) // 5118
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _index = (INT32)index;
	INT32 _subindex = (INT32)subindex;
	INT32 _val = (INT32)val;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)11; packet << (byte)8;
	packet << nodeid; packet << _index; packet << _subindex; packet << _val;
	packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 3 * sizeof(INT32);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

float CZygoInstrumentService::ReadSdoReal32(byte nodeid, int index, int subindex) // 5119
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _index = (INT32)index;
	INT32 _subindex = (INT32)subindex;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)11; packet << (byte)9;
	packet << nodeid; packet << _index; packet << _subindex; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 2 * sizeof(INT32);
	pMxClient->Send((char*)packet.GetData(), nLen);
	float fRtn = pMxClient->GetReturnFloat(pMxClient->Send((char*)packet.GetData(), nLen));
	return fRtn;
}

void CZygoInstrumentService::WriteSdoReal32(byte nodeid, int index, int subindex, float val) // 51110
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _index = (INT32)index;
	INT32 _subindex = (INT32)subindex;
	FLOAT _val = (FLOAT)val;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)11; packet << (byte)10;
	packet << nodeid; packet << _index; packet << _subindex; packet << _val;
	packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 2 * sizeof(INT32) + sizeof(FLOAT);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

double CZygoInstrumentService::ReadSdoReal64(byte nodeid, int index, int subindex) // 51111
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _index = (INT32)index;
	INT32 _subindex = (INT32)subindex;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)11; packet << (byte)11;
	packet << nodeid; packet << _index; packet << _subindex; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 2 * sizeof(INT32);
	pMxClient->Send((char*)packet.GetData(), nLen);
	double dRtn = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dRtn;
}

void CZygoInstrumentService::WriteSdoReal64(byte nodeid, int index, int subindex, double val) // 51112
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _index = (INT32)index;
	INT32 _subindex = (INT32)subindex;
	DOUBLE _val = (DOUBLE)val;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)11; packet << (byte)12;
	packet << nodeid; packet << _index; packet << _subindex; packet << _val;
	packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 2 * sizeof(INT32) + sizeof(DOUBLE);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

CString CZygoInstrumentService::ReadSdoStr(byte nodeid, int index, int subindex) // 51113
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _index = (INT32)index;
	INT32 _subindex = (INT32)subindex;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)11; packet << (byte)13;
	packet << nodeid; packet << _index; packet << _subindex; packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 2 * sizeof(INT32);
	pMxClient->Send((char*)packet.GetData(), nLen);
	CString sRtn = pMxClient->GetReturnString(pMxClient->Send((char*)packet.GetData(), nLen));
	return sRtn;
}

void CZygoInstrumentService::WriteSdoStr(byte nodeid, int index, int subindex, CString val) // 51114
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	INT32 _index = (INT32)index;
	INT32 _subindex = (INT32)subindex;
	int _nLen = val.GetLength();
	char* pData = new char[_nLen + 1]; // for '\0'
	pMxClient->StringToChar(val, pData);

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)11; packet << (byte)14;
	packet << nodeid; packet << _index; packet << _subindex; packet << pData;
	packet << (byte)0; // NULL(for end)
	int nLen = 5 * sizeof(byte) + 2 * sizeof(INT32) + (_nLen + 1);
	pMxClient->Send((char*)packet.GetData(), nLen);
	delete pData;
}

void CZygoInstrumentService::CalculateGeometry() // 5121
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)12; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

void CZygoInstrumentService::CalculateHomogeneity() // 5122
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)12; packet << (byte)2;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

BOOL CZygoInstrumentService::IsDmiActive() // 5131
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)13; packet << (byte)1;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	BOOL rtn = pMxClient->GetReturnBool(pMxClient->Send((char*)packet.GetData(), nLen));
	return rtn;
}

DmiStatus CZygoInstrumentService::GetDmiStatus() // 5132
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)13; packet << (byte)2;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	int nRtn = pMxClient->GetReturnInt(pMxClient->Send((char*)packet.GetData(), nLen));
	DmiStatus rtn = (DmiStatus)nRtn;
	return rtn;
}

double CZygoInstrumentService::GetDmiPosition() // 5133
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;

	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)13; packet << (byte)3;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	double dRtn = pMxClient->GetReturnDouble(pMxClient->Send((char*)packet.GetData(), nLen));
	return dRtn;
}

void CZygoInstrumentService::ResetDmi() // 5134
{
	CZygoMxClient* pMxClient = (CZygoMxClient*)m_pParent;
	pMxClient->ClearReturn();
	CPacket packet;
	packet << (byte)5; packet << (byte)13; packet << (byte)4;
	packet << (byte)0; // NULL(for end)
	int nLen = 4 * sizeof(byte);
	pMxClient->Send((char*)packet.GetData(), nLen);
}

