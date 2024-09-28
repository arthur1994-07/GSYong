#pragma once

#include "../Util/UIListBox.h"

#include "./SelectContinentListProperty.h"

class COuterInterface;

// ���� ����Ʈ �ڽ� Component;
class CSelectContinentListComp : public CUIListBox
{
public:
	CSelectContinentListComp ( COuterInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CSelectContinentListComp ();

	// Property �߰�;
	virtual void AddItem ( const TCHAR* item, ... );

	// Property ����;
	virtual void SetItem ( int nIndex, const TCHAR* item, ... );

public:
	CSelectContinentListProperty* GetProperty ( int nIndex );

protected:
	// Property ����;
	CSelectContinentListProperty* CreateProperty ();

private:
	COuterInterface* m_pInterface;
};
