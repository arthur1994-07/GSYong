#pragma	once


#include "../Util/UIGroupHelper.h"

#include "../Util/TapSelectorEx.h"

class RnCharacterWearWindow;

class RnStatInfoTap;
class RnHistoryInfoTap;

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;

class CBasicVarTextBox;

struct GLCHARLOGIC;

class CSmartComboBoxRollOver;

class CSwapImage;

class RnCharacterInfoTap : public CUIGroupHelper, private CTapSelectorEx<DEFAULT_RADIO_BUTTON>
{
public:
	RnCharacterInfoTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	void CreateSubControl();

	void DeleteUIMeshData();

	void UpdateTitleData();

	void SetArmSwapTabButton();
	//////////////////////////////////////////////////////////////////////////
	

	virtual void SetVisibleSingle(BOOL isVisible);
private:
	enum
	{
		STAT_RADIO=1, 
		HISTORY_RADIO,
		BRIGHT_FRAME,
		BRIGHT_SLIDER,
		TITLE_BUTTON,
		TITLE_REGION,
		TITLE_COMBO_ROLLOVER,
		ACADEMY_QUESTION_BUTTON,
		NAME_COUNTRY_MARK,
	};

	CUIGroup* GetTapWindow(UIGUID controlID);
	virtual void EventSelectedTap(UIGUID controlID);


private:
//	void UpdateCharacterView();

	void SetTextTitleCombo( DWORD dwIndex );
	void UpdateTitlePos();

	//////////////////////////////////////////////////////////////////////////
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
private:
	void SetName( const GLCHARLOGIC& sCharData );
	void SetClubName();
	void SetBadge( const GLCHARLOGIC& sCharData );

	//////////////////////////////////////////////////////////////////////////
	void SetAcademyDepartment(const GLCHARLOGIC& sCharData);
	void SetBright( const GLCHARLOGIC& sCharData );
	void SetActivityPoint( const GLCHARLOGIC& sCharData );
	void SetContributionPoint ( const GLCHARLOGIC& sCharData );	

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
private:

	GLGaeaClient*    m_pGaeaClient;
	CInnerInterface* m_pInterface;


	RnCharacterWearWindow* m_pCharacterWearWindow;

	RnStatInfoTap*		 m_pStatTap;
	RnHistoryInfoTap*	 m_pHistoryTap;
	CUIGroup*			 m_pCurrentTap;


	//////////////////////////////////////////////////////////////////////////
	CSwapImage*				m_pCountryMark;
	CBasicTextBox*			m_pCharName;
	CBasicTextBox*			m_pClubName;
	CBasicTextBox*			m_pTitleName;
	CBasicButton*           m_pTitleButton;

	CSmartComboBoxRollOver* m_pComboBoxTitleRollOver;


	//////////////////////////////////////////////////////////////////////////

	
	//////////////////////////////////////////////////////////////////////////


	// 스크롤바 컨트롤로 하는게 좋을거 같은데 -_-; 귀찬군 ,그냥 기존거에서 가져온다
	// 이 스크롤 바는 왠지 여기서만 쓰이는거 같아  또다시 반복해서 쓰인다면
	// 클래스화 시킨다
	CBasicTextBox*	m_pAcademyDepartment;
	CBasicTextBox*	m_pActivityPointText;
	CBasicTextBox*	m_pActivityPointValue;

	CBasicTextBox*  m_pContributionValue;
	CBasicTextBox*	m_pCheckText;


	CUIControl*		m_pBrightFrame;
	CUIControl*		m_pBrightSlider;
};
