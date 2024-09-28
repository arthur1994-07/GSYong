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
        // ���� �ѹ��� ȣ��ǰ� �ϱ� ���ؼ� ���� �ð��� �����Ѵ�.
        m_LastReqTime = GetTickCount() - 60000;
	}

	GLInventoryBank( WORD _sizeX, WORD _sizeY )
		: GLInventory( _sizeX, _sizeY )
	{
        // ���� �ѹ��� ȣ��ǰ� �ϱ� ���ؼ� ���� �ð��� �����Ѵ�.
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