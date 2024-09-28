#pragma	once
#include "../../InnerInterface.h"
#include "../../EngineLib/GUInterface/UIGroup.h"
class CBasicTextBox;


class InstanceRow : public CUIGroup
{

protected:
	enum
	{

	};

public:
	InstanceRow ( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~InstanceRow ();

	virtual void		CreateSubControl ();

public:
	virtual void		Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void		TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );


private:
	CBasicTextBox*	m_pInstanceName;
	CUIControl*	m_pWatingImage;
	DWORD m_dwInstanceId;

public:
	void SetText(const TCHAR* pText, DWORD dwColor = NS_UITEXTCOLOR::WHITE);
	void ClearText();
	void SetInstanceId(DWORD dwID);
	DWORD GetInstanceId();

};
