#pragma once

#include "../Util/UIListBox.h"

#include "./SelectCountryListProperty.h"

class COuterInterface;

// ���� ����Ʈ �ڽ� Component;
class CSelectCountryListComp : public CUIListBox
{
public:
	CSelectCountryListComp ( COuterInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CSelectCountryListComp ();

	// Property �߰�;
	virtual void AddItem ( const TCHAR* item, ... );

	// Property ����;
	virtual void SetItem ( int nIndex, const TCHAR* item, ... );

public:
	CSelectCountryListProperty* GetProperty ( int nIndex );

protected:
	// Property ����;
	CSelectCountryListProperty* CreateProperty ();

private:
	COuterInterface* m_pInterface;
};
