#include "StdAfx.h"

#include "../../InnerInterface.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UIWindow.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxEx.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBox.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBoxSmart.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"

#include "../Util/UIWindowEx.h"
#include "../Util/UIGroupHelper.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/Item/GLItemCostumeAttach.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/CostumeStat/CotStatClient.h"


#include "CotStatProperty.h"

/*
#define COLOR_TXT(num, color) if(num > 0) { color = NS_UITEXTUTIL::ORENGE; } \
							  else if(num < 0){ color = NS_UITEXTUTIL::LIGHTGREY; } \
							  else { color = NS_UITEXTUTIL::WHITE; }
*/

#define COLOR_TXT(num, color) if(num > 0) { color = NS_UITEXTUTIL::ORENGE; } \
							  else if(num < 0){ color = D3DCOLOR_ARGB(0xFF,0x87,0x87,0x87); } \
							  else { color = NS_UITEXTUTIL::WHITE; }


CCostumeParts::CCostumeParts( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup( pEngineDevice ), m_pGaeaClient( pGaeaClient ), m_pInterface( pInterface ), m_pFont9( NULL )
{
	INIT_ARRAY_NULL( m_pSubCtrl, CP_SUBCTRL_MAX );
}

CCostumeParts::~CCostumeParts( )
{

}

void CCostumeParts::CreateText( WORD wCtrlNum, const char* szControlName, int nAlign )
{
	if( wCtrlNum >= CP_SUBCTRL_MAX ) return;

	m_pSubCtrl[wCtrlNum] = new CBasicTextBox(m_pEngineDevice);
	m_pSubCtrl[wCtrlNum]->CreateSub(this, szControlName, UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pSubCtrl[wCtrlNum]->SetFont(m_pFont9);
	DCAST( CBasicTextBox, m_pSubCtrl[wCtrlNum] )->SetTextAlign( nAlign );

	RegisterControl ( m_pSubCtrl[wCtrlNum] );
}

void CCostumeParts::CreateSubControl() 
{	
	CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
	pLinBoxSmart->CreateSub( this, "COSTUME_PARTS_NAME", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLinBoxSmart->CreateSubControl( "COSTUME_STAT_BODY_BACK_IMAGE" );
	RegisterControl( pLinBoxSmart );

	CBasicLineBoxEx* pBasicLineBox = new CBasicLineBoxEx(m_pEngineDevice);   
	pBasicLineBox->CreateSub ( this, "COSTUME_LINE_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseCostumeTitleBox( "COSTUME_PARTS_NAME" );
	RegisterControl ( pBasicLineBox );	

	m_pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_BOLD_FLAG | D3DFONT_SHADOW );

	CreateText( CP_SUBCTRL_TEXT_PARTS_TITLE, "COSTUME_STAT_PARTS_TITLE", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
	CreateText( CP_SUBCTRL_TEXT_PARTS_NAME , "COSTUME_STAT_PARTS_NAME" , TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );

	if( m_pSubCtrl[CP_SUBCTRL_TEXT_PARTS_TITLE] )
	{
		DCAST( CBasicTextBox, m_pSubCtrl[CP_SUBCTRL_TEXT_PARTS_TITLE])->SetText(ID2GAMEWORD("COSTUME_STAT_ASSIGN", 4), NS_UITEXTUTIL::WHITE );
	}

	if( m_pSubCtrl[CP_SUBCTRL_TEXT_PARTS_NAME] )
	{
		DCAST( CBasicTextBox, m_pSubCtrl[CP_SUBCTRL_TEXT_PARTS_NAME])->SetText(" ", NS_UITEXTUTIL::ORENGE );
	}
}

void CCostumeParts::SetPartsName( const char* pszName )
{
	if( m_pSubCtrl[CP_SUBCTRL_TEXT_PARTS_NAME] )
	{
		DCAST( CBasicTextBox, m_pSubCtrl[CP_SUBCTRL_TEXT_PARTS_NAME])->SetText(pszName, NS_UITEXTUTIL::ORENGE );
	}
}

void CCostumeParts::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CCostumeParts::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CCostumeParts::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle(bVisible);
}

/*****************************************************************************************************************************************/

CCostumeHavePoint::CCostumeHavePoint( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup( pEngineDevice ), m_pGaeaClient( pGaeaClient ), m_pInterface( pInterface ), m_pFont10( NULL ), m_pFont10Ascii( NULL ), m_i64Point( 0 ), m_i64SubPoint( 0 )
{
	INIT_ARRAY_NULL( m_pSubCtrl, CP_SUBCTRL_MAX );
}

CCostumeHavePoint::~CCostumeHavePoint( )
{

}

void CCostumeHavePoint::CreateText( WORD wCtrlNum, const char* szControlName, CD3DFontPar* pFont, int nAlign )
{
	if( wCtrlNum >= CP_SUBCTRL_MAX ) return;

	m_pSubCtrl[wCtrlNum] = new CBasicTextBox(m_pEngineDevice);
	m_pSubCtrl[wCtrlNum]->CreateSub(this, szControlName, UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pSubCtrl[wCtrlNum]->SetFont(pFont);
	DCAST( CBasicTextBox, m_pSubCtrl[wCtrlNum] )->SetTextAlign( nAlign );
	RegisterControl ( m_pSubCtrl[wCtrlNum] );
}

void CCostumeHavePoint::CreateSubControl()
{	
	CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
	pLinBoxSmart->CreateSub( this, "COSTUME_POINT_NAME", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLinBoxSmart->CreateSubControl( "COSTUME_STAT_BODY_BACK_IMAGE" );
	RegisterControl( pLinBoxSmart );

	CBasicLineBoxEx* pBasicLineBox = new CBasicLineBoxEx(m_pEngineDevice);   
	pBasicLineBox->CreateSub ( this, "COSTUME_LINE_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseCostumeTitleBox( "COSTUME_POINT_NAME" );

	RegisterControl ( pBasicLineBox );

	m_pFont10      = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_BOLD_FLAG | D3DFONT_SHADOW );
	m_pFont10Ascii = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, D3DFONT_BOLD | D3DFONT_SHADOW | D3DFONT_ASCII );

	CreateText( CP_SUBCTRL_TEXT_POINT_TITLE, "COSTUME_STAT_PARTS_TITLE", m_pFont10,      TEXT_ALIGN_LEFT  | TEXT_ALIGN_CENTER_Y );
	CreateText( CP_SUBCTRL_TEXT_POINT_VALUE , "COSTUME_STAT_PARTS_NAME" , m_pFont10Ascii, TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );

	if( m_pSubCtrl[CP_SUBCTRL_TEXT_POINT_TITLE] )
	{
		DCAST( CBasicTextBox, m_pSubCtrl[CP_SUBCTRL_TEXT_POINT_TITLE])->SetText(ID2GAMEWORD("COSTUME_STAT_ASSIGN", 5), NS_UITEXTUTIL::WHITE );
	}

	if( m_pSubCtrl[CP_SUBCTRL_TEXT_POINT_VALUE] )
	{
		DCAST( CBasicTextBox, m_pSubCtrl[CP_SUBCTRL_TEXT_POINT_VALUE])->SetText(" ", D3DCOLOR_ARGB(255, 165, 242, 47) );
	}
}

void CCostumeHavePoint::SetPointText()
{
	if( m_pSubCtrl[CP_SUBCTRL_TEXT_POINT_VALUE] )
	{
		CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( m_i64Point - m_i64SubPoint, 3, "," );

		DCAST( CBasicTextBox, m_pSubCtrl[CP_SUBCTRL_TEXT_POINT_VALUE])->SetText(strText, D3DCOLOR_ARGB(255, 165, 242, 47) );
	}
}

void CCostumeHavePoint::SetHavePoint( __int64 iPoint )
{
	m_i64Point    = iPoint;
	m_i64SubPoint = 0;

	SetPointText();
}

void CCostumeHavePoint::SetSubPoint( __int64 iPoint )
{
	m_i64SubPoint = iPoint;

	if( m_i64Point < m_i64SubPoint )
		m_i64SubPoint = m_i64Point;

	SetPointText();
}

void CCostumeHavePoint::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( IsVisible() && m_pGaeaClient )
	{		
	}
}

void CCostumeHavePoint::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CCostumeHavePoint::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle(bVisible);
}

/*****************************************************************************************************************************************/

CCostumeStatTitle::CCostumeStatTitle( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup( pEngineDevice ), m_pGaeaClient( pGaeaClient ), m_pInterface( pInterface ), m_pFont10( NULL ), m_wInvestStatMax( 0 ), m_wInvestStat( 0 ), m_iInvestPoint ( 0 )
{
	INIT_ARRAY_NULL( m_pSubCtrl, CP_SUBCTRL_MAX );
}

CCostumeStatTitle::~CCostumeStatTitle( )
{

}

void CCostumeStatTitle::CreateText( WORD wCtrlNum, const char* szControlName, CD3DFontPar* pFont, int nAlign )
{
	if( wCtrlNum >= CP_SUBCTRL_MAX ) return;

	m_pSubCtrl[wCtrlNum] = new CBasicTextBox(m_pEngineDevice);
	m_pSubCtrl[wCtrlNum]->CreateSub(this, szControlName, UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pSubCtrl[wCtrlNum]->SetFont(pFont);
	DCAST( CBasicTextBox, m_pSubCtrl[wCtrlNum] )->SetTextAlign( nAlign );
	RegisterControl ( m_pSubCtrl[wCtrlNum] );
}

void CCostumeStatTitle::CreateSubControl()
{	
	CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
	pLinBoxSmart->CreateSub( this, "COSTUME_STAT_NAME", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLinBoxSmart->CreateSubControl( "COSTUME_STAT_BODY_BACK_IMAGE" );
	RegisterControl( pLinBoxSmart );

	CBasicLineBoxEx* pBasicLineBox = new CBasicLineBoxEx(m_pEngineDevice);   
	pBasicLineBox->CreateSub ( this, "COSTUME_LINE_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseCostumeTitleBox( "COSTUME_STAT_NAME" );

	RegisterControl ( pBasicLineBox );

	m_pFont10      = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_BOLD_FLAG | D3DFONT_SHADOW );

	CreateText( CP_SUBCTRL_TEXT_EFFECT_TITLE, "COSTUME_STAT_EFFECT_TITLE"         , m_pFont10, TEXT_ALIGN_LEFT  | TEXT_ALIGN_CENTER_Y );
	CreateText( CP_SUBCTRL_TEXT_INVEST_POINT , "COSTUME_STAT_EFFECT_INVEST_POINT" , m_pFont10, TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );	
}

void CCostumeStatTitle::SetTextTitle( const char* pszTitle, int iMax, int iInvestStat )
{
	m_wInvestStatMax = iMax;
	m_wInvestStat	 = iInvestStat;

	m_szTitle = pszTitle;

	if( m_pSubCtrl[CP_SUBCTRL_TEXT_EFFECT_TITLE] )
	{
		std::string szText = sc::string::format("%s ( %d/%d )", m_szTitle.c_str(), iInvestStat, iMax);

		DCAST( CBasicTextBox, m_pSubCtrl[CP_SUBCTRL_TEXT_EFFECT_TITLE])->SetText( szText.c_str(), NS_UITEXTUTIL::WHITE );
	}
}

void CCostumeStatTitle::SetInvestStatText()
{
	if( m_pSubCtrl[CP_SUBCTRL_TEXT_EFFECT_TITLE] )
	{
		std::string szText = sc::string::format("%s ( %d/%d )", m_szTitle.c_str(), m_wInvestStat, m_wInvestStatMax);

		DCAST( CBasicTextBox, m_pSubCtrl[CP_SUBCTRL_TEXT_EFFECT_TITLE])->SetText( szText.c_str(), NS_UITEXTUTIL::WHITE );
	}
}

void CCostumeStatTitle::IncInvestStatCnt( )
{
	++m_wInvestStat;
	
	SetInvestStatText();
}

void CCostumeStatTitle::DecInvestStatCnt( )
{
	--m_wInvestStat;

	SetInvestStatText();
}

bool CCostumeStatTitle::IsFullCnt()
{
	if( m_wInvestStat >= m_wInvestStatMax ) return true;

	return false;
}

void CCostumeStatTitle::ResetMember()
{
	m_wInvestStat = 0;

	SetInvestStatText();
	SetInvestPoint(0);
}

void CCostumeStatTitle::SetInvestPoint( int iInvestPt )
{
	m_iInvestPoint = iInvestPt;

	if( m_pSubCtrl[CP_SUBCTRL_TEXT_INVEST_POINT] )
	{		 
		std::string szText;

		D3DCOLOR dwColor = NS_UITEXTUTIL::WHITE;

		if( iInvestPt <= 0 )
		{
			szText = sc::string::format("%d", iInvestPt);
		}
		else
		{
			dwColor = D3DCOLOR_ARGB(255, 165, 242, 47);

			szText = sc::string::format("+%d", iInvestPt);
		}

		DCAST( CBasicTextBox, m_pSubCtrl[CP_SUBCTRL_TEXT_INVEST_POINT])->SetText( szText.c_str(), dwColor );
	}
}

void CCostumeStatTitle::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CCostumeStatTitle::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CCostumeStatTitle::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle(bVisible);
}

/*****************************************************************************************************************************************/

void CCostumeStats::_sStatOne::ControlMove(const D3DXVECTOR2& vPos )
{
	if( cpTxtBoxStatname ) cpTxtBoxStatname->AddPosition( vPos );
	if( cpTxtBoxNumeric  ) cpTxtBoxNumeric->AddPosition( vPos );
	if( cpEditInvestPt   ) cpEditInvestPt->AddPosition( vPos );
	if( cpTxtBoxEnablePt ) cpTxtBoxEnablePt->AddPosition( vPos );
	if( cpEditBGEnable  ) cpEditBGEnable->AddPosition( vPos );
	if( cpEditBGDisable ) cpEditBGDisable->AddPosition( vPos );
}

void CCostumeStats::_sStatOne::RegistControl( CUIGroup* pParent )
{
	if( pParent == NULL ) return;

	if( cpTxtBoxStatname ) pParent->RegisterControl(cpTxtBoxStatname);
	if( cpTxtBoxNumeric  ) pParent->RegisterControl(cpTxtBoxNumeric );
	if( cpTxtBoxEnablePt ) pParent->RegisterControl(cpTxtBoxEnablePt);
	if( cpEditBGEnable   ) pParent->RegisterControl(cpEditBGEnable  );
	if( cpEditBGDisable  ) pParent->RegisterControl(cpEditBGDisable );
	if( cpEditInvestPt   ) pParent->RegisterControl(cpEditInvestPt  );
}

void CCostumeStats::_sStatOne::EditEnable()
{
	cpEditInvestPt->EndEdit();
	cpEditInvestPt->BeginEdit();

	cpEditBGEnable->SetVisibleSingle(TRUE);
	cpEditBGDisable->SetVisibleSingle(FALSE);
}

void CCostumeStats::_sStatOne::EditDisable()
{
	cpEditInvestPt->EndEdit();

	if( !bInvest ) {
		cpEditBGEnable->SetVisibleSingle(FALSE);
		cpEditBGDisable->SetVisibleSingle(TRUE);
	}
}

int CCostumeStats::_sStatOne::GetEditValue()
{
	return atoi( cpEditInvestPt->GetEditString() );
}

void CCostumeStats::_sStatOne::ResetNeedMember()
{
	bEnable = true;

	bInvest = false;

	iPoint  = 0;

	cpEditInvestPt->ClearEdit();

	EditDisable();
}

CCostumeStats::E_STAT_TYPE CCostumeStats::GetRangeToType( COSTUME_STATS eValue  )
{
	if( eValue >= E_EXPMULTIPLE && eValue <= E_EMPTY )
	{
		return E_STAT_GENERAL_TYPE;
	}
	else
	if( eValue >= E_ADD_HITRATE && eValue <= E_ADD_EMPTY3 )
	{
		return E_STAT_ADDED_TYPE;
	}
	else
	if( eValue >= E_R_HP_INC && eValue <= E_R_EMPTY )
	{
		return E_STAT_VARRATE_TYPE;
	}

	return E_STAT_NONE_TYPE;
}

CCostumeStats::CCostumeStats( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice, E_STAT_TYPE eStatType  )
: CUIGroup( pEngineDevice ), m_pGaeaClient( pGaeaClient ), m_pInterface( pInterface ), m_pTitle( NULL ), m_eStatType( eStatType ), m_pStats( NULL ), m_pFont10( NULL ), m_iStatTotal( 0 ),
  m_eParts(SUIT_HEADGEAR)
{
	
}

CCostumeStats::~CCostumeStats( )
{
	SAFE_DELETE_ARRAY( m_pStats );
}

void CCostumeStats::CreateStatLine( const char** pszStatName )
{
	if( m_pStats == NULL || pszStatName == NULL ) return;

	if( m_pFont10 == NULL ) m_pFont10 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG | D3DFONT_SHADOW );

	D3DXVECTOR2 vInterval = D3DXVECTOR2( 0, 0 );

	int iUiControlKey = 0;

	switch( m_eStatType )
	{
	case E_STAT_GENERAL_TYPE : 
		{
			SetPerType( E_EXPMULTIPLE );

			iUiControlKey = E_EXPMULTIPLE; 
		}		
		break;
	case E_STAT_VARRATE_TYPE : 
		{
			SetPerType( E_R_HP_INC );
			SetPerType( E_R_MP_INC );
			SetPerType( E_R_SP_INC );
			SetPerType( E_R_HMSP_INC );
			SetPerType( E_R_MOVESPEED );
			SetPerType( E_R_ATTACKSPEED );
			SetPerType( E_R_CRITICALHIT );
			SetPerType( E_R_STRIKEHIT );
			SetPerType( E_R_EMPTY );

			iUiControlKey = E_R_HP_INC;
		}
		break;
	case E_STAT_ADDED_TYPE   : 
		{		
			iUiControlKey = E_ADD_HITRATE;
		}
		break;
	}

	for( int i = 0; i < m_iStatTotal; ++i )
	{
		m_pStats[i].cpTxtBoxStatname = new CBasicTextBox( m_pEngineDevice );
		m_pStats[i].cpTxtBoxNumeric  = new CBasicTextBox( m_pEngineDevice );
		m_pStats[i].cpEditInvestPt   = new CUIEditBox(m_pEngineDevice);
		m_pStats[i].cpTxtBoxEnablePt = new CBasicTextBox( m_pEngineDevice );
		m_pStats[i].cpEditBGEnable   = new CBasicLineBoxSmart( m_pEngineDevice );
		m_pStats[i].cpEditBGDisable  = new CBasicLineBoxSmart( m_pEngineDevice );

		m_pStats[i].szStateName		 = pszStatName[i];
		m_pStats[i].cpTxtBoxStatname->CreateSub(this, "COSTUME_STAT_NTITLE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pStats[i].cpTxtBoxStatname->SetFont(m_pFont10);
		m_pStats[i].cpTxtBoxStatname->SetText(pszStatName[i], NS_UITEXTUTIL::WHITE );
		m_pStats[i].cpTxtBoxStatname->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );

		m_pStats[i].cpTxtBoxNumeric->CreateSub(this, "COSTUME_STAT_NNUMERIC", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pStats[i].cpTxtBoxNumeric->SetFont(m_pFont10);

		if( m_pStats[i].bPer )
			m_pStats[i].cpTxtBoxNumeric->SetText("0.00%", NS_UITEXTUTIL::WHITE );
		else
			m_pStats[i].cpTxtBoxNumeric->SetText("0", NS_UITEXTUTIL::WHITE );

		m_pStats[i].cpTxtBoxNumeric->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );

		m_pStats[i].cpEditInvestPt->CreateSub ( this, "COSTUME_STAT_NEDIT", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, iUiControlKey + i );
		m_pStats[i].cpEditInvestPt->CreateCarrat ( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
		m_pStats[i].cpEditInvestPt->SetFont ( m_pFont10 );
		m_pStats[i].cpEditInvestPt->SetLimitInput ( 5 );
		m_pStats[i].cpEditInvestPt->SET_MONEY_FORMAT( 3, "," );
		m_pStats[i].cpEditInvestPt->SetAlign(EDIT_RIGHT_ALIGN);

		m_pStats[i].cpTxtBoxEnablePt->CreateSub(this, "COSTUME_STAT_NPOINT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pStats[i].cpTxtBoxEnablePt->SetFont(m_pFont10);
		m_pStats[i].cpTxtBoxEnablePt->SetText("0/999", NS_UITEXTUTIL::WHITE );
		m_pStats[i].cpTxtBoxEnablePt->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );

		m_pStats[i].cpEditBGEnable->CreateSub ( this, "COSTUME_STAT_EDIT_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pStats[i].cpEditBGEnable->CreateSubControl( "POSTBOX_PAGE_SEND_LINEBOX_DISABLE_TEXINFO" );

		m_pStats[i].cpEditBGDisable->CreateSub ( this, "COSTUME_STAT_EDIT_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pStats[i].cpEditBGDisable->CreateSubControl( "POSTBOX_PAGE_SEND_LINEBOX_TEXINFO" );

		m_pStats[i].RegistControl(this);

		m_pStats[i].ControlMove(vInterval);

		vInterval.y += 19;
	}
}

void CCostumeStats::CreateSubControl( const char* pCtlName )
{	
	CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
	pLinBoxSmart->CreateSub( this, pCtlName, UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLinBoxSmart->CreateSubControl( "COSTUME_STAT_BODY_BACK_IMAGE" );
	RegisterControl( pLinBoxSmart );


	m_pTitle = new CCostumeStatTitle( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	m_pTitle->CreateSub( this, "COSTUME_STAT_TITLE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pTitle->CreateSubControl();

	RegisterControl ( m_pTitle );
	
	CBasicLineBoxEx* pBasicLineBox = new CBasicLineBoxEx(m_pEngineDevice);   

	switch( m_eStatType )
	{
	case E_STAT_GENERAL_TYPE :
		pBasicLineBox->CreateSub ( this, "COSTUME_STAT_LINE_GENERAL", UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseCostumeTitleBoxII("COSTUME_STAT_LINE_GENERAL_SUB", "COSTUME_WINDOW_PARENT_LINE_GN_BODY", "BASIC_LINE_BOX_EX_LEFT1_ESC", "BASIC_LINE_BOX_EX_RIGHT1_ESC");
		break;
	case E_STAT_VARRATE_TYPE :
		pBasicLineBox->CreateSub ( this, "COSTUME_STAT_LINE_VARRATE", UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseCostumeTitleBoxII("COSTUME_STAT_LINE_VARRATE_SUB", "COSTUME_WINDOW_PARENT_LINE_VAR_BODY", "BASIC_LINE_BOX_EX_LEFT2_ESC", "BASIC_LINE_BOX_EX_RIGHT2_ESC");
		break;
	case E_STAT_ADDED_TYPE   :
		pBasicLineBox->CreateSub ( this, "COSTUME_STAT_LINE_ADDED", UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseCostumeTitleBoxII("COSTUME_STAT_LINE_ADDED_SUB", "COSTUME_WINDOW_PARENT_LINE_ADDED_BODY", "BASIC_LINE_BOX_EX_LEFT3_ESC", "BASIC_LINE_BOX_EX_RIGHT3_ESC");
		break;
	}	

	RegisterControl ( pBasicLineBox );
	

	/************************************************************************************************************************/

	const char* pszTemp1[E_GENERAL_MAX] = {
		ID2GAMEWORD("CHARACTER_STATUS_STATIC", 20), ID2GAMEWORD("CHARACTER_STATUS_STATIC", 3), ID2GAMEWORD("ITEM_INFO_SET_BONUS", 78), "-"
	};

	const char* pszTemp2[E_ADDED_MAX] = {
		ID2GAMEWORD("CHARACTER_STATUS_STATIC", 9 ), ID2GAMEWORD("CHARACTER_STATUS_STATIC", 10), ID2GAMEWORD("CHARACTER_STATUS_STATIC", 2), 
		ID2GAMEWORD("CHARACTER_STATUS_STATIC", 3 ), ID2GAMEWORD("CHARACTER_STATUS_STATIC", 6 ), ID2GAMEWORD("CHARACTER_STAT_STATIC"  , 0), 
		ID2GAMEWORD("CHARACTER_STAT_STATIC"  , 4 ), ID2GAMEWORD("CHARACTER_STAT_STATIC"  , 2 ), ID2GAMEWORD("CHARACTER_STAT_STATIC"  , 1), 
		ID2GAMEWORD("CHARACTER_STATUS_STATIC", 11), ID2GAMEWORD("CHARACTER_STATUS_STATIC", 5 ), ID2GAMEWORD("CHARACTER_STATUS_STATIC", 4),
		"-", "-", "-"
	};

	const char* pszTemp3[E_VARRATE_MAX] = {
		ID2GAMEWORD("CHARACTER_STATUS_STATIC", 21), ID2GAMEWORD("CHARACTER_STATUS_STATIC", 22), ID2GAMEWORD("CHARACTER_STATUS_STATIC", 23), 
		ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", 4), ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", 5), ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", 6), 
		ID2GAMEWORD("PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM", 22), ID2GAMEWORD("PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM", 23), "-"
	};

	const char** pTemp = NULL;

	switch( m_eStatType )
	{
	case E_STAT_GENERAL_TYPE : m_iStatTotal = E_GENERAL_MAX; m_pStats = new sStatLine[E_GENERAL_MAX]; pTemp = pszTemp1; break;
	case E_STAT_VARRATE_TYPE : m_iStatTotal = E_VARRATE_MAX; m_pStats = new sStatLine[E_VARRATE_MAX]; pTemp = pszTemp3; break;
	case E_STAT_ADDED_TYPE   : m_iStatTotal = E_ADDED_MAX;	 m_pStats = new sStatLine[E_ADDED_MAX];   pTemp = pszTemp2; break;
	}
	
	CreateStatLine( pTemp );
}

void CCostumeStats::SetIncPoint( int iIdx, double dPt )
{
	D3DCOLOR dwColor = NS_UITEXTUTIL::WHITE;

	double dTempPt = dPt;

	if( dPt < 0 ) dTempPt = 0;

	COLOR_TXT( dPt, dwColor );

	if( m_pStats[iIdx].bPer )
	{
		CString strCombine;

		if( dPt > 0 ) {
			
			float fResult = static_cast<float>(dTempPt*100);

			strCombine.Format( "+%.2f", (float)fResult );
		}
		else {
			strCombine.Format( "%.2f", (float)dTempPt );
		}
		
		strCombine += "%";

		m_pStats[iIdx].cpTxtBoxNumeric->SetText(strCombine );
		m_pStats[iIdx].cpTxtBoxNumeric->SetUseTextColor(0, TRUE);
		m_pStats[iIdx].cpTxtBoxNumeric->SetTextColor(0, dwColor);
	}
	else
	{
		std::string sTemp;

		if( dPt > 0 ) {
			 sTemp = sc::string::format("+%d", (int)dTempPt);
		}
		else {
			 sTemp = sc::string::format("%d", (int)dTempPt);
		}

		m_pStats[iIdx].cpTxtBoxNumeric->SetText(sTemp.c_str());
		m_pStats[iIdx].cpTxtBoxNumeric->SetUseTextColor(0, TRUE);
		m_pStats[iIdx].cpTxtBoxNumeric->SetTextColor(0, dwColor);
	}
}

void CCostumeStats::SetMaxPerInvestPt( int iIdx, WORD wMax, int iInvPt)
{
	D3DCOLOR dwColor = NS_UITEXTUTIL::WHITE;

	int iTempPt = iInvPt;

	if( iInvPt < 0 ) iTempPt = 0;

	std::string sTemp = sc::string::format("%1%/%2%", iTempPt, wMax);
	
	COLOR_TXT( iInvPt, dwColor );

	m_pStats[iIdx].cpTxtBoxEnablePt->SetText(sTemp.c_str());
	m_pStats[iIdx].cpTxtBoxEnablePt->SetUseTextColor(0, TRUE);
	m_pStats[iIdx].cpTxtBoxEnablePt->SetTextColor(0, dwColor);

	
}



void CCostumeStats::SetTitleColor( int iIdx, int iPt )
{
	D3DCOLOR dwColor = NS_UITEXTUTIL::WHITE;

	int iTempPt = iPt;

	if( iPt < 0 ) iTempPt = 0;

	COLOR_TXT( iPt, dwColor );

	m_pStats[iIdx].cpTxtBoxStatname->SetText(m_pStats[iIdx].szStateName.c_str());
	m_pStats[iIdx].cpTxtBoxStatname->SetUseTextColor(0, TRUE);
	m_pStats[iIdx].cpTxtBoxStatname->SetTextColor(0, dwColor);
}

void CCostumeStats::SetDefaultInfo( EMSUIT eParts )
{	
	m_eParts = eParts;

	const COSTUMESTATSDATA::CPartsDt::mPsData* const pData = COSSDATA.GetStatInfo( eParts );

	if( pData == NULL ) return;

	int iMax = 0;

	for( int i = 0; i < m_iStatTotal; ++i )
	{
		COSTUME_STATS eCs = GetArrayIdxToCs(i);

		switch( eCs )
		{
		case E_EMPTY      :
		case E_ADD_EMPTY1 :
		case E_ADD_EMPTY2 :
		case E_ADD_EMPTY3 :
		case E_R_EMPTY    :
			m_pStats[i].cpTxtBoxStatname->SetText("-", NS_UITEXTUTIL::LIGHTGREY );
			m_pStats[i].cpTxtBoxNumeric->SetText("-", NS_UITEXTUTIL::LIGHTGREY );
			m_pStats[i].cpTxtBoxEnablePt->SetText("-/-", NS_UITEXTUTIL::LIGHTGREY);
			m_pStats[i].EditDisable();
			m_pStats[i].bEnable = false;
			continue;
		}

		COSTUMESTATSDATA::CPartsDt::mPsData::const_iterator _cIter = pData->find(eCs);

		m_pStats[i].ResetNeedMember();
		
		SetTitleColor(i); 
		SetIncPoint(i);

		if( _cIter != pData->end() )
		{			
			iMax = _cIter->second.wMaxPt;

			SetMaxPerInvestPt(i, _cIter->second.wMaxPt );
		}
		else
		{
			iMax = 0;
			SetMaxPerInvestPt(i);			
		}

		if( iMax == 0 )
		{
			m_pStats[i].bEnable = false;

			SetIncPoint(i, -1);
			SetMaxPerInvestPt(i, 0, -1 );
			SetTitleColor(i, -1);
		}
	}

	m_pTitle->ResetMember();
}

void CCostumeStats::ResetSetting()
{
	SetDefaultInfo(m_eParts);

	m_pTitle->ResetMember();
}

void CCostumeStats::ResetAll()
{
	for( int i = 0; i < m_iStatTotal; ++i )
	{
		m_pStats[i].ResetNeedMember();

		SetTitleColor(i); 
		SetIncPoint(i);	
		SetMaxPerInvestPt(i);
	}
}

void CCostumeStats::SetPerType( int iStatIdx )
{
	int iIndex = -1;

	switch( m_eStatType )
	{
	case E_STAT_GENERAL_TYPE : 
		{
			if( iStatIdx < E_EXPMULTIPLE && iStatIdx > E_EMPTY ) return;
			
			ID_STAT((int)iStatIdx, iIndex );

			m_pStats[iIndex].SetPerType();
		}
		break;
	case E_STAT_VARRATE_TYPE : 
		{
			if( iStatIdx < E_R_HP_INC && iStatIdx > E_R_EMPTY ) return;

			ID_STAT((int)iStatIdx, iIndex );

			m_pStats[iIndex].SetPerType();
		}
		break;
	case E_STAT_ADDED_TYPE   : 
		{
			if( iStatIdx < E_ADD_HITRATE && iStatIdx > E_ADD_EMPTY3 ) return;

			ID_STAT((int)iStatIdx, iIndex );

			m_pStats[iIndex].SetPerType();
		}
		break;
	default: return;
	}

	
}

void CCostumeStats::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	
}

void CCostumeStats::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{	
	if( ControlID >= E_EXPMULTIPLE && ControlID <= E_R_EMPTY )
	{
		TranslateEditBox( ControlID, dwMsg );
	}
	else
	{

	}


	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CCostumeStats::TranslateEditBox( UIGUID ControlID, DWORD dwMsg )
{
	if( m_pStats == NULL ) return;
	
	int iIdx = -1;

	ID_STAT((int)ControlID, iIdx );

	switch( ControlID )
	{
	case E_EMPTY      :
	case E_ADD_EMPTY1 :
	case E_ADD_EMPTY2 :
	case E_ADD_EMPTY3 :
	case E_R_EMPTY    :
		m_pStats[iIdx].EditDisable();
		return;
	}

	CCostumeStatClient* csClient = m_pGaeaClient->GetCostumeClient();

	if( csClient == NULL ) return;

	if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
	{
		bool bResult = ( !csClient->IsInvestEnable((COSTUME_STATS)ControlID) || (csClient->GetRemainPoint() == 0) || m_pTitle->IsFullCnt() ) ? true : false;

		if( (!m_pStats[iIdx].bEnable || bResult) && !m_pStats[iIdx].bInvest )
		{			
			m_pStats[iIdx].EditDisable();
		}
		else
		{
			m_pStats[iIdx].EditEnable();
		}
	}
	else 
	if( CHECK_LB_DOWN_LIKE( dwMsg ) )
	{
		m_pStats[iIdx].EditDisable();

		int iValue    = m_pStats[iIdx].GetEditValue();
		int iMaxPoint = COSSDATA.GetStatMaxPt( m_eParts, (COSTUME_STATS)ControlID );

		if( m_pStats[iIdx].iPoint != iValue )
		{
			std::string str = sc::string::format("%1%", m_pStats[iIdx].iPoint);

			m_pStats[iIdx].cpEditInvestPt->SetEditString(str.c_str());
		}
	}
	
	if( m_pStats[iIdx].cpEditInvestPt->IsBegin() )
	{
		int iValue    = m_pStats[iIdx].GetEditValue();
		int iMaxPoint = COSSDATA.GetStatMaxPt( m_eParts, (COSTUME_STATS)ControlID );
		int iRemainPt = iMaxPoint;
		
		iRemainPt = csClient->GetRemainPoint();

		//if( iMaxPoint >= iRemainPt + iValue ) iMaxPoint = iRemainPt + iValue;
		if( iMaxPoint >= iRemainPt + m_pStats[iIdx].iPoint ) iMaxPoint = iRemainPt + m_pStats[iIdx].iPoint;

		if( iValue >= iMaxPoint && iValue > 0 )
		{
			iValue = iMaxPoint;

			std::string str = sc::string::format("%1%", iMaxPoint);

			m_pStats[iIdx].cpEditInvestPt->SetEditString(str.c_str());
		}

		if( iValue > 0 && !m_pStats[iIdx].bInvest )
		{
			m_pStats[iIdx].bInvest = true;

			m_pTitle->IncInvestStatCnt();
		}
		else
		if( iValue == 0 && m_pStats[iIdx].bInvest )
		{
			m_pStats[iIdx].bInvest = false;

			m_pTitle->DecInvestStatCnt();

			csClient->ResetStatPos((COSTUME_STATS)ControlID);
		}
 
		if( iValue != m_pStats[iIdx].iPoint ) 
		{
			csClient->SetNumeric((COSTUME_STATS)ControlID, iValue);

			m_pStats[iIdx].iPoint = iValue;
			
			m_pTitle->SetInvestPoint(GetInvestTotalPoint());
			
			UpdateText( iIdx, iValue, (COSTUME_STATS)ControlID );
			
			if( GetParent() ) GetParent()->TranslateUIMessage(GetWndID(), MSG_UPDATE_POINT);
		}			

		if( iValue == 0 && !m_pStats[iIdx].bInvest )
		{
			m_pStats[iIdx].cpEditInvestPt->SetEditString("");
		}

	}	
	else {

		int iCheckValue = ( m_pTitle->IsFullCnt() || csClient->GetRemainPoint() == 0 || !csClient->IsInvestEnable((COSTUME_STATS)ControlID) ) ? -1 : 0;

		if( !m_pStats[iIdx].bInvest && !IsEmptySlot((COSTUME_STATS)ControlID) && m_pStats[iIdx].bEnable ){
			UpdateText( iIdx, iCheckValue, (COSTUME_STATS)ControlID );
		}		
	}
}

void CCostumeStats::UpdateText( int iIdx, int iValue, COSTUME_STATS eStat )
{
	SetIncPoint(iIdx, iValue*COSSDATA.GetStatInc( m_eParts, eStat ));
	SetMaxPerInvestPt(iIdx, COSSDATA.GetStatMaxPt( m_eParts, eStat ), iValue );
	SetTitleColor(iIdx, iValue);
}

bool CCostumeStats::IsEmptySlot( COSTUME_STATS eStat )
{
	switch( eStat )
	{
	case E_EMPTY      :
	case E_ADD_EMPTY1 :
	case E_ADD_EMPTY2 :
	case E_ADD_EMPTY3 :
	case E_R_EMPTY    :
		return true;
	}

	return false;
}

int CCostumeStats::GetInvestPt()
{
	return m_pTitle->GetInvestPoint();
}

int CCostumeStats::GetInvestTotalPoint()
{
	int iPoint = 0;

	for( int i = 0; i < m_iStatTotal; ++i )
	{
		iPoint += m_pStats[i].iPoint;
	}

	return iPoint;
}

COSTUME_STATS CCostumeStats::GetArrayIdxToCs( int iNum )
{
	switch(m_eStatType)
	{
	case E_STAT_GENERAL_TYPE : return (COSTUME_STATS)(iNum + E_EXPMULTIPLE);
	case E_STAT_VARRATE_TYPE : return (COSTUME_STATS)(iNum + E_R_HP_INC);
	case E_STAT_ADDED_TYPE   : return (COSTUME_STATS)(iNum + E_ADD_HITRATE);

	}

	return E_NONE;
}

void CCostumeStats::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle(bVisible);
}

/*****************************************************************************************************************************************/

CCostumeStatExplain::CCostumeStatExplain( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup( pEngineDevice ), m_pGaeaClient( pGaeaClient ), m_pInterface( pInterface ), m_pFont10( NULL )
{
	INIT_ARRAY_NULL( m_pSubCtrl, CP_SUBCTRL_MAX );
}

CCostumeStatExplain::~CCostumeStatExplain( )
{

}

void CCostumeStatExplain::CreateText( WORD wCtrlNum, const char* szControlName, CD3DFontPar* pFont, int nAlign )
{
	if( wCtrlNum >= CP_SUBCTRL_MAX ) return;

	m_pSubCtrl[wCtrlNum] = new CBasicTextBox(m_pEngineDevice);
	m_pSubCtrl[wCtrlNum]->CreateSub(this, szControlName, UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pSubCtrl[wCtrlNum]->SetFont(pFont);
	DCAST( CBasicTextBox, m_pSubCtrl[wCtrlNum] )->SetTextAlign( nAlign );
	RegisterControl ( m_pSubCtrl[wCtrlNum] );
}

void CCostumeStatExplain::CreateSubControl()
{	
	CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
	pLinBoxSmart->CreateSub( this, "COSTUME_STAT_BOX_EXPLAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLinBoxSmart->CreateSubControl( "COSTUME_STAT_BODY_BACK_IMAGE" );
	RegisterControl( pLinBoxSmart );

	CBasicLineBoxEx* pBasicLineBox = new CBasicLineBoxEx(m_pEngineDevice);   
	pBasicLineBox->CreateSub ( this, "COSTUME_LINE_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseCostumeTitleBox( "COSTUME_STAT_BOX_EXPLAIN" );

	RegisterControl ( pBasicLineBox );

	m_pFont10      = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_BOLD_FLAG | D3DFONT_SHADOW );

	CreateText( CP_SUBCTRL_TEXT_EXPLAIN, "COSTUME_STAT_TXT_EXPLAIN", m_pFont10, TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );

	if( m_pSubCtrl[CP_SUBCTRL_TEXT_EXPLAIN] )
	{
		DCAST( CBasicTextBox, m_pSubCtrl[CP_SUBCTRL_TEXT_EXPLAIN])->SetText(ID2GAMEINTEXT("COSTUME_STAT_EXPLAIN"), NS_UITEXTUTIL::WHITE );	
	}

}

void CCostumeStatExplain::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CCostumeStatExplain::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CCostumeStatExplain::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle(bVisible);
}