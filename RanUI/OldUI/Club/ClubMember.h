#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Club/GLClubDefine.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_ADDNAME_TO_CHATBOX_FROM_CLUBWINDOW = UIMSG_USER5;
////////////////////////////////////////////////////////////////////

class CInnerInterface;
class CBasicTextBoxEx;
class CBasicScrollBarEx;
class GLGaeaClient;

class CClubMember : public CUIGroup
{
protected:
	enum
	{
		CLUB_LIST_TEXTBOX = NO_ID + 1,
		CLUB_LIST_SCROLLBAR
	};

public:
	CClubMember(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CClubMember();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

private:
    int m_nSelectIndex;
    CString	m_strSelectName;
    GLCLUBMEMBER m_sClubMember;

    CBasicTextBoxEx*	m_pListText;
    CBasicScrollBarEx*	m_pListScrollBar;
   
public:
	void CreateSubControl();

	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

	const DWORD GetClubMemberID();
	const CString GetClubMemberName() const	{ return m_strSelectName; }

	void LoadClubMemberList();
	BOOL IsSelectedIndex();	
	void PartyRecruit();

private:
	void LoadClubMember( const CString& strText, const bool bOnline, const DWORD& dwGaeaID, const bool bCDM );
};
