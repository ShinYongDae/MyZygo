#pragma once
class CZygoMxService
{
	void* m_pParent;

public:
	CZygoMxService(void* pParent = NULL);
	~CZygoMxService();

public:
	BOOL IsApplicationOpen(); // 411
	CString GetApplicationPath(); // 412
	void OpenApplication(CString fileName); // 413
	void CloseApplication(); // 414
	void SaveApplicationAs(CString sPath); // 415
	void LoadSettings(CString sPath); // 416
	void SaveSettings(CString sPath); // 417
	void Analyze(); // 431
	void LoadData(CString sPath); // 432
	BOOL SaveData(CString sPath); // 433
	void LoadSignalData(CString sPath); // 434
	void SaveSignalData(CString sPath); // 435
	void ResetData(); // 436
	void ClearProcessStats(); // 451
	void StoreProcessStats(); // 452
	void LogReports(); // 453
	void CreateAnnotation(CString annotationLabel, CString annotationValue); // 471
	void LogInfo(CString message); // 481
	void LogError(CString message); // 482
	void LogFatal(CString message); // 483
	void SetUserDefinedWaves(double valueToSet); // 4101
	BOOL GetAutoSaveDataStatus(); // 4111
	void SetAutoSaveDataStatus(BOOL status); // 4112
	void StartSequence(int count); // 4121
};

