#include "StdAfx.h"
#include "CaptureTheIslandsUI.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UIWindowBody.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Club/GLClubClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTIAuthProgressUI::CTIAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, fAuthTime(0.0f)
, fCurrentTime(0.0f)
{
}

CTIAuthProgressUI::~CTIAuthProgressUI ()
{
}

void CTIAuthProgressUI::CreateSubControl ()
{    
	fAuthTime = 0.0f;
	fCurrentTime = 0.0f;

	CBasicProgressBar* pProgress = new CBasicProgressBar(m_pEngineDevice);
	pProgress->CreateSub ( this, "CTI_AUTH_PROGRESS_IMAGE", UI_FLAG_DEFAULT);
	pProgress->CreateOverImage ( "CTI_AUTH_PROGRESS_OVER_IMAGE" );
	RegisterControl ( pProgress );
	m_pProgress = pProgress;
}

void CTIAuthProgressUI::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( fAuthTime > 0.0f )
	{
		fCurrentTime += fElapsedTime;
		if ( fAuthTime <= fCurrentTime )
		{
			fCurrentTime = 0.0f;
			fAuthTime = 0.0f;
			SetVisibleSingle(FALSE);
		}
		else
		{
			const float _timerPercent = fCurrentTime / fAuthTime;
			m_pProgress->SetPercent(_timerPercent);
			m_pProgress->UpdateProgress();
		}
	}
}

void CTIAuthProgressUI::SetAuthTime(const float _fAuthTime, const float _fCurrentTime)
{
	fAuthTime = _fAuthTime;
	fCurrentTime = _fCurrentTime;
}

CTIProgressInfoUI::CTIProgressInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pTopText(NULL)
, m_pMiddleText(NULL)
, m_pBottomText(NULL)
, m_pBackGround(NULL)
{
}

CTIProgressInfoUI::~CTIProgressInfoUI ()
{
}

void CTIProgressInfoUI::CreateSubControl()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG);
	const int nAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	
	{
		CUIWindowBody* pContorl = new CUIWindowBody(m_pEngineDevice);
		pContorl->CreateSub( this, "CTI_PROGRESS_INFO_BACKGROUND", UI_FLAG_CENTER_X);
		pContorl->CreateBody("CTI_PROGRESS_INFO_BACKGROUND_LEFT",
			"CTI_PROGRESS_INFO_BACKGROUND_UP",
			"CTI_PROGRESS_INFO_BACKGROUND_MAIN",
			"CTI_PROGRESS_INFO_BACKGROUND_DOWN",
			"CTI_PROGRESS_INFO_BACKGROUND_RIGHT");
		RegisterControl ( pContorl );
		m_pBackGround = pContorl;
	}
	{
		CBasicTextBox* pText = new CBasicTextBox(m_pEngineDevice);
		pText->CreateSub ( this, "CTI_PROGRESS_INFO_TOP_TEXTBOX", UI_FLAG_DEFAULT);
		pText->SetFont ( pFont );
		pText->SetTextAlign ( nAlign );
		RegisterControl ( pText );
		m_pTopText = pText;
	}
	{
		CBasicTextBox* pText = new CBasicTextBox(m_pEngineDevice);
		pText->CreateSub ( this, "CTI_PROGRESS_INFO_MIDDLE_TEXTBOX", UI_FLAG_DEFAULT);
		pText->SetFont ( pFont );
		pText->SetTextAlign ( nAlign );
		RegisterControl ( pText );
		m_pMiddleText = pText;
	}
	{
		CBasicTextBox* pText = new CBasicTextBox(m_pEngineDevice);
		pText->CreateSub ( this, "CTI_PROGRESS_INFO_BOTTOM_TEXTBOX", UI_FLAG_DEFAULT);
		pText->SetFont ( pFont );
		pText->SetTextAlign ( nAlign );
		RegisterControl ( pText );
		m_pBottomText = pText;
	}
}

void CTIProgressInfoUI::SetTopParam(const int _iParam1, const int _iParam2)
{
	std::string _strName = sc::string::format(ID2GAMEWORD("CTI_INFO_NAME_WORD", _iParam1), _iParam1);  // 성문 선도클럽 결정전;
	std::string _strProgress = sc::string::format(ID2GAMEWORD("CTI_INFO_PROGRESS_WORD", _iParam2), _iParam1);  // 진행중;
	m_pTopText->SetOneLineText(sc::string::format("%1% %2%", _strName, _strProgress).c_str());
}

void CTIProgressInfoUI::SetMiddleParam(const int _iParam1)
{
	std::string strMiddle = sc::string::format(ID2GAMEWORD("CTI_INFO_WORD", 0), _iParam1);  // 목표점수 : %1%;
	m_pMiddleText->SetOneLineText(strMiddle.c_str());
}

void CTIProgressInfoUI::SetBottomParam(const float _fParam1)
{
	m_fRemainTime = _fParam1;
}

void CTIProgressInfoUI::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update(x, y , LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
	if ( m_fRemainTime > 0.0f )
	{
		m_fRemainTime -= fElapsedTime;
		const int _iRemainTime = (int)m_fRemainTime;
		std::string strBottom = sc::string::format(ID2GAMEWORD("CTI_INFO_WORD", 1), (_iRemainTime / 60),  (_iRemainTime % 60));  // 남은 시간 : %1%:%2%;
		m_pBottomText->SetOneLineText(strBottom.c_str());
	}
	if ( MouseUpdate( x, y ) )
	{
		m_pBackGround->SetVisibleSingle(TRUE);
	}
	else
	{
		m_pBackGround->SetVisibleSingle(FALSE);
	}
}

CTIRankInfoUI::CTIRankInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pCountryRankText(NULL)
, m_pCountryNameText(NULL)
, m_pCountryPointText(NULL)
, m_pMyRankText(NULL)
, m_pMyNameText(NULL)
, m_pMyPointText(NULL)
, m_pBackGround(NULL)
{
}

CTIRankInfoUI::~CTIRankInfoUI ()
{
}

void CTIRankInfoUI::CreateSubControl()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
	const int nAlignCenter = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
	{
		CUIWindowBody* pContorl = new CUIWindowBody(m_pEngineDevice);
		pContorl->CreateSub( this, "CTI_RANK_INFO_BACKGROUND", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
		pContorl->CreateBody("CTI_RANK_INFO_BACKGROUND_LEFT",
			"CTI_RANK_INFO_BACKGROUND_UP",
			"CTI_RANK_INFO_BACKGROUND_MAIN",
			"CTI_RANK_INFO_BACKGROUND_DOWN",
			"CTI_RANK_INFO_BACKGROUND_RIGHT");
		RegisterControl ( pContorl );
		m_pBackGround = pContorl;
	}
	{
		CBasicTextBox* pText = CreateBasicTextControl(this, "CTI_RANK_INFO_TITLE_RANK_TEXTBOX", pFont, nAlignCenter);
		const char* strText = ID2GAMEWORD("CTI_RANK_WORD", 0);
		//const char* strText = "순위";
		pText->SetOneLineText(strText);
	}
	{
		CBasicTextBox* pText = CreateBasicTextControl(this, "CTI_RANK_INFO_TITLE_NAME_TEXTBOX", pFont, nAlignCenter);
		const char* strText = ID2GAMEWORD("CTI_RANK_WORD", 1);
		//const char* strText = "클럽명";
		pText->SetOneLineText(strText);
	}
	{
		CBasicTextBox* pText = CreateBasicTextControl(this, "CTI_RANK_INFO_TITLE_POINT_TEXTBOX", pFont, nAlignCenter);
		const char* strText = ID2GAMEWORD("CTI_RANK_WORD", 2);
		//const char* strText = "누적포인트";
		pText->SetOneLineText(strText);
	}
	{
		CBasicTextBox* pText = CreateBasicTextControl(this, "CTI_RANK_INFO_HEADLINE_TEXTBOX", pFont, nAlignCenter);
		const char* strText = ID2GAMEWORD("CTI_RANK_WORD", 3);
		//const char* strText = "==============================";
		pText->SetOneLineText(strText);        
	}
	{
		CBasicTextBox* pText = CreateBasicTextControl(this, "CTI_RANK_INFO_RANK_TEXTBOX", pFont, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP);
		pText->SetPartInterval(5.0f);
		m_pCountryRankText = pText;
	}
	{
		CBasicTextBox* pText = CreateBasicTextControl(this, "CTI_RANK_INFO_NAME_TEXTBOX", pFont, TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP);
		pText->SetPartInterval(5.0f);
		m_pCountryNameText = pText;
	}
	{
		CBasicTextBox* pText = CreateBasicTextControl(this, "CTI_RANK_INFO_POINT_TEXTBOX", pFont, TEXT_ALIGN_RIGHT | TEXT_ALIGN_TOP);
		pText->SetPartInterval(5.0f);
		m_pCountryPointText = pText;
	}
	{
		CBasicTextBox* pText = CreateBasicTextControl(this, "CTI_RANK_INFO_TAILLINE_TEXTBOX", pFont, nAlignCenter);
		const char* strText = ID2GAMEWORD("CTI_RANK_WORD", 3);
		//const char* strText = "==============================";
		pText->SetOneLineText(strText);        
	}
	{
		CBasicTextBox* pText = CreateBasicTextControl(this, "CTI_RANK_INFO_MY_RANK_TEXTBOX", pFont, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP);
		m_pMyRankText = pText;
	}
	{
		CBasicTextBox* pText = CreateBasicTextControl(this, "CTI_RANK_INFO_MY_NAME_TEXTBOX", pFont, TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP);
		m_pMyNameText = pText;
	}
	{
		CBasicTextBox* pText = CreateBasicTextControl(this, "CTI_RANK_INFO_MY_POINT_TEXTBOX", pFont, TEXT_ALIGN_RIGHT | TEXT_ALIGN_TOP);
		m_pMyPointText = pText;
	}
}

void CTIRankInfoUI::SetRankInfo(GLMSG::NET_CTI_RANK_VEC& _RankInfo)
{
	m_pCountryRankText->ClearText();
	m_pCountryNameText->ClearText();
	m_pCountryPointText->ClearText();

	const char* strPointXml = ID2GAMEWORD("CTI_RANK_WORD", 4);
	//const char* strPointXml = "%1%P";
	for ( GLMSG::NET_CTI_RANK_VEC::iterator iter = _RankInfo.begin(); iter != _RankInfo.end(); iter++)
	{
		m_pCountryRankText->AddText(sc::string::format("%1%", iter->iRank).c_str());
		m_pCountryNameText->AddText(sc::string::format("%1%", iter->countryName).c_str());
		m_pCountryPointText->AddText(sc::string::format(strPointXml, iter->iPoint).c_str());
	}
}

void CTIRankInfoUI::SetRankMyInfo(const int _iRank, const int _iPoint)
{
	m_pMyRankText->SetOneLineText(sc::string::format("%1%", _iRank).c_str());

	Country::GLCountry* pCountry = m_pGaeaClient->GetClientCountry();
	if ( pCountry )
	{   
		m_pMyNameText->SetOneLineText( pCountry->GetCountryName().c_str() );
	}
	const char* strPointXml = ID2GAMEWORD("CTI_RANK_WORD", 4);
	//const char* strPointXml = "%1%P";
	m_pMyPointText->SetOneLineText(sc::string::format(strPointXml, _iPoint).c_str());
}

void CTIRankInfoUI::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update(x, y , LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
	if ( MouseUpdate( x, y ) )
	{
		m_pBackGround->SetVisibleSingle(TRUE);
	}
	else
	{
		m_pBackGround->SetVisibleSingle(FALSE);
	}
}

void CTIRankInfoUI::SetVisibleSingle( BOOL bVisible )
{
	if ( bVisible )
	{
		// UI 위치가 겹쳐서 퀘스트 도우미UI를 끈다;
		m_pInterface->UiHideGroup( QUEST_HELPER_DISPLAY );
	}
	else
	{
		// 꺼놓았던 퀘스트 도우미UI를 켠다;
		m_pInterface->UiShowGroupBottom( QUEST_HELPER_DISPLAY );
	}

	CUIGroup::SetVisibleSingle(bVisible);
}




























MyCTIAuthProgressUI::MyCTIAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CTIAuthProgressUI(pGaeaClient, pInterface, pEngineDevice)
{
}

void MyCTIAuthProgressUI::CreateUIWindowAndRegisterOwnership()
{
	CTIAuthProgressUI::Create(CTI_AUTH_PROGRESS_UI, "CTI_AUTH_PROGRESS_UI", UI_FLAG_CENTER_X | UI_FLAG_BOTTOM );
	CTIAuthProgressUI::CreateSubControl();
	CTIAuthProgressUI::m_pInterface->UiRegisterControl( this );
	CTIAuthProgressUI::m_pInterface->UiShowGroupFocus( CUIGroup::GetWndID() );
	CTIAuthProgressUI::SetVisibleSingle(FALSE);
}

void MyCTIAuthProgressUI::SetAuthTime(const bool _bVisible, const float _fAuthTime, const float _fCurrentTime /* = 0.0f */)
{
	SetVisibleSingle(_bVisible);
	if ( _bVisible == true )
	{
		CTIAuthProgressUI::SetAuthTime(_fAuthTime, _fCurrentTime);
	}
}

MyCTIProgressInfoUI::MyCTIProgressInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CTIProgressInfoUI(pGaeaClient, pInterface, pEngineDevice)
{
}

void MyCTIProgressInfoUI::CreateUIWindowAndRegisterOwnership()
{
	CTIProgressInfoUI::Create(CTI_PROGRESS_INFO_UI, "CTI_PROGRESS_INFO_UI", UI_FLAG_XRESRATE);
	CTIProgressInfoUI::CreateSubControl();
	CTIProgressInfoUI::m_pInterface->UiRegisterControl( this );
	CTIProgressInfoUI::m_pInterface->UiShowGroupFocus( CUIGroup::GetWndID() );
	CTIProgressInfoUI::SetVisibleSingle(FALSE);
}

void MyCTIProgressInfoUI::SetProgressInfo(const NET_MSG_GENERIC* _pMessag)
{
	GLMSG::NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC* pMsg = (GLMSG::NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC*)_pMessag;
	CTIProgressInfoUI::SetVisibleSingle(TRUE);
	CTIProgressInfoUI::SetTopParam(pMsg->iNameIndex, pMsg->iProgressIndex);
	CTIProgressInfoUI::SetMiddleParam(pMsg->iPoint);
	CTIProgressInfoUI::SetBottomParam(pMsg->fRemainTime);
}

MyCTIRankInfoUI::MyCTIRankInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CTIRankInfoUI(pGaeaClient, pInterface, pEngineDevice)
{
}

void MyCTIRankInfoUI::CreateUIWindowAndRegisterOwnership()
{
	CTIRankInfoUI::Create(CTI_RANK_INFO_UI, "CTI_RANK_INFO_UI", UI_FLAG_RIGHT);
	CTIRankInfoUI::CreateSubControl();
	CTIRankInfoUI::m_pInterface->UiRegisterControl( this );
	CTIRankInfoUI::m_pInterface->UiShowGroupFocus( CUIGroup::GetWndID() );
	CTIRankInfoUI::SetVisibleSingle(FALSE);
}

void MyCTIRankInfoUI::SetRankInfo(const NET_MSG_GENERIC* _pMessag)
{
	GLMSG::NET_INSTANCE_CTI_UI_RANK_FC netMsg;
	if ( CastNetMsg(_pMessag, netMsg) == false )
		return;

	CTIRankInfoUI::SetVisibleSingle(TRUE);
	CTIRankInfoUI::SetRankInfo(netMsg.rankVector);
}

void MyCTIRankInfoUI::SetRankMyInfo(const NET_MSG_GENERIC* _pMessag )
{
	const GLMSG::NET_INSTANCE_CTI_UI_RANK_MY_INFO_FC* pMsg = 
		static_cast< const GLMSG::NET_INSTANCE_CTI_UI_RANK_MY_INFO_FC* >( _pMessag );
	CTIRankInfoUI::SetRankMyInfo( pMsg->iMyCountryRank, pMsg->iMyCountryPoint );
}