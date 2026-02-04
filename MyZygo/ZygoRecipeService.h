#pragma once
class CZygoRecipeService
{
	void* m_pParent;

public:
	CZygoRecipeService(void* pParent = NULL);
	~CZygoRecipeService();

public:
	void Load(CString fileName); // 1011
	void Save(CString fileName); // 1012
	void Run(); // 1021
};

