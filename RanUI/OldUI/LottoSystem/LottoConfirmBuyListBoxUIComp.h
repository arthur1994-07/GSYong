#pragma once

#include "../Util/UIListBox.h"

class CInnerInterface;

class CLottoConfirmBuyListPropertyUIComp;

// ���� ����Ʈ �ڽ� Component;
class CLottoConfirmBuyListBoxUIComp : public CUIListBox
{
public:
	CLottoConfirmBuyListBoxUIComp( CInnerInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoConfirmBuyListBoxUIComp();

	// Property �߰�;
	virtual void AddItem(const TCHAR* item, ...);

	// Property ����;
	virtual void SetItem(int nIndex, const TCHAR* item, ...);

public:
	CLottoConfirmBuyListPropertyUIComp* GetProperty( int nIndex );

protected:
	// Property ����;
	CLottoConfirmBuyListPropertyUIComp* CreateProperty();

private:
	CInnerInterface* m_pInterface;
};
