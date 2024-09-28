#include "StdAfx.h"

#include "../../../SigmaCore/String/basestring.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/Common/SubPath.h"
#include "../../../EngineLib/G-Logic/GLogic.h"

#include "../../../RanLogicClient/Etc/BaseStringParser.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../StaticUIManager.h"
#include "../../InnerInterface.h"
#include "SimpleMessageMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const	float	CSimpleMessageMan::fLIFE_TIME = 4.0f;

CSimpleMessageMan::CSimpleMessageMan(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
{
}

CSimpleMessageMan::~CSimpleMessageMan ()
{
}

void CSimpleMessageMan::CreateSubControl ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "SIMPLE_MESSAGE_MAN_TEXT" );
	pTextBox->SetFont ( pFont );
	RegisterControl ( pTextBox );
	m_pInfoDisplay = pTextBox;

	CUIControl*	pDummy = new CUIControl(m_pEngineDevice);
	pDummy->CreateSub ( this, "SIMPLE_MESSAGE_MAN_TEXT" );
	RegisterControl ( pDummy );
	m_pInfoDisplayDummy = pDummy;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		CGameTextMan::GetInstance().LoadText( RANPARAM::strTipText.GetString(), CGameTextMan::EM_TIP_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT );
	else
	{
		DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
		CString strFullPath = pGlobalStage->GetAppPath();
		const SUBPATH* pPath = m_pGaeaClient->GetSubPath();
		strFullPath += pPath->GuiFileRoot().c_str(); //SUBPATH::GLOGIC_FILE;
		strFullPath += ID2GAMEWORD("TIP_FILE");

		CBaseStringParser::Parse( strFullPath, m_vecMESSAGE );
	}
}

void CSimpleMessageMan::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_fLifeTime -= fElapsedTime;

	if ( m_fLifeTime < 0.0f )
	{
		ClearText ();
	}

	if ( m_fLifeTime < -3.0f )
	{
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			AddText ( CGameTextMan::GetInstance().GetTipRandomText(), NS_UITEXTCOLOR::WHITE );
		else
		{
			if ( !m_vecMESSAGE.size () )
			{
				m_pInterface->UiHideGroup ( GetWndID () );
				return ;
			}

			const int MAX_INDEX = static_cast<int>(m_vecMESSAGE.size ());
			//srand ( (UINT)time( NULL ) );
			int nIndex = rand () % MAX_INDEX;
			AddText ( m_vecMESSAGE[nIndex], NS_UITEXTCOLOR::WHITE );
		}
	}

	const long lResolution = CUIMan::GetResolution ();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	//	NOTE
	//		포지션 설정
	{
		bool bOVER_ONELINE = false;
		if ( 1 < m_pInfoDisplay->GetTotalLine () ) bOVER_ONELINE = true;

		const UIRECT& rcOriginPos = m_pInfoDisplayDummy->GetGlobalPos ();

		D3DXVECTOR2 vNewPos;
		vNewPos.x = (X_RES - rcOriginPos.sizeX) / 2.0f;
		vNewPos.y = 170.0f;

		if ( bOVER_ONELINE )
		{
			vNewPos.x += 60.0f;
			m_pInfoDisplay->SetTextAlign ( TEXT_ALIGN_LEFT );
		}
		else
		{
			m_pInfoDisplay->SetTextAlign ( TEXT_ALIGN_CENTER_X );
		}

		m_pInfoDisplay->SetGlobalPos ( vNewPos );			
	}
}

void	CSimpleMessageMan::ClearText ()
{
	m_pInfoDisplay->ClearText ();
}

void CSimpleMessageMan::AddText ( CString strMessage, D3DCOLOR dwMessageColor )
{
	m_pInfoDisplay->AddText ( strMessage, dwMessageColor );

	m_fLifeTime = fLIFE_TIME * m_pInfoDisplay->GetLineCount(0);
}