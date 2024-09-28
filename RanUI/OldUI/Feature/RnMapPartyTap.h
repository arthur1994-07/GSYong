

#pragma once


#include "../Util/UIGroupHelper.h"
#include "../../../RanLogic/Party/GLPartyDefine.h"

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;

class CBasicScrollBarEx;
class RnEditBox;
class RnButton;

namespace RnMapPrivate
{
	class IMapInterface;
}

class RnMapPartyTap : public CUIGroupHelper
{
	enum
	{
		PARTY_AD_LIST = NO_ID + 1,
		PARTY_INFO_RECT1,
		PARTY_INFO_RECT2,
		PARTY_INFO_RECT3,
		PARTY_INFO_RECT4,
		PARTY_INFO_RECT5,
		PARTY_INFO_RECT6,
		PARTY_INFO_RECT7,
		PARTY_INFO_RECT8,
		PARTY_INFO_RECT_MAX,

		PARTY_REGISTER_BUTTON,
		PARTY_DELETE_BUTTON,
		PARTY_REFRESH_BUTTON,
		PARTY_INFO_EDITBOX,
	};
public:
	RnMapPartyTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice,RnMapPrivate::IMapInterface* pMapInterface);

	void CreateSubControl();
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void SetVisibleSingle( BOOL bVisible );

	void MsgMapPartyInfoListBoard(bool isExistMyBoard,DWORD mapID,
		const std::vector<std::string>& NameVec,const std::vector<DWORD>& MaxPlayerVec,
		const std::vector<DWORD>& CurrentPlayerVec,const std::vector<std::string>& TitleVec);

	void MsgMapPartyInfoDetail(DWORD mapID, const std::vector<GLPARTY_LINK_MEM>& partyMemberInfoVec);


private:
	void RefreshPartyInfo();
	void UpdatePartyInfoList();

private:
	struct SPartyInfo
	{
		std::string m_Name;
		DWORD m_MaxPlayer;
		DWORD m_CurrentPlayer;
		std::string m_Title;
	};

private:
	static const int PARTY_LIST_MAX = PARTY_INFO_RECT_MAX - PARTY_INFO_RECT1;

	GLGaeaClient*	 m_pGaeaClient;
	CInnerInterface* m_pInterface;
	RnMapPrivate::IMapInterface* m_pMapInterface;

	RnButton*		 m_pRegisterBtn;
	RnButton*		 m_pDeleteBtn;

	DWORD m_CurrentMapID;

	CUIControl*			m_pPartyInfoRect[PARTY_LIST_MAX];
	CBasicTextBox*		m_pPartyInfoTitle[PARTY_LIST_MAX];
	CBasicTextBox*	    m_pPartyInfoNumber[PARTY_LIST_MAX];
	CBasicScrollBarEx*  m_pPartyInfoScroll;

	std::vector<SPartyInfo> m_PartInfoDataVec;

	RnEditBox* m_pEditBox;
};