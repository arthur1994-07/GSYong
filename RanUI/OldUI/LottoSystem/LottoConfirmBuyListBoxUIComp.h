#pragma once

#include "../Util/UIListBox.h"

class CInnerInterface;

class CLottoConfirmBuyListPropertyUIComp;

// 구매 리스트 박스 Component;
class CLottoConfirmBuyListBoxUIComp : public CUIListBox
{
public:
	CLottoConfirmBuyListBoxUIComp( CInnerInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoConfirmBuyListBoxUIComp();

	// Property 추가;
	virtual void AddItem(const TCHAR* item, ...);

	// Property 수정;
	virtual void SetItem(int nIndex, const TCHAR* item, ...);

public:
	CLottoConfirmBuyListPropertyUIComp* GetProperty( int nIndex );

protected:
	// Property 생성;
	CLottoConfirmBuyListPropertyUIComp* CreateProperty();

private:
	CInnerInterface* m_pInterface;
};
