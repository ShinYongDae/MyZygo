#pragma once
#include <vector>

class CPacket
{
	std::vector<char> data;
	std::size_t       pos;
	bool              invalid;

	bool CheckSize(std::size_t size);

public:
	CPacket();
	~CPacket();

public:
	void Append(const void* data, std::size_t sizeInBytes);
	void Clear();
	const char* GetData() const;
	std::size_t GetDataSize() const;
	bool EndOfPacket() const;

public:
	operator bool() const;

	CPacket& operator >> (bool& data);
	CPacket& operator >> (int8_t& data);
	CPacket& operator >> (uint8_t& data);
	CPacket& operator >> (int16_t& data);
	CPacket& operator >> (uint16_t& data);
	CPacket& operator >> (int32_t& data);
	CPacket& operator >> (uint32_t& data);
	CPacket& operator >> (int64_t& data);
	CPacket& operator >> (uint64_t& data);
	CPacket& operator >> (float& data);
	CPacket& operator >> (double& data);
	CPacket& operator >> (char* data);
	CPacket& operator >> (std::string&  data);
	CPacket& operator >> (wchar_t* data);
	CPacket& operator >> (std::wstring& data);

	CPacket& operator << (bool data);
	CPacket& operator << (int8_t data);
	CPacket& operator << (uint8_t  data);
	CPacket& operator << (int16_t  data);
	CPacket& operator << (uint16_t data);
	CPacket& operator << (int32_t data);
	CPacket& operator << (uint32_t  data);
	CPacket& operator << (int64_t data);
	CPacket& operator << (uint64_t  data);
	CPacket& operator << (float data);
	CPacket& operator << (double data);
	CPacket& operator << (const char* data);
	CPacket& operator << (const std::string& data);
	CPacket& operator << (const wchar_t* data);
	CPacket& operator << (const std::wstring& data);
};

