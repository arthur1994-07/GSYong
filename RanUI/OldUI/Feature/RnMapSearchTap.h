


#pragma once


#include "../Util/UIGroupHelper.h"

#include "RnMapHelper.h"

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;

class CCheckBox;

class RnEditBox;
class CBasicScrollBarEx; 

namespace RnMapPrivate
{
	class IMapInterface;
}

class RnMapSearchTap : public CUIGroupHelper , public RnMapPrivate::IMapSearchInterface
{


	enum 
	{
		SEARCH_BUTTON = 10,
		SEARCH_EDITBOX,
		CURRENT_REGION_CHECKBOX,
		CACHE_BUTTON,

		SEARCH_NAME_RECT1,
		SEARCH_NAME_RECT2,
		SEARCH_NAME_RECT3,
		SEARCH_NAME_RECT4,
		SEARCH_NAME_RECT5,
		SEARCH_NAME_RECT6,
		SEARCH_NAME_RECT7,
		SEARCH_NAME_RECT8,
		SEARCH_NAME_RECT9,
		SEARCH_NAME_RECT10,
		SEARCH_NAME_RECT11,
		SEARCH_NAME_RECT12,
		SEARCH_NAME_RECT13,
		SEARCH_NAME_RECT_MAX,
		SEARCH_NAME_LIST_SCROLLBAR,
		SHOW_MAP_POSITION_BUTTON,
		WHISPER_BUTTON,
		REGISTER_FRIEND_BUTTON,
		CHARACTER_VIEW_BUTTON,
		PARTY_BUTTON,
		INVITE_CLUB_BUTTON,

	};
public:
	RnMapSearchTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice,
		RnMapPrivate::IMapInterface* pMapInterface);

	void CreateSubControl();

	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

    void MsgMapSearchNameList( const std::vector<std::pair<int, std::string> >& nameList );
	void MsgMapSearchNameDetailInfo(int level,const std::string& charName,const std::string& title,int school,int emClass,
		bool isParty,const std::string& clubName,DWORD dwGaeaID);

	void MsgMapSearchNamePosition(int level,const std::string& charName,DWORD mapID,const D3DXVECTOR3& pos);


private:

	void SendSearchMsg();
	void UpdateNameList();


	std::string GetDetailInfoName();
private:
	virtual bool GetSearchedCharacter(int* pLevel,std::string* pName,DWORD* pMapID,D3DXVECTOR3* pPos);


private:
	GLGaeaClient*    m_pGaeaClient;
	CInnerInterface* m_pInterface;
	RnMapPrivate::IMapInterface* m_pMapInterface;

	RnEditBox*  m_pEditBox;
	static const int MAX_SEARCH_NAME = SEARCH_NAME_RECT_MAX - SEARCH_NAME_RECT1;
	CBasicTextBox* m_pSearchName[MAX_SEARCH_NAME];
	CUIControl* m_pSearchNameRect[MAX_SEARCH_NAME];

	CBasicScrollBarEx*  m_pNameListScrollBar;

	CBasicTextBox* m_pInfoSchool;
	CBasicTextBox* m_pInfoName;
	CBasicTextBox* m_pInfoBadge;
	CBasicTextBox* m_pInfoClass;
	CBasicTextBox* m_pInfoParty;
	CBasicTextBox* m_pInfoClub;

	DWORD		   m_dwDetailInfoGaeaID;

	CCheckBox* m_pCurrentRegionCheckBox;

    //! int 캐릭터 레벨
    //! std::string 캐릭터 이름
    std::vector<std::pair<int, std::string> > m_NameList;;

	int			m_SearchCharacerLevel;
	std::string m_SearchCharacerName;
	DWORD		m_SearchCharacerMapID;
	D3DXVECTOR3 m_SearchCharacerPosition;

};







