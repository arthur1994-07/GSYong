#pragma once

#include "../Util/UIListBox.h"

class CInnerInterface;

class CLottoBasicBuyListPropertyUIComp;

// ���� ����Ʈ �ڽ� Component;
class CLottoBasicBuyListBoxUIComp : public CUIListBox
{
public:
	CLottoBasicBuyListBoxUIComp( CInnerInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoBasicBuyListBoxUIComp();

	// Property �߰�;
	virtual void AddItem(const TCHAR* item, ...);

	// Property ����;
	virtual void SetItem(int nIndex, const TCHAR* item, ...);

protected:
	// Property ����;
	CLottoBasicBuyListPropertyUIComp* CreateProperty();

private:
	CInnerInterface* m_pInterface;
};
