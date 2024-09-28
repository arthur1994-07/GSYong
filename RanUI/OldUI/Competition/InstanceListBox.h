#pragma once

#include "../Util/UIListBox.h"
class CInnerInterface;

class InstanceRow;

// 구매 리스트 박스 Component;
class InstanceListBox : public CUIListBox
{
public:
	InstanceListBox( CInnerInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~InstanceListBox();

	// Property 추가;
	virtual void AddItem(const TCHAR* pText, ...);

	// Property 수정;
	virtual void SetItem(int nIndex, const TCHAR* item, ...);

	void SetPropertyText ( DWORD dwInstanceID, const std::string& strText );

protected:
	// Property 생성;
	InstanceRow* CreateProperty(const TCHAR* pText);

private:
	CInnerInterface* m_pInterface;
public:
	DWORD GetInstanceID();
};
