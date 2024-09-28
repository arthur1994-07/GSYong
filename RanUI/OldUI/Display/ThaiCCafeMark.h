#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"


class CThaiCCafeMark : public CUIGroup
{
public:
	CUIControl*		m_pImage;
	DWORD			m_dwClass;

public:
	CThaiCCafeMark(EngineDeviceMan* pEngineDevice);
	virtual ~CThaiCCafeMark();
	
	
	void	CreateSubControl ();
	DWORD	GetClass()	{ return m_dwClass; };
	void	SetClass( DWORD dwClass );
};