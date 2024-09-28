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
	, m_bOnlyUI(FALSE)		// Client �������� ������ �ִ� Data.
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
		// ȭ���� ������ �� �� �� �ֵ��� �۾��Ѵ�.
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
		// ���� ������ FogRange ���� �����ϵ��� �Ѵ�.
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

// 	// uuid ����
// 	UUID sGUID;
// 	sc::SeqUniqueGUID::Instance()->getGUID( sGUID );

	SITEMCUSTOM sITEMCUSTOM = sInvenItem.sItemCustom;
	
	// guid Ȯ��
	if ( sITEMCUSTOM.IS_GUID_NULL() )
	{
		return false;
	}

	// ��ħ ������ ��� ������ ������ �������� �̹� ��ϵǾ� �ִ��� �˻�
	if ( pITEM->ISPILE() )
	{
		bool bREG = IsRegItem( sInvenItem.sItemCustom.GetNativeID() );
		if ( bREG )
			return false;
		sITEMCUSTOM.wTurnNum = static_cast< WORD >( dwNUM );
	}
	
	// ��ħ�� �ƴ� ���� �Ǹ� ���� 1���� ����.
	if ( !pITEM->ISPILE() )
		dwNUM = 1;

	// �� ������ �����Ǿ� ���� ���� ���.
	if ( sSALEPOS == SNATIVEID( false ) )
	{
		BOOL bOK = m_invenSALE.FindInsertable( sSALEPOS.wMainID, sSALEPOS.wSubID );
		if ( !bOK )
			return false;
	}

	// �κ��丮�� ���.
	BOOL bOK = m_invenSALE.InsertItem( sITEMCUSTOM, sSALEPOS.wMainID, sSALEPOS.wSubID );
	if ( !bOK )
		return false;

	// �Ǹ� ���� ���
	SSALEITEM sSALE;
	sSALE.sSALEPOS = sSALEPOS;
	sSALE.sITEMCUSTOM = sInvenItem.sItemCustom;
	sSALE.sINVENPOS = sINVENPOS;
	sSALE.llPRICE = llPRICE;
	sSALE.dwNUMBER = dwNUM;
	sSALE.sGUID = sInvenItem.sItemCustom.guid;
		
	// insert �� �� value_type �� ����ϴ°� ����(make_pare ����)
	m_mapSALE.insert( MAPITEM_VALUE( sSALEPOS.dwID,sSALE ) );

	// �Ʒ� ���� �˻��ý��� ������� ����
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
