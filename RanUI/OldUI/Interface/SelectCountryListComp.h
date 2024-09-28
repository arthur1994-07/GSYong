#pragma once

#include "../Util/UIListBox.h"

#include "./SelectCountryListProperty.h"

class COuterInterface;

// 구매 리스트 박스 Component;
class CSelectCountryListComp : public CUIListBox
{
public:
	CSelectCountryListComp ( COuterInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CSelectCountryListComp ();

	// Property 추가;
	virtual void AddItem ( const TCHAR* item, ... );

	// Property 수정;
	virtual void SetItem ( int nIndex, const TCHAR* item, ... );

public:
	CSelectCountryListProperty* GetProperty ( int nIndex );

protected:
	// Property 생성;
	CSelectCountryListProperty* CreateProperty ();

private:
	COuterInterface* m_pInterface;
};
