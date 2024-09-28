#pragma once

#include "./GLInventory.h"

enum
{
	EBankX			= 6,
	EBankY			= 10,
	EBankPage		= 5,

	EBankTotalLine	= EBankY * EBankPage,
};

class GLInventoryBank : public GLInventory
{
protected:
    DWORD m_LastReqTime;

public:
	GLInventoryBank()
		: GLInventory(EBankX, EBankTotalLine)
	{
        // 최초 한번은 호출되게 하기 위해서 과거 시간을 세팅한다.
        m_LastReqTime = GetTickCount() - 60000;
	}

	GLInventoryBank( WORD _sizeX, WORD _sizeY )
		: GLInventory( _sizeX, _sizeY )
	{
        // 최초 한번은 호출되게 하기 위해서 과거 시간을 세팅한다.
        m_LastReqTime = GetTickCount() - 60000;
	}

    DWORD GetLastReqTime() const { return m_LastReqTime; }
    void  SetLastReqTime(DWORD LastTime) { m_LastReqTime = LastTime; }

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<GLInventory>(*this);
	}
};