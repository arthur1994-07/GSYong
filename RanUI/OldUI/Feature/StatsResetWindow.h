#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../../RanLogic/Item/GLItemDef.h"
//#include "../../../RanLogicClient/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"

class	CInnerInterface;
class	CD3DFontPar;
class	CBasicProgressBar;
class	CBasicTextBox;
class   CBasicTextButton;
class	CBasicButton;
class	CBasicLineBox;
class   CStatsResetComboBoxRollOver;
class	CStatsInfo;
class	GLGaeaClient;
struct  SCHARSTATS;
struct	GLCHARLOGIC;
class GLGaeaClient;

class CStatsResetWindow : public CUIWindowEx
{
protected:
	enum
	{
		STATSRESET_POW_BUTTON = ET_CONTROL_NEXT,
		STATSRESET_DEX_BUTTON,
		STATSRESET_SPI_BUTTON,
		STATSRESET_INT_BUTTON,
		STATSRESET_STR_BUTTON,
		STATSRESET_STA_BUTTON,

		STATSRESET_STATSINFO_DEFAULT,
		STATSRESET_STATSINFO_PREVIEW,

		STATSRESET_ITEM_COMBO_ROLLOVER,

		STATSRESET_INFO_HEAD_TEXTBOX,
		STATSRESET_INFO_TAIL_TEXTBOX,

		STATSRESET_BUTTON_OK,
		STATSRESET_BUTTON_CANCEL,
		STATSRESET_BUTTON_RESET,
	};

public:
	enum EMINFOTEXTHEAD
	{
		EMINFOTEXTHEAD_NONE = 0,

		EMINFOTEXTHEAD_STATS_SELECTABLE	   = 0,
		EMINFOTEXTHEAD_STATS_NONSELECTABLE = 1,
		EMINFOTEXTHEAD_STATS_ALLSTATS	   = 2,
		EMINFOTEXTHEAD_SKILL_ONLY		   = 3,
	};

	enum EMINFOTEXTTAIL
	{
		EMINFOTEXTTAIL_NONE = 0,

		EMINFOTEXTTAIL_CONFIRMATION			= 0,
		EMINFOTEXTTAIL_RESETSTATS_REMAINED  = 1,
	};

public:
    CStatsResetWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CStatsResetWindow ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual void SetGlobalPos(const D3DXVECTOR2& vPos);

public:
	void	CreateSubControl ();
	void	Close ();

public :
	void	ResetProperty(const WORD wReturnStats, const UI::EMSTATSRESETWINDOW_TYPE emType, const EMITEM_TYPE emItemType);
	void	ResetStatsControl    ();
	void	ResetSkillControl    ();
	void	ResetTextControl	 ();

public :
	void	Recalculate			 ();

public :
	void	CalBasicStats ( SCHARSTATS& sOutStats );
	void	CalAddedStats ( SCHARSTATS& sOutStats );

public :
	void	ReqResetSkillStats ();

private:
	CBasicTextBox*	CreateStaticControl     (const char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CBasicLineBox*	CreateBackgroundControl ( char* szConatrolKeyword );

private:
	void	SetPointControl        ( const GLCHARLOGIC& sCharData );
	void	SetVariableTextControl ();

private:
	//	현재 스텟
	CStatsInfo*					 m_pStats;

	CUIControl*					 m_pPointDisable;
	CUIControl*					 m_pPointEnable;
	CBasicTextBox*				 m_pPointText;	

	// 반환 스텟
	CStatsInfo*					 m_pStats_Preview;

	CUIControl*					 m_pPointDisable_Preview;
	CUIControl*					 m_pPointEnable_Preview;
	CBasicTextBox*				 m_pPointText_Preview;

	// 반환 아이템
	CStatsResetComboBoxRollOver* m_pReturn_Item_ComboBox;

	// 반환 값
	CBasicTextBox*				 m_pReturn_Stats_Text;
	CBasicTextBox*				 m_pReturn_Skill_Text;
	CBasicTextBox*				 m_pReturn_Item_Text;
//  CBasicTextBox*               m_pReq_Inven_Text;

	// 가변 정보 텍스트 박스
	CBasicTextBox*				 m_pInfo_Head_Text;
	CBasicTextBox*				 m_pInfo_Tail_Text;

	// 항목 텍스트
	CBasicTextBox*				 m_pTopic_Stats_Current;
	CBasicTextBox*				 m_pTopic_Stats_Preview;
	CBasicTextBox*				 m_pTopic_Item_Return;


	// 버튼
	CBasicTextButton*			 m_pReset_Button;
//  CBasicTextButton*            m_pOK_Button;


private:
	static const int nSTATS_MAX = 6;
	WORD					m_wResetStats[nSTATS_MAX];
	WORD					m_wReturnStatsMax;
	WORD					m_wReturnStats;
	WORD					m_wReturnSkillPoint;
	WORD					m_wReturnItem;
//  WORD                    m_wRemainInven;
//  WORD                    m_wReqInven;
	EMITEM_TYPE				m_emItemType;

	UI::EMSTATSRESETWINDOW_TYPE m_emType;
	EMINFOTEXTHEAD			m_emInfoTextHead;
	EMINFOTEXTTAIL			m_emInfoTextTail;

protected:
	GLGaeaClient*			m_pGaeaClient;

};
