#pragma once

#include "../Util/UIListBox.h"
class CInnerInterface;

class InstanceRow;

// ���� ����Ʈ �ڽ� Component;
class InstanceListBox : public CUIListBox
{
public:
	InstanceListBox( CInnerInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~InstanceListBox();

	// Property �߰�;
	virtual void AddItem(const TCHAR* pText, ...);

	// Property ����;
	virtual void SetItem(int nIndex, const TCHAR* item, ...);

	void SetPropertyText ( DWORD dwInstanceID, const std::string& strText );

protected:
	// Property ����;
	InstanceRow* CreateProperty(const TCHAR* pText);

private:
	CInnerInterface* m_pInterface;
public:
	DWORD GetInstanceID();
};
