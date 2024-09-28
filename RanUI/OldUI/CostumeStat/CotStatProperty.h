#pragma once

#include "../../../RanLogic/Item/GLItem.h"
#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CCostumeParts : public CUIGroup
{
public:
	enum {
		CP_SUBCTRL_TEXT_PARTS_TITLE,
		CP_SUBCTRL_TEXT_PARTS_NAME,
		CP_SUBCTRL_MAX
	};
public:
	CCostumeParts( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CCostumeParts();

private :
	GLGaeaClient*    m_pGaeaClient;
	CInnerInterface* m_pInterface;

	CUIGroup* m_pSubCtrl[CP_SUBCTRL_MAX];

	CD3DFontPar*	 m_pFont9;

protected :
	void CreateText( WORD wCtrlNum, const char* szControlName, int nAlign = 0 );

public :
	void CreateSubControl();
	void SetPartsName( const char* pszName );

	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
};

/*************************************************************************************************************************************/

class CCostumeHavePoint : public CUIGroup
{
public:
	enum {
		CP_SUBCTRL_TEXT_POINT_TITLE,
		CP_SUBCTRL_TEXT_POINT_VALUE,
		CP_SUBCTRL_MAX
	};
public:
	CCostumeHavePoint( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CCostumeHavePoint();

private :
	GLGaeaClient*    m_pGaeaClient;
	CInnerInterface* m_pInterface;

	CUIGroup* m_pSubCtrl[CP_SUBCTRL_MAX];

	CD3DFontPar*	 m_pFont10;
	CD3DFontPar*	 m_pFont10Ascii;

	__int64			 m_i64Point;
	__int64			 m_i64SubPoint;

protected :
	void CreateText( WORD wCtrlNum, const char* szControlName, CD3DFontPar* pFont, int nAlign = 0 );
	void SetPointText();

public :
	void CreateSubControl();
	void SetHavePoint( __int64 iPoint = 0 );
	void SetSubPoint( __int64 iPoint = 0 );
	
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
};

/*************************************************************************************************************************************/

class CCostumeStatTitle : public CUIGroup
{
public:
	enum {
		CP_SUBCTRL_TEXT_EFFECT_TITLE,
		CP_SUBCTRL_TEXT_INVEST_POINT,
		CP_SUBCTRL_MAX
	};

public:
	CCostumeStatTitle( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CCostumeStatTitle();

protected :
	void CreateText( WORD wCtrlNum, const char* szControlName, CD3DFontPar* pFont, int nAlign = 0 );

private :
	GLGaeaClient*    m_pGaeaClient;
	CInnerInterface* m_pInterface;

	CUIGroup* m_pSubCtrl[CP_SUBCTRL_MAX];

	CD3DFontPar*	 m_pFont10;

	WORD			 m_wInvestStatMax;
	WORD			 m_wInvestStat;

	int				 m_iInvestPoint;

	std::string		 m_szTitle;

protected:
	void SetInvestStatText();
public :
	void CreateSubControl();
	void SetTextTitle( const char* pszTitle, int iMax, int iInvestStat );
	void SetInvestPoint( int iInvestPt = 0 );
	void IncInvestStatCnt(  );
	void DecInvestStatCnt( );
	void ResetMember();

	int  GetInvestPoint() { return m_iInvestPoint; }

	bool IsFullCnt();

	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
};

/*************************************************************************************************************************************/

class CUIEditBox;
class CBasicTextBox;

class CCostumeStats : public CUIGroup
{
public:

	enum {
		MSG_UPDATE_POINT = 0xffffff01
	};

	enum E_STAT_TYPE
	{
		E_STAT_NONE_TYPE,
		E_STAT_GENERAL_TYPE,
		E_STAT_VARRATE_TYPE,
		E_STAT_ADDED_TYPE
	};

	static E_STAT_TYPE GetRangeToType( COSTUME_STATS eValue );
	
	typedef struct _sStatOne 
	{
		bool           bEnable;
		bool		   bPer;
		bool		   bInvest;

		int			   iPoint;

		std::string    szStateName;

		CBasicTextBox* cpTxtBoxStatname;
		CBasicTextBox* cpTxtBoxNumeric;
		CUIEditBox*    cpEditInvestPt;
		CBasicTextBox* cpTxtBoxEnablePt;

		CBasicLineBoxSmart* cpEditBGEnable;
		CBasicLineBoxSmart* cpEditBGDisable;

		_sStatOne() : cpTxtBoxStatname( NULL ), cpTxtBoxNumeric( NULL ), cpEditInvestPt( NULL ), cpTxtBoxEnablePt( NULL ), cpEditBGEnable( NULL ), cpEditBGDisable( NULL ), bInvest( 0 ), iPoint( 0 )
		{  bPer = false; bEnable = true; }
		~_sStatOne(){}

		void ControlMove(const D3DXVECTOR2& vPos );
		void RegistControl( CUIGroup* pParent );
		void EditEnable();
		void EditDisable();
		void SetPerType() { bPer = true; }
		void ResetNeedMember();

		int  GetEditValue();


	}sStatLine;

public:
	CCostumeStats( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice, E_STAT_TYPE eStatType = E_STAT_NONE_TYPE );
	virtual	~CCostumeStats();

private :
	CCostumeStatTitle* m_pTitle;
	GLGaeaClient*      m_pGaeaClient;
	CInnerInterface*   m_pInterface;

	E_STAT_TYPE		   m_eStatType;

	int				   m_iStatTotal;

	// 불편한 코드.........
	sStatLine*		   m_pStats;
	CD3DFontPar*	   m_pFont10;

	EMSUIT			   m_eParts;

private:
	void CreateStatLine( const char** pszStatName );
	void TranslateEditBox( UIGUID ControlID, DWORD dwMsg );

	void SetIncPoint( int iIdx, double dPt = 0.f );
	void SetMaxPerInvestPt( int iIdx, WORD wMax = 0, int iInvPt = 0);
	void SetTitleColor( int iIdx, int iPt = 0 );
	void UpdateText( int iIdx, int iValue, COSTUME_STATS eStat );

	int  GetInvestTotalPoint();

	COSTUME_STATS GetArrayIdxToCs( int iNum );


public :
	void CreateSubControl(const char* pCtlName );

	CCostumeStatTitle*	GetTitleText() { return m_pTitle; }

	void SetPerType( int iStatIdx );
	void SetParts( EMSUIT eSuit ) { m_eParts = eSuit; }
	void SetDefaultInfo( EMSUIT eParts );

	void ResetSetting();
	void ResetAll();

	bool IsEmptySlot( COSTUME_STATS eStat );
	int  GetInvestPt();

	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
};

#define GET_TITLE(Ptr) ( Ptr != NULL ) ? Ptr->GetTitleText() : NULL;

/*************************************************************************************************************************************/

class CCostumeStatExplain : public CUIGroup
{
public :
	enum {
		CP_SUBCTRL_TEXT_EXPLAIN,
		CP_SUBCTRL_MAX
	};

public:
	CCostumeStatExplain( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CCostumeStatExplain();

private :
	GLGaeaClient*    m_pGaeaClient;
	CInnerInterface* m_pInterface;

	CUIGroup* m_pSubCtrl[CP_SUBCTRL_MAX];

	CD3DFontPar*	 m_pFont10;

protected :
	void CreateText( WORD wCtrlNum, const char* szControlName, CD3DFontPar* pFont, int nAlign = 0 );

public :
	void CreateSubControl();

	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
};