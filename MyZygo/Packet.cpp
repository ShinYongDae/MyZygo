#include "stdafx.h"
#include "Packet.h"


CPacket::CPacket()
{
	pos = 0;
	invalid = true;
}


CPacket::~CPacket()
{
}

bool CPacket::CheckSize(std::size_t size)
{
	invalid = invalid && (pos + size <= data.size());
	return invalid;
}

void CPacket::Append(const void* data, std::size_t size)
{
	if (data && (size > 0))
	{
		std::size_t start = this->data.size();
		this->data.resize(start + size);
		memcpy(&this->data[start], data, size);
	}
}

void CPacket::Clear()
{
	data.clear();
	pos = 0;
	invalid = true;
}

const char* CPacket::GetData() const
{
	return !data.empty() ? &data[0] : NULL;
}

std::size_t CPacket::GetDataSize() const
{
	return  data.size();;
}

bool CPacket::EndOfPacket() const
{
	return pos >= data.size();
}


CPacket::operator bool() const
{
	return invalid;
}


CPacket& CPacket::operator >> (bool& data)
{
	uint8_t value;
	if (*this >> value)
		data = (value != 0);
	return *this;
}

CPacket& CPacket::operator >> (int8_t&data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const int8_t*>(GetData() + pos);
		pos += sizeof(data);
	}

	return *this;
}

CPacket& CPacket::operator >> (uint8_t& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const uint8_t*>(GetData() + pos);
		pos += sizeof(data);
	}

	return *this;
}

CPacket& CPacket::operator >> (int16_t& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const int16_t*>(GetData() + pos);
		pos += sizeof(data);
	}

	return *this;
}

CPacket& CPacket::operator >> (uint16_t& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const uint16_t*>(GetData() + pos);
		pos += sizeof(data);
	}

	return *this;

}

CPacket& CPacket::operator >> (int32_t& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const int32_t*>(GetData() + pos);
		pos += sizeof(data);
	}

	return *this;
}

CPacket& CPacket::operator >> (uint32_t& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const uint32_t*>(GetData() + pos);
		pos += sizeof(data);
	}

	return *this;
}

CPacket& CPacket::operator >> (int64_t& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const int64_t*>(GetData() + pos);
		pos += sizeof(data);
	}

	return *this;
}

CPacket& CPacket::operator >> (uint64_t& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const uint64_t*>(GetData() + pos);
		pos += sizeof(data);
	}

	return *this;
}

CPacket& CPacket::operator >> (float& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const float*>(GetData() + pos);
		pos += sizeof(data);
	}

	return *this;
}

CPacket& CPacket::operator >> (double& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const double*>(GetData() + pos);
		pos += sizeof(data);
	}

	return *this;
}

CPacket& CPacket::operator >> (char* data)
{
	// First extract string length
	uint32_t length = 0;
	*this >> length;

	if ((length > 0) && CheckSize(length))
	{
		// Then extract characters
		memcpy(data, GetData() + pos, length);
		data[length] = '\0';

		// Update reading position
		pos += length;
	}

	return *this;
}

CPacket& CPacket::operator >> (std::string& data)
{
	// First extract string length
	uint32_t length;
	*this >> length;

	data.clear();
	if ((length > 0) && CheckSize(length))
	{
		// Then extract characters
		data.assign(GetData() + pos, length);

		// Update reading position
		pos += length;
	}

	return *this;
}

CPacket& CPacket::operator >> (wchar_t* data)
{
	// First extract string length
	uint32_t length;
	*this >> length;

	//if ((length > 0) && CheckSize(length * sizeof(uint32_t)))
	//{
	// Then extract characters
	for (uint32_t i = 0; i < length; ++i)
	{
		uint8_t character;
		*this >> character;
		data[i] = static_cast<wchar_t>(character);
	}
	data[length] = L'\0';
	//}

	return *this;

}

CPacket& CPacket::operator >> (std::wstring& data)
{
	// First extract string length
	uint32_t length;
	*this >> length;

	data.clear();
	if ((length > 0) && CheckSize(length * sizeof(uint32_t)))
	{
		// Then extract characters
		for (uint32_t i = 0; i < length; ++i)
		{
			uint32_t character;
			*this >> character;
			data += static_cast<wchar_t>(character);
		}
	}

	return *this;
}


CPacket& CPacket::operator <<(bool data)
{
	*this << static_cast<uint8_t>(data);
	return *this;
}

CPacket& CPacket::operator <<(int8_t data)
{
	Append(&data, sizeof(data));
	return *this;
}

CPacket& CPacket::operator <<(uint8_t data)
{
	Append(&data, sizeof(data));
	return *this;
}

CPacket& CPacket::operator <<(int16_t data)
{
	Append(&data, sizeof(data));
	return *this;
}

CPacket& CPacket::operator <<(uint16_t data)
{
	Append(&data, sizeof(data));
	return *this;
}

CPacket& CPacket::operator <<(int32_t data)
{
	Append(&data, sizeof(data));
	return *this;
}

CPacket& CPacket::operator <<(uint32_t data)
{
	Append(&data, sizeof(data));
	return *this;
}

CPacket& CPacket::operator <<(int64_t data)
{
	Append(&data, sizeof(data));
	return *this;
}

CPacket& CPacket::operator <<(uint64_t data)
{
	Append(&data, sizeof(data));
	return *this;
}

CPacket& CPacket::operator <<(float data)
{
	Append(&data, sizeof(data));
	return *this;

}

CPacket& CPacket::operator <<(double data)
{
	Append(&data, sizeof(data));
	return *this;
}

CPacket& CPacket::operator <<(const char* data)
{
	// First insert string length
	uint32_t length = 0;
	for (const char* c = data; *c != '\0'; ++c)
		++length;
	*this << length;

	// Then insert characters
	Append(data, length * sizeof(char));
	return *this;
}

CPacket& CPacket::operator <<(const std::string& data)
{
	// First insert string length
	uint32_t length = static_cast<uint32_t>(data.size());
	*this << length;

	// Then insert characters
	if (length > 0)
	{
		Append(data.c_str(), length * sizeof(std::string::value_type));
	}
	return *this;
}

CPacket& CPacket::operator <<(const wchar_t* data)
{
	// first insert string length
	uint32_t length = 0;
	for (const wchar_t* c = data; *c != L'\0'; ++c)
		++length;
	*this << length;

	// then insert characters
	for (const wchar_t* c = data; *c != L'\0'; ++c)
		*this << static_cast<int32_t>(*c);

	return *this;
}

CPacket& CPacket::operator <<(const std::wstring& data)
{
	// first insert string length
	uint32_t length = static_cast<uint32_t>(data.size());
	*this << length;

	// then insert characters
	if (length > 0)
	{
		for (std::wstring::const_iterator c = data.begin(); c != data.end(); ++c)
			*this << static_cast<int32_t>(*c);
	}
	return *this;
}
