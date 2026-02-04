#pragma once
#include "ZygoEnumerations.h"

using namespace ZygoEnumeration;
using namespace ZygoEnumerations;

class CZygoInstrumentService
{
	void* m_pParent;

public:
	CZygoInstrumentService(void* pParent = NULL);
	~CZygoInstrumentService();

public:
	CString Measure(BOOL bWait); // 532
	void AutoFocus(); // 551
	void AutoLightLevel(); // 554
	void MoveTurret(int position); // 562
	void SetZoom(double dZoom); // 572
	double GetLightLevel(); // 581
	void SetLightLevel(double dlightLevel); // 582

public:
	AlignViewMode GetAlignViewMode(); // 511
	void SetAlignViewMode(int mode); // 512
	RingSpotMode GetRingSpotMode(); // 521
	void SetRingSpotMode(int mode); // 522
	CString Acquire(BOOL bWait); // 531
	BOOL IsFrameGrabComplete(CString sId); // 541
	void WaitForFrameGrabComplete(CString taskId, double timeout); // 542
	BOOL IsAcquisitionComplete(CString sId); // 543
	void WaitForAcquisitionComplete(CString taskId, double timeout); // 544
	BOOL IsMeasureComplete(CString sId); // 545
	void WaitForMeasureComplete(CString taskId, double timeout); // 546
	void AutoFocusTilt(); // 553
	void AutoLateralCalibration(double length); // 555
	void AutoCenter(); // 556
	int GetTurret(); // 561
	double GetZoom(); // 571
	double GetMaximumZoom(); // 573
	double GetMinimumZoom(); // 574
	void LockZoom(); // 575
	void UnlockZoom(); // 576
	void SetWandEnabled(BOOL enabled); // 591
	double GetCameraResolution(); // 5101, 5102
	double GetCameraSizeX(); // 5103, 5104
	double GetCameraSizeY(); // 5105, 5106
	void SetCameraResolution(double value); // 5107
	void SetSleepModeEnabled(BOOL enabled); // 5111
	CString GetSystemTypeName(); // 5112
	int ReadSdoInt8(byte nodeid, int index, int subindex); // 5113
	void WriteSdoInt8(byte nodeid, int index, int subindex, int val); // 5114
	int ReadSdoInt16(byte nodeid, int index, int subindex); // 5115
	void WriteSdoInt16(byte nodeid, int index, int subindex, int val); // 5116
	int ReadSdoInt32(byte nodeid, int index, int subindex); // 5117
	void WriteSdoInt32(byte nodeid, int index, int subindex, int val); // 5118
	float ReadSdoReal32(byte nodeid, int index, int subindex); // 5119
	void WriteSdoReal32(byte nodeid, int index, int subindex, float val); // 51110
	double ReadSdoReal64(byte nodeid, int index, int subindex); // 51111
	void WriteSdoReal64(byte nodeid, int index, int subindex, double val); // 51112
	CString ReadSdoStr(byte nodeid, int index, int subindex); // 51113
	void WriteSdoStr(byte nodeid, int index, int subindex, CString val); // 51114
	void CalculateGeometry(); // 5121
	void CalculateHomogeneity(); // 5122
	BOOL IsDmiActive(); // 5131
	DmiStatus GetDmiStatus(); // 5132
	double GetDmiPosition(); // 5133
	void ResetDmi(); // 5134
};

