
template<class T>
CParam& CParam::operator << (T t)
{
	size_t s = sizeof(t);
	m_vBuffer.resize(m_vBuffer.size() + s);
	*reinterpret_cast<T*>(&m_vBuffer.front() + m_write) = t;
	m_write += s;
	return *this;
}

template<class T>
CParam& CParam::operator >> (T& t)
{
	if (m_read < m_write)
	{
		t = *reinterpret_cast<T*>(&m_vBuffer.front() + m_read);
		m_read += sizeof(t);
	}
	return *this;
}

inline CParam& CParam::operator >> (void* fun)
{
	static_cast< void* (*) (CParam*) > (fun) (this);
    return *this;
}

inline void* Reset(CParam* pParam = 0)
{
	if (pParam != 0)
	{
		pParam->Reset();
	}

	return Reset;
}
