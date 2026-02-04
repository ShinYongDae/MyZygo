#pragma once
class CZygoPatternService
{
	void* m_pParent;

public:
	CZygoPatternService(void* pParent = NULL);
	~CZygoPatternService();

public:
	void Load(CString fileName); // 911
	void Save(CString fileName); // 912
	void Align(); // 921
	void PreAlign(); // 922
	void Run(); // 923
};

