#include "stdafx.h"
#include "Param.h"

CParam& CParam::operator << (const TCHAR* str)
{
	size_t s = GetByteFromString(str);
	m_vBuffer.resize(m_vBuffer.size() + s);
	memcpy(&m_vBuffer.front() + m_write, str, s);
	m_write += s;
	return *this;
}

CParam& CParam::operator << (TCHAR* str)
{
	return operator << (static_cast<const TCHAR*>(str));
}

CParam& CParam::operator >> (const TCHAR*& str)
{
	str = reinterpret_cast<TCHAR*>(&m_vBuffer.front() + m_read);
	m_read += GetByteFromString(str);
	return *this;
}

CParam& CParam::operator >> (TCHAR*& str)
{
	return operator >> ( const_cast<const TCHAR*&>(str) );
}

void CParam::Reset()
{
	m_read = 0;
}

void CParam::Clear()
{
	m_read = m_write = 0;
	m_vBuffer.clear();
}

void CParam::Remove()
{
    BufferContainerIterator it = m_vBuffer.begin();
    std::advance(it, m_read);

    m_vBuffer.erase(m_vBuffer.begin(), it);
    m_write -= m_read;
    m_read = 0;
}

size_t CParam::GetByteFromString(const TCHAR* str)
{
	size_t s = _tcslen(str);

	if (s > 0)
	{
		return (s + 1) * sizeof(TCHAR);
	}

	return 0;
}
