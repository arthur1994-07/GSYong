#pragma once

#include "../Util/UIListBox.h"

class CInnerInterface;

class CLottoBasicBuyListPropertyUIComp;

// 구매 리스트 박스 Component;
class CLottoBasicBuyListBoxUIComp : public CUIListBox
{
public:
	CLottoBasicBuyListBoxUIComp( CInnerInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoBasicBuyListBoxUIComp();

	// Property 추가;
	virtual void AddItem(const TCHAR* item, ...);

	// Property 수정;
	virtual void SetItem(int nIndex, const TCHAR* item, ...);

protected:
	// Property 생성;
	CLottoBasicBuyListPropertyUIComp* CreateProperty();

private:
	CInnerInterface* m_pInterface;
};
