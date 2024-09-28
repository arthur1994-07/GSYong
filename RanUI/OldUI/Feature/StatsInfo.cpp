#include "StdAfx.h"
#include "StatsInfo.h"

#include "../../InnerInterface.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CStatsInfo::CStatsInfo(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
{
	for ( int i=0; i<EMSIZE; ++i )
	{
		m_pItem [i] = NULL;
	}
}

CStatsInfo::~CStatsInfo()
{
}

void CStatsInfo::CreateSubControl()
{
	CStatsItem** ppItem;

	ppItem = &(m_pItem[EMPOW]);
	(*ppItem) = new CStatsItem(m_pEngineDevice);
	(*ppItem)->CreateSub( this, "STATSINFO_ITEM_POW", UI_FLAG_YSIZE, STATSINFO_ITEM_POW );
	(*ppItem)->CreateSubControl ( ID2GAMEWORD("CHARACTER_STAT_STATIC", 0) );
	RegisterControl ( (*ppItem) );

	ppItem = &(m_pItem[EMDEX]);
	(*ppItem) = new CStatsItem(m_pEngineDevice);
	(*ppItem)->CreateSub( this, "STATSINFO_ITEM_DEX", UI_FLAG_YSIZE, STATSINFO_ITEM_DEX );
	(*ppItem)->CreateSubControl ( ID2GAMEWORD("CHARACTER_STAT_STATIC", 1) );
	RegisterControl ( (*ppItem) );

	ppItem = &(m_pItem[EMSPI]);
	(*ppItem) = new CStatsItem(m_pEngineDevice);
	(*ppItem)->CreateSub( this, "STATSINFO_ITEM_SPI", UI_FLAG_YSIZE, STATSINFO_ITEM_SPI );
	(*ppItem)->CreateSubControl ( ID2GAMEWORD("CHARACTER_STAT_STATIC", 2) );
	RegisterControl ( (*ppItem) );

	ppItem = &(m_pItem[EMSTR]);
	(*ppItem) = new CStatsItem(m_pEngineDevice);
	(*ppItem)->CreateSub( this, "STATSINFO_ITEM_STR", UI_FLAG_YSIZE, STATSINFO_ITEM_STR );
	(*ppItem)->CreateSubControl ( ID2GAMEWORD("CHARACTER_STAT_STATIC", 4) );
	RegisterControl ( (*ppItem) );

	ppItem = &(m_pItem[EMSTA]);
	(*ppItem) = new CStatsItem(m_pEngineDevice);
	(*ppItem)->CreateSub( this, "STATSINFO_ITEM_STA", UI_FLAG_YSIZE, STATSINFO_ITEM_STA );
	(*ppItem)->CreateSubControl ( ID2GAMEWORD("CHARACTER_STAT_STATIC", 5) );
	RegisterControl ( (*ppItem) );
}

void CStatsInfo::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	SetBasicData ( m_sStatsBasic, m_sStatsBasicAdd );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CStatsInfo::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case STATSINFO_ITEM_POW:
	case STATSINFO_ITEM_DEX:
	case STATSINFO_ITEM_SPI:
	case STATSINFO_ITEM_STR:
	case STATSINFO_ITEM_STA:
		{
			AddMessageEx( dwMsg );
		}
		break;
	}
}

void CStatsInfo::Reset ( const SCHARSTATS& sBasic, const SCHARSTATS& sBasicAdd )
{
	SetBasicData ( sBasic, sBasicAdd );

	m_pItem[EMPOW]->SetAdd((INT32)sBasicAdd.wPow);
	m_pItem[EMDEX]->SetAdd((INT32)sBasicAdd.wDex);
	m_pItem[EMSPI]->SetAdd((INT32)sBasicAdd.wSpi);
	m_pItem[EMSTR]->SetAdd((INT32)sBasicAdd.wStr);
	m_pItem[EMSTA]->SetAdd((INT32)sBasicAdd.wSta);

	UnlockDownButton();
}

void CStatsInfo::SetBasicData ( const SCHARSTATS& sBasic, const SCHARSTATS& sBasicAdd )
{
	m_sStatsBasic    = sBasic;
	m_sStatsBasicAdd = sBasicAdd;

	m_pItem[EMPOW]->SetBasic  (m_sStatsBasic.wPow);
	m_pItem[EMPOW]->SetMinMax (0, m_sStatsBasicAdd.wPow);

	m_pItem[EMDEX]->SetBasic  (m_sStatsBasic.wDex);
	m_pItem[EMDEX]->SetMinMax (0, m_sStatsBasicAdd.wDex);

	m_pItem[EMSPI]->SetBasic  (m_sStatsBasic.wSpi);
	m_pItem[EMSPI]->SetMinMax (0, m_sStatsBasicAdd.wSpi);

	m_pItem[EMSTR]->SetBasic  (m_sStatsBasic.wStr);
	m_pItem[EMSTR]->SetMinMax (0, m_sStatsBasicAdd.wStr);

	m_pItem[EMSTA]->SetBasic  (m_sStatsBasic.wSta);
	m_pItem[EMSTA]->SetMinMax (0, m_sStatsBasicAdd.wSta);
}

void CStatsInfo::SetAdd ( const SCHARSTATS& sCurAdd )
{
	m_pItem[EMPOW]->SetAdd((INT32)sCurAdd.wPow);
	m_pItem[EMDEX]->SetAdd((INT32)sCurAdd.wDex);
	m_pItem[EMSPI]->SetAdd((INT32)sCurAdd.wSpi);
	m_pItem[EMSTR]->SetAdd((INT32)sCurAdd.wStr);
	m_pItem[EMSTA]->SetAdd((INT32)sCurAdd.wSta);
}

void CStatsInfo::SetType ( const DWORD dwType )
{
	for ( int i=0; i<EMSIZE; ++i )
	{
		if ( !m_pItem[i] ) continue;

		m_pItem[i]->SetType( dwType );
	}
}

void CStatsInfo::LockDownButton()
{
	for ( int i=0; i<EMSIZE; ++i )
	{
		if ( !m_pItem[i] ) continue;

		m_pItem[i]->SetEnableDownButton( FALSE );
	}
}

void CStatsInfo::UnlockDownButton()
{
	for ( int i=0; i<EMSIZE; ++i )
	{
		if ( !m_pItem[i] ) continue;

		m_pItem[i]->SetEnableDownButton( TRUE );
	}
}

void CStatsInfo::GetDecreaseStats ( WORD* pOutStats )
{
	if ( !pOutStats )
	{
		return;
	}

	for ( int i=0; i<EMSIZE; ++i )
	{
		if ( !m_pItem[i] ) continue;

		pOutStats[i] = m_pItem[i]->GetMax() - m_pItem[i]->GetAdd();
	}
}
