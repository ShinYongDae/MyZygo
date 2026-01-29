#pragma once
class CZygoInstrumentService
{
	void* m_pParent;

public:
	CZygoInstrumentService(void* pParent = NULL);
	~CZygoInstrumentService();

public:
	void AutoLightLevel(); // 554
	double GetLightLevel(); // 581
	CString Measure(BOOL bWait); // 532
	void AutoFocus(); // 551
	void MoveTurret(int position); // 562
	void SetZoom(double dZoom); // 572
	void SetLightLevel(double dlightLevel); // 582
};

