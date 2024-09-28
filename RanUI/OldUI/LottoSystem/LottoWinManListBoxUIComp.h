#pragma once

#include "../Util/UIListBox.h"

class CInnerInterface;

class CLottoWinManListPropertyUIComp;

// 구매 리스트 박스 Component;
class CLottoWinManListBoxUIComp : public CUIListBox
{
public:
	CLottoWinManListBoxUIComp( CInnerInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoWinManListBoxUIComp();

	// Property 추가;
	virtual void AddItem(const TCHAR* item, ...);

	// Property 수정;
	virtual void SetItem(int nIndex, const TCHAR* item, ...);

protected:
	// Property 생성;
	CLottoWinManListPropertyUIComp* CreateProperty();

private:
	CInnerInterface* m_pInterface;
};
