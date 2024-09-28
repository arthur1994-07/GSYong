#pragma once

#include "../Util/UIListBox.h"

class CInnerInterface;

class CLottoWinManListPropertyUIComp;

// ���� ����Ʈ �ڽ� Component;
class CLottoWinManListBoxUIComp : public CUIListBox
{
public:
	CLottoWinManListBoxUIComp( CInnerInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoWinManListBoxUIComp();

	// Property �߰�;
	virtual void AddItem(const TCHAR* item, ...);

	// Property ����;
	virtual void SetItem(int nIndex, const TCHAR* item, ...);

protected:
	// Property ����;
	CLottoWinManListPropertyUIComp* CreateProperty();

private:
	CInnerInterface* m_pInterface;
};
