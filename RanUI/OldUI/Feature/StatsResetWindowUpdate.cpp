#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "StatsResetWindow.h"
#include "StatsInfo.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CStatsResetWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	const GLCHARLOGIC& sCharData  = m_pGaeaClient->GetCharacterLogic ();	
	const GLCONST_CHARCLASS& cCHARCONST = GLCONST_CHAR::cCONSTCLASS[sCharData.GETCHARINDEX()];	//	�ɸ����� �����.
	BOOL bPointUsable = (sCharData.m_wStatsPoint)?TRUE:FALSE;

	// Note : ���� ��Ʈ��
	SCHARSTATS sBasic,sBasicAdded;

	CalBasicStats ( sBasic      );
	CalAddedStats ( sBasicAdded );

	m_pStats		->SetBasicData(sBasic, sBasicAdded);
	m_pStats_Preview->SetBasicData(sBasic, sBasicAdded);

	// Note : ���� ����
	SetPointControl ( sCharData );

	// Note : ���� ���� �ؽ�Ʈ ����
	SetVariableTextControl ();

	// Note : ��ȯ ������ �ִ�ġ�� �Ѿ�� ��ư�� ��ٴ�.
	if ( m_wReturnStats >= m_wReturnStatsMax )
	{
		m_pStats_Preview->LockDownButton();
	}
	else
	{
		m_pStats_Preview->UnlockDownButton();
	}

//     GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();	
// 
// 	if (pCharacter)
//         m_wRemainInven = pCharacter->m_cInventory.GetAmountEmpty(false);
// 
//     //  OK��ư �� Ȱ��ȭ
//     if (m_wReqInven > m_wRemainInven)
//         m_pOK_Button->SetFlip(true);
//     else
//         m_pOK_Button->SetFlip(false);

	/*
	{	//	�� �ڸ� ã�ư���
		const UIRECT& rcPowPos = m_pPowDisableButton->GetGlobalPos ();
		m_pPowButton->SetGlobalPos ( D3DXVECTOR2 ( rcPowPos.left, rcPowPos.top ) );

		const UIRECT& rcDexPos = m_pDexDisableButton->GetGlobalPos ();
		m_pDexButton->SetGlobalPos ( D3DXVECTOR2 ( rcDexPos.left, rcDexPos.top ) );

		const UIRECT& rcSpiPos = m_pSpiDisableButton->GetGlobalPos ();
		m_pSpiButton->SetGlobalPos ( D3DXVECTOR2 ( rcSpiPos.left, rcSpiPos.top ) );

		//const UIRECT& rcIntPos = m_pIntDisableButton->GetGlobalPos ();
		//m_pIntButton->SetGlobalPos ( D3DXVECTOR2 ( rcIntPos.left, rcIntPos.top ) );

		const UIRECT& rcStaPos = m_pStaDisableButton->GetGlobalPos ();
		m_pStaButton->SetGlobalPos ( D3DXVECTOR2 ( rcStaPos.left, rcStaPos.top ) );

		const UIRECT& rcStrPos = m_pStrDisableButton->GetGlobalPos ();
		m_pStrButton->SetGlobalPos ( D3DXVECTOR2 ( rcStrPos.left, rcStrPos.top ) );
	}
	*/

	/*
	{
		m_pStatsText->m_pStats[EMPOW]->SetUseTextColor ( 0, FALSE );
		m_pStatsText->m_pName [EMPOW]->SetUseTextColor ( 0, FALSE );

		m_pStatsText->m_pStats[EMDEX]->SetUseTextColor ( 0, FALSE );
		m_pStatsText->m_pName [EMDEX]->SetUseTextColor ( 0, FALSE );

		m_pStatsText->m_pStats[EMSPI]->SetUseTextColor ( 0, FALSE );
		m_pStatsText->m_pName [EMSPI]->SetUseTextColor ( 0, FALSE );

		m_pStatsText->m_pStats[EMSTR]->SetUseTextColor ( 0, FALSE );
		m_pStatsText->m_pName [EMSTR]->SetUseTextColor ( 0, FALSE );

		m_pStatsText->m_pStats[EMSTA]->SetUseTextColor ( 0, FALSE );
		m_pStatsText->m_pName [EMSTA]->SetUseTextColor ( 0, FALSE );
	}
	*/

	CUIWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CStatsResetWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindow::SetVisibleSingle( bVisible );

	if( bVisible )
	{
		const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();
		//SetPhoneNumber( sCharData.m_szPhoneNumber );
	}
}
