#pragma once

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../Interface/BasicTextBoxExLink.h"
#include "../Interface/UIEditBoxLink.h"

#include "../../InnerInterface.h"

#include "../Util/UIWindowEx.h"
#include "../Util/TapSelectorEx.h"

#include "../../../RanLogic/Club/GLClubDefine.h"

class FriendTab;
class ClubTab;
class	CBasicTextBoxEx;
class	CBasicScrollBarEx;
class   RnButton;

class ChatGroupInviteWindow : public CUIWindowEx, private CTapSelectorEx<DEFAULT_TAPBUTTON>
{
private:
	typedef CTapSelectorEx::BUTTON STAPBUTTON;
	enum 
	{
		ID_RNFRIEND_TAB = ET_CONTROL_NEXT + 1,

		ID_FRIEND_TAB,
		ID_CLUB_TAB,

		ID_FRIEND_PAGE,
		ID_CLUB_PAGE,

		ID_NAME_LIST_BOX,
		ID_NAME_LIST_SCROLLBAR,
		ID__INVITE_BUTTON,
		ID_EDIT_BOX,
	};

	enum EMPAGE
	{
		FRIEND_PAGE,
		CLUB_PAGE,
	};


public:
	ChatGroupInviteWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
	void	            CreateSubControl();
	void				EDIT_BEGIN ( const CString& strMessage = "" );
	virtual void        SetVisibleSingle( BOOL bVisible );


protected:
	virtual void        TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	GLGaeaClient*       m_pGaeaClient;
	CInnerInterface*    m_pInterface;
	CUIGroup*           m_pCurrentPage;

private:
	CUIGroup*           GetTapWindow(UIGUID controlID);
	void                EventSelectedTap( UIGUID controlID );
	virtual void        Refresh();
	void				LoadFriend();
	void				LoadClubMember();
	void				SetScroolBarUpDate(CBasicTextBoxEx* pBasicTextBoxEx);

	CUIGroup*			 m_pFriendPage;
	CUIGroup*			 m_pClubPage;

	CUIControl*			 m_pNameBoxBack;
	CBasicTextBoxExLink* m_pNameListBox;
	CBasicScrollBarEx*	 m_pNameListScrollBar;

	RnButton*            m_pInviteButton;
	EMPAGE				 m_PageSelect;

	CUIControl*			 m_pEditBoxBack;
	CUIEditBoxLink*		 m_pEditBox;

	CString				 m_strSelectName;
	DWORD				 m_dwInviteDbnum;
	BOOL				 m_bDbNumRegWaite;
	float				 m_fWaiteTime;

	std::vector<club::GLCLUBMEMBER_CLIENT> m_vecMembers;
	club::ALIGN_ORDER_TYPE m_OrderType;
	club::ALIGN_ORDER_DES_ASC m_OrderASC_DSC;
};


class MyChatGroupInviteWindow : public IChatGroupInviteWindow, private ChatGroupInviteWindow
{
public:
	MyChatGroupInviteWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
		: ChatGroupInviteWindow( pGaeaClient, pInterface, pEngineDevice )
	{

	}
	virtual ~MyChatGroupInviteWindow()   {};

public:
	virtual void CreateUIWindowAndRegisterOwnership();
	virtual void AddFriendName_To_EditBox( const UI::String& strName );
	virtual void AddFriend( const UI::String& strName );
	virtual void LoadFriendList();
	virtual UI::String GetFriendName() const;
	virtual UI::String GetSelectBlockFriendName() const;
	virtual void EndEditFriendList();
	virtual std::tr1::shared_ptr<SFRIEND> GetFriendSMSInfo();
};