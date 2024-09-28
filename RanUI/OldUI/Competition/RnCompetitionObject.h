#pragma	once
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../Util/UIWindowObject.h"
#include "../Util/TapSelectorEx.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicLineBoxSmart;
class CBasicTextBox;
class CBasicTextButton;
class InstanceListBox;
class RnCompetitionRewardBuff;
class RnCompetitionRewardItem;
class CSwapImage;

namespace MatchingSystem
{
    class GLMatchingInfo;
}

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_COMPETITION_OBJECT_TAP_CHANGE = UIMSG_WINDOW_OBJECT_NEXT;
////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------//

class RnCompetitionObject : public CUIWindowObject
{

protected:
	enum
	{
		COMPETITION_OBJECT_NONE = WINDOW_OBJECT_NEXT,

		COMPETITION_OBJECT_TAP_CTF,
		COMPETITION_OBJECT_TAP_PVE,

		INFO_BUTTON,
		BUTTON_1,
		BUTTON_2,
		INSTANCE_LIST_BOX,
		CTF_HISTORY_BUFF_BEGIN_1,
		CTF_HISTORY_BUFF_BEGIN_2,
		CTF_HISTORY_BUFF_BEGIN_3,
		CTF_HISTORY_BUFF_BEGIN_4,
		CTF_HISTORY_BUFF_BEGIN_5,
		CTF_HISTORY_BUFF_BEGIN_6,
		RNCOMPETITION_NEXT,
	};

public:
	RnCompetitionObject ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~RnCompetitionObject ();

public:
	virtual void		Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void		TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	virtual void		CreateSubControlEx ();

private:
	CBasicLineBoxSmart* CreateLineBox( const std::string& strControl, const std::string& strTexInfo );
	CBasicTextBox* CreateTextBox( const std::string& strControl, 
									const std::string& strText, 
									int nAlingflag = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, 
									const DWORD TextColor = NS_UITEXTCOLOR::ORANGE, DWORD dwFontSize = 9 );

public:
	CBasicTextButton*	m_pInfoButton;
	CBasicTextButton*	m_pButton1;
	CBasicTextButton*	m_pButton2;
	InstanceListBox*	m_pInstanceListBox;

	CBasicTextBox* m_pMapNameTextBox;
	CBasicTextBox* m_pNextWarTextBox;
	CBasicTextBox* m_pNextWarTimeTextBox;
	CBasicTextBox* m_pRankTextBox;
	CBasicTextBox* m_pInstanceListTextBox;
	CBasicTextBox* m_pObjectTextBox;
	CBasicTextBox* m_pObjectTextInfoBox;
	CBasicTextBox* m_pMoreInfoTextBox;
	CBasicTextBox* m_pMoreInfoInfoTextBox;
	CBasicTextBox* m_pRewardTextBox;
	CBasicTextBox* m_pRewardInfoTextBox;
	CBasicTextBox* m_pStatus;

	std::vector<RnCompetitionRewardBuff*> m_vecRnCompetitionRewardBuff;
	std::vector<RnCompetitionRewardItem*> m_vecRnCompetitionRewardItem;

	CSwapImage*			 m_pMap;

	// 보상 TEXT
	CBasicTextBox*	 m_pBuffText;
	CBasicTextBox*	 m_pItemText;
	CBasicTextBox*	 m_pMoneyText;
	CBasicTextBox*	 m_pExpText;
	CBasicTextBox*	 m_pContributionText;

    DWORD           m_dwSelectID;

    float           m_fInfoButton_Delay; // 정보보기버튼을 연속적으로 누르지못하도록 딜레이를 줌;
	float           m_fButton1_Delay; // 신청버튼을 연속적으로 누르지못하도록 딜레이를 줌;

    bool            m_bInfoButton_Enable;
    bool            m_bButton1_Enable;

public:
	void SetInstanceName(const char* szText);
	void SetNextWar(const char* szText);
	void SetNextWarTime(const char* szText);
	void SetRankText(const char* szText);
	void SetInstanceListText(const char* szText);
	void SetObjectText(const char* szText);
	void SetMoreInfoText(const char* szText);
	void SetMoreInfoInfoText(const char* szText);
	void SetRewardText(const char* szText);
	void SetRewardInfoText(const char* szText);
	void SetInfoButtonText(const char* szText);
	void SetButton1Text(const char* szText);
	void SetButton2Text(const char* szText);
	void SetObjectInfoText(const char* szText);

	//보상
	void SetContributionText(const char* szText);
	void SetExpText(const char* szText);
	void SetMoneyText(const char* szText);
	void SetItemText (const char* szText);
	void SetBuffText(const char* szText);

public:
	void SetVisibleSingleContribution(bool bVisible);
	void SetVisibleSingleExp(bool bVisible);
	void SetVisibleSingleMoney(bool bVisible);
	void SetVisibleSingleItem(bool bVisible);
	void SetVisibleSingleBuff(bool bVisible);
	void SetVisibleSingleButton1(bool bVisible);
	void SetVisibleSingleButton2(bool bVisible);

public:
	void SetInstanceList( DWORD dwInstanceID, const TCHAR* szName );
	void AddInstanceList( const TCHAR* pInstNaem, DWORD dwInstanceID );
	void ClearInstanceList();
	void SetCurSel(int nSelect);
	DWORD GetSelectInstanceID();

    void SelectContents( const DWORD _dwSelectScriptID );
protected:
    void _SelectContentsInfo( const MatchingSystem::GLMatchingInfo* _Info );
    void _UpdateContentsInfo( const MatchingSystem::GLMatchingInfo* _Info );

    void _SetEnableInfoButton(const bool _bEnable);
    void _SetEnableButton1(const bool _bEnable);

protected:
	CInnerInterface*     m_pInterface;
	GLGaeaClient*        m_pGaeaClient;
};
