#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBoxEx;
class CBasicScrollBarEx;
class GLGaeaClient;

class CClubUnion : public CUIGroup
{
protected:
	enum
	{
		CLUB_LIST_TEXTBOX = NO_ID + 1,
		CLUB_LIST_SCROLLBAR
	};

public:
	CClubUnion(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CClubUnion();

protected:
    GLGaeaClient* m_pGaeaClient;
    int m_nSelectIndex;
    CString	m_strSelectName;

private:
    CBasicTextBoxEx*	m_pTextBox;
    CBasicScrollBarEx*	m_pScrollBarEx;

public:
	void CreateSubControl();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

	const DWORD GetUnionMemberID();
	const CString GetUnionMemberName() const { return m_strSelectName; }

	BOOL IsSelectedIndex();
	void LoadUnionMemberList();
};