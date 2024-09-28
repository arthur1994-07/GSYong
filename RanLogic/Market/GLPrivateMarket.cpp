#include "pch.h"

#include <psapi.h>

#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "../../EngineLib/DxTools/DxFogMan.h"

#include "../Item/GLItemMan.h"
#include "../GLogicDataMan.h"
#include "./GLPrivateMarket.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLPrivateMarket::GLPrivateMarket(void)
	: m_bOPEN(false)
	, m_bOnlyUI(FALSE)		// Client 내에서만 가지고 있는 Data.
{
}

GLPrivateMarket::~GLPrivateMarket(void)
{
}

bool GLPrivateMarket::DoMarketOpen ()
{
	m_bOPEN = true;

	if ( m_bOnlyUI )
	{
		// 화면을 렌더링 안 할 수 있도록 작업한다.
		//
		DxFogMan::GetInstance().BackUpFogRangeForPMarket();
	}

	return true;
}

void GLPrivateMarket::DoMarketClose()
{
	m_bOPEN = false;
	m_strTITLE.clear();

	m_mapSALE.clear();
	m_invenSALE.DeleteItemAll();

	m_mapSearchItem.clear();

	if ( m_bOnlyUI )
	{
		// 상점 열기전 FogRange 값을 셋팅하도록 한다.
		//
		DxFogMan::GetInstance().RollBackFogRangeForPMarket();

		m_bOnlyUI = FALSE;
	}
}

void GLPrivateMarket::DoMarketInfoRelease ()
{
	m_mapSALE.clear();
	m_mapSearchItem.clear();
	m_invenSALE.DeleteItemAll();

}

void GLPrivateMarket::SetTITLE(const std::string& strTITLE)
{
	m_strTITLE = strTITLE;
}

bool GLPrivateMarket::SetSaleState(const SNATIVEID& sSALEPOS, DWORD dwNUM, bool bSOLD)
{
	MAPITEM_ITER pos = m_mapSALE.find(sSALEPOS.dwID);
	if (pos == m_mapSALE.end())
		return false;

	SSALEITEM &sSALE = (*pos).second;
	sSALE.dwNUMBER = dwNUM;
	sSALE.bSOLD = bSOLD;

	const SITEM *pITEM = GLogicData::GetInstance().GetItem(sSALE.sITEMCUSTOM.GetBoxWrappedItemID());
	if (pITEM && pITEM->ISPILE())
	{
		SINVENITEM* pINVENITEM = m_invenSALE.GetItem(sSALEPOS.wMainID, sSALEPOS.wSubID);
		if (pINVENITEM)
			pINVENITEM->sItemCustom.wTurnNum = (WORD) dwNUM;
	}
	return true;
}

bool GLPrivateMarket::IsRegItem ( SNATIVEID nidITEM )
{
	MAPITEM_ITER pos = m_mapSALE.begin();
	MAPITEM_ITER end = m_mapSALE.end();
	for ( ; pos!=end; ++pos )
	{
		if ( (*pos).second.sITEMCUSTOM.GetBoxWrappedItemID() == nidITEM )
			return true;
	}

	return false;
}

DWORD GLPrivateMarket::GetItemTurnNum ( SNATIVEID nidITEM )
{
	MAPITEM_ITER pos = m_mapSALE.begin();
	MAPITEM_ITER end = m_mapSALE.end();
	for ( ; pos!=end; ++pos )
	{
		if ( (*pos).second.sITEMCUSTOM.GetBoxWrappedItemID() == nidITEM )
			return (*pos).second.dwNUMBER;
	}

	return 0;

}

bool GLPrivateMarket::IsRegInvenPos ( SNATIVEID sINVENPOS )
{
	MAPITEM_ITER pos = m_mapSALE.begin();
	MAPITEM_ITER end = m_mapSALE.end();
	for ( ; pos!=end; ++pos )
	{
		if ( (*pos).second.sINVENPOS == sINVENPOS )
			return true;
	}

	return false;
}

bool GLPrivateMarket::IsSoldPos( SNATIVEID sInvenPos )
{
	MAPITEM_ITER iter = m_mapSALE.begin();
	MAPITEM_ITER iterEnd = m_mapSALE.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( iter->second.sINVENPOS == sInvenPos )
			return iter->second.bSOLD;
	}

	return false;
}

bool GLPrivateMarket::FindInsertPos(const SNATIVEID& nidITEM, SNATIVEID& sSALEPOS)
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(nidITEM);
	if (!pITEM)
		return false;

	BOOL bOK = m_invenSALE.FindInsertable(sSALEPOS.wMainID, sSALEPOS.wSubID);
	if (!bOK)
		return false;
    else
	    return true;
}

bool GLPrivateMarket::RegItem( const SINVENITEM& sInvenItem, LONGLONG llPRICE, DWORD dwNUM, SNATIVEID sSALEPOS/* =SNATIVEID */, bool bSearchMarket /*= FALSE*/ )
{
	if ( m_mapSALE.size() >= EPrivateMarketTotalSlot )
		return false;

	SNATIVEID sINVENPOS( sInvenItem.wPosX,sInvenItem.wPosY );

	const SITEM *pITEM = GLogicData::GetInstance().GetItem( sInvenItem.sItemCustom.GetBoxWrappedItemID() );
	if ( !pITEM )
		return false;

// 	// uuid 생성
// 	UUID sGUID;
// 	sc::SeqUniqueGUID::Instance()->getGUID( sGUID );

	SITEMCUSTOM sITEMCUSTOM = sInvenItem.sItemCustom;
	
	// guid 확인
	if ( sITEMCUSTOM.IS_GUID_NULL() )
	{
		return false;
	}

	// 겹침 가능일 경우 동일한 종류의 아이템이 이미 등록되어 있는지 검사
	if ( pITEM->ISPILE() )
	{
		bool bREG = IsRegItem( sInvenItem.sItemCustom.GetNativeID() );
		if ( bREG )
			return false;
		sITEMCUSTOM.wTurnNum = static_cast< WORD >( dwNUM );
	}
	
	// 겹침이 아닐 경우는 판매 수량 1개로 제한.
	if ( !pITEM->ISPILE() )
		dwNUM = 1;

	// 들어갈 슬롯이 지정되어 있지 않을 경우.
	if ( sSALEPOS == SNATIVEID( false ) )
	{
		BOOL bOK = m_invenSALE.FindInsertable( sSALEPOS.wMainID, sSALEPOS.wSubID );
		if ( !bOK )
			return false;
	}

	// 인벤토리에 등록.
	BOOL bOK = m_invenSALE.InsertItem( sITEMCUSTOM, sSALEPOS.wMainID, sSALEPOS.wSubID );
	if ( !bOK )
		return false;

	// 판매 정보 등록
	SSALEITEM sSALE;
	sSALE.sSALEPOS = sSALEPOS;
	sSALE.sITEMCUSTOM = sInvenItem.sItemCustom;
	sSALE.sINVENPOS = sINVENPOS;
	sSALE.llPRICE = llPRICE;
	sSALE.dwNUMBER = dwNUM;
	sSALE.sGUID = sInvenItem.sItemCustom.guid;
		
	// insert 할 때 value_type 을 사용하는게 좋음(make_pare 보다)
	m_mapSALE.insert( MAPITEM_VALUE( sSALEPOS.dwID,sSALE ) );

	// 아래 기존 검색시스템 사용하지 않음
	return true;
	/*
	// Add Search Item Data
	SSEARCHITEMDATA searchItemData;
	searchItemData.m_ItemName = pITEM->GetNameStr();
	searchItemData.dwReqCharClass = pITEM->sBasicOp.dwReqCharClass;
	searchItemData.dwSuitType	  = (DWORD)pITEM->sSuitOp.emAttack;
	searchItemData.wReqLevel	  = pITEM->sBasicOp.wReqLevelDW;
	searchItemData.sReqStats	  = pITEM->sBasicOp.sReqStats;	
	searchItemData.llPRICE		  = llPRICE;

	searchItemData.RandOptTYPE1	  = (BYTE)sITEMCUSTOM.GETOptTYPE1();	
	searchItemData.RandOptTYPE2	  = (BYTE)sITEMCUSTOM.GETOptTYPE2()	;	
	searchItemData.RandOptTYPE3	  = (BYTE)sITEMCUSTOM.GETOptTYPE3();	
	searchItemData.RandOptTYPE4	  = (BYTE)sITEMCUSTOM.GETOptTYPE4()	;	

	searchItemData.fRandOptValue1 = sITEMCUSTOM.GETOptVALUE1();	
	searchItemData.fRandOptValue2 = sITEMCUSTOM.GETOptVALUE2();	
	searchItemData.fRandOptValue3 = sITEMCUSTOM.GETOptVALUE3();	
	searchItemData.fRandOptValue4 = sITEMCUSTOM.GETOptVALUE4();	

	searchItemData.cDAMAGE		  = sITEMCUSTOM.cDAMAGE;		
	searchItemData.cDEFENSE		  = sITEMCUSTOM.cDEFENSE;		

	searchItemData.cRESIST_FIRE	  = sITEMCUSTOM.cRESIST_FIRE;	
	searchItemData.cRESIST_ICE	  = sITEMCUSTOM.cRESIST_ICE;		
	searchItemData.cRESIST_ELEC   = sITEMCUSTOM.cRESIST_ELEC;		
	searchItemData.cRESIST_POISON = sITEMCUSTOM.cRESIST_POISON;		
	searchItemData.cRESIST_SPIRIT = sITEMCUSTOM.cRESIST_SPIRIT;		


	if (bSearchMarket)
    {
		m_mapSearchItem.insert(std::make_pair(pITEM->sBasicOp.sNativeID.dwID, searchItemData));
    }

	return true;
	*/
}

bool GLPrivateMarket::DisItem(const SNATIVEID& sSALEPOS, bool bSearchMarket)
{
	// Add Search Item Data
	if (bSearchMarket)
	{
		SSALEITEM* pSaleItem = GetItem(sSALEPOS);
		if (pSaleItem)
		{
			MAPSEARCH_ITER pos = m_mapSearchItem.find(pSaleItem->sITEMCUSTOM.GetNativeID().dwID);
			if (pos != m_mapSearchItem.end())
				m_mapSearchItem.erase(pos);
		}
	}

	
    MAPITEM_ITER pos = m_mapSALE.find(sSALEPOS.dwID);
    if (pos == m_mapSALE.end())
	    return false;

    m_invenSALE.DeleteItem(sSALEPOS.wMainID, sSALEPOS.wSubID);
    m_mapSALE.erase(pos);

	return true;
}

SSALEITEM* GLPrivateMarket::GetItem ( SNATIVEID sSALEPOS )
{
	MAPITEM_ITER pos = m_mapSALE.find ( sSALEPOS.dwID );
	if ( pos==m_mapSALE.end() )
		return NULL;

	return &(*pos).second;
}

SSALEITEM* GLPrivateMarket::GetItem( const UUID& sGUID )
{
	MAPITEM_ITER pos = m_mapSALE.begin();
	MAPITEM_ITER end = m_mapSALE.end();
	for ( ; pos!=end; ++pos )
	{
		SSALEITEM& sSALE = (*pos).second;
		if ( sSALE.sGUID == sGUID )
		{
			return &(*pos).second;
		}
	}

	return NULL;
}
