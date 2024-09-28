#pragma once

#include "../Util/UIListBox.h"

#include "./SelectContinentListProperty.h"

class COuterInterface;

// 구매 리스트 박스 Component;
class CSelectContinentListComp : public CUIListBox
{
public:
	CSelectContinentListComp ( COuterInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CSelectContinentListComp ();

	// Property 추가;
	virtual void AddItem ( const TCHAR* item, ... );

	// Property 수정;
	virtual void SetItem ( int nIndex, const TCHAR* item, ... );

public:
	CSelectContinentListProperty* GetProperty ( int nIndex );

protected:
	// Property 생성;
	CSelectContinentListProperty* CreateProperty ();

private:
	COuterInterface* m_pInterface;
};
