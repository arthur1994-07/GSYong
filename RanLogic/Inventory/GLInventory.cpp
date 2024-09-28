#include "../pch.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "../Item/GLItemDef.h"
#include "../Item/GLItemMan.h"
#include "../GLogicDataMan.h"
#include "../GLUseFeatures.h"
#include "../stl_Func.h"

#include "./GLInventory.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLInventory::GLInventory()
	: m_wCellSX( EInvenX )
	, m_wCellSY( EInvenTotalLine )
	, m_wADDLINE( 0 )
	, m_bLimit( false )
    , m_Barrier( EInvenX, EInvenTotalLine )
{
	SetState( m_wCellSX, m_wCellSY );
}

GLInventory::GLInventory( WORD _sizeX, WORD _sizeY )
	: m_wCellSX( _sizeX )
	, m_wCellSY( _sizeY )
	, m_wADDLINE( 0 )
	, m_bLimit( false )
    , m_Barrier( _sizeX, _sizeY )
{
	SetState( m_wCellSX, m_wCellSY );
}

GLInventory::~GLInventory()
{
	DeleteItemAll();
}

void GLInventory::Assign(const GLInventory& Inven)
{
	SetAddLine(Inven.GETAddLine(), Inven.IsLimit());
	SetState(Inven.m_wCellSX, Inven.m_wCellSY);

    m_Barrier.Clear();
    m_Barrier = Barrier(Inven.m_wCellSX, Inven.m_wCellSY);
	
	if (m_wCellSX == Inven.m_wCellSX && m_wCellSY == Inven.m_wCellSY)
	{
        // 기존 인벤과 크기가 같을 경우
		CELL_MAP_CITER iter = Inven.m_ItemMap.begin();
		CELL_MAP_CITER iter_end = Inven.m_ItemMap.end();
		for ( ; iter != iter_end; ++iter)
		{
			InsertItem(
				iter->second->sItemCustom,
				iter->second->wPosX,
				iter->second->wPosY,
				true);
		}
	}	
	else
	{
        // 크기가 틀려졌을 경우
		SINVENITEM* pInvenItem = NULL;
		CELL_MAP_CITER iter = Inven.m_ItemMap.begin();
		CELL_MAP_CITER iter_end = Inven.m_ItemMap.end();
		for ( ; iter != iter_end; ++iter)
		{
            pInvenItem = iter->second;
			InsertItem(pInvenItem->CustomItem(), true);
		}
	}
}

void GLInventory::SetState( WORD wCellSX, WORD wCellSY )
{
	//GASSERT( m_wCellSX * m_wCellSY <= 255 );

	DeleteItemAll();

	m_wCellSX = wCellSX;
	m_wCellSY = wCellSY;
}

void GLInventory::SetAddLine( WORD wLine, bool bLimit )
{
	m_bLimit = bLimit;

	if( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
		m_wADDLINE = min( wLine, EInvenTotalLine - GLCONST_CHAR::wInvenDefaultLine );
	else
		m_wADDLINE = min( wLine, EInvenY - GLCONST_CHAR::wInvenDefaultLine );
}

void GLInventory::SetItemGenTime()
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		CTime tTIME = CTime::GetCurrentTime();

		SINVENITEM* pInvenItem = iter->second;
		pInvenItem->sItemCustom.tBORNTIME = tTIME.GetTime();
	}
}

void GLInventory::SetBarrier(BYTE nType, WORD nPosX, WORD nPosY)
{
//     GASSERT(nPosX < m_wCellSX);
//     GASSERT(nPosY < m_wCellSY);
//     
//     m_btBarrier[nPosX][nPosY] |= nType;
	m_Barrier.SetBarrier( nType, nPosX, nPosY );
}

void GLInventory::ResetBarrier( BYTE nType, WORD nPosX, WORD nPosY )
{
// 	GASSERT(nPosX < m_wCellSX);
// 	GASSERT(nPosY < m_wCellSY);
// 
// 	m_btBarrier[nPosX][nPosY] &= ~nType;
	m_Barrier.ResetBarrier( nType, nPosX, nPosY );
}

SINVENITEM* GLInventory::FindPosItem( const WORD wPosX, const WORD wPosY )
{
	if( wPosX >= m_wCellSX )
		return NULL;

	if( wPosY >= m_wCellSY )
		return NULL;

// 	if( m_btBarrier[ wPosX ][ wPosY ] == EInvenBarrierEmpty )
// 		return NULL;

	BYTE nType;
	if( m_Barrier.GetInvenState( wPosX, wPosY, nType ) )
		if( nType == EInvenBarrierUsed )
			return GetItem( wPosX, wPosY );

	return NULL;
}

SINVENITEM* GLInventory::FindItemByGenNumber( sc::MGUID& guid/*LONGLONG llGenNum, SNATIVEID sID, BYTE cGenType*/ )
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
		if( !pItem )
			continue;

		if( pInvenItem->sItemCustom.guid == guid )
		{
			return pInvenItem;
		}
		/*if( pInvenItem->sItemCustom.lnGenNum == llGenNum &&
			pInvenItem->sItemCustom.GetNativeID() == sID &&
			pInvenItem->sItemCustom.cGenType == cGenType )
		{
			return pInvenItem;
		}*/
	}

	return NULL;
}

BOOL GLInventory::FindInsertable( WORD &wPosX, WORD &wPosY, bool bAllLine )
{
	WORD nCellSX = m_wCellSX;
	WORD nCellSY = GetValidCellY( bAllLine );

	for( WORD cy=0; cy<nCellSY; cy++ )
	{
		for( WORD cx=0; cx<nCellSX; cx++ )
		{
// 			if( m_btBarrier[ cx ][ cy ] == EInvenBarrierEmpty )
// 			{
// 				wPosX = cx;
// 				wPosY = cy;
// 
// 				return TRUE;
// 			}
            BYTE nType;
            if( m_Barrier.GetInvenState( cx, cy, nType ) )
            {
                if( nType == EInvenBarrierEmpty )
                {
                    wPosX = cx;
                    wPosY = cy;

                    return TRUE;
                }
            }
		}
	}

	wPosX = 0;
	wPosY = 0;

	return FALSE;
}

BOOL GLInventory::ValidCheckInsrt( const WORD _wInsertNum, bool bAllLine )
{
	if( GetAmountEmpty( bAllLine ) < _wInsertNum )
		return FALSE;

	return TRUE;
}

BOOL GLInventory::ValidPileInsrt(const WORD wINSRTNUM, const SNATIVEID &sNID, const WORD wPILENUM, bool bAllLine)
{
	//	넣기 요청된 아이템수.( 잔여량. )
	WORD wREQINSRTNUM(wINSRTNUM);

	// 더 겹쳐질수 있는 아이템이 있는지 검사하여 인벤에 들어가야하는
	//		아이템 점검 숫자를 감소시킴.
	GLInventory::CELL_MAP_ITER iter = m_ItemMap.begin();
	for( ; iter!=m_ItemMap.end(); ++iter )
	{
		SITEMCUSTOM &sITEMCUSTOM = iter->second->sItemCustom;

		if( sITEMCUSTOM.GetNativeID() != sNID )
			continue;
		if( sITEMCUSTOM.wTurnNum>=wPILENUM )
			continue;
	
		WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum;
		if( wREQINSRTNUM > wSURPLUSNUM )
		{
			wREQINSRTNUM -= wSURPLUSNUM;
		}
		else
		{
			//	기존 아이템에 겹치는 것 만으로도 새로 넣을 아이템이 충분히 들어감.
			return TRUE;
		}
	}

	//	새로이 인벤에 들어가야할 아이템의 갯수 파악후 인벤에 들어갈 공간이 있는지 검사.

	//	최대겹침아이템 수량.
	WORD wITEMNUM = wREQINSRTNUM / wPILENUM;

	//	여분겹침아이템의 겹침수.
	WORD wSPLITNUM = wREQINSRTNUM % wPILENUM;
	if( wSPLITNUM > 0 )
		wITEMNUM += 1;

	return ValidCheckInsrt( wITEMNUM, bAllLine );
}

DWORD GLInventory::CountItem( const SNATIVEID& sNID )
{
	DWORD dwNum = 0;

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
		if( pItem && pItem->sBasicOp.sNativeID == sNID )
			++dwNum;
	}

	return dwNum;
}

DWORD GLInventory::CountItem( EMITEM_TYPE emType )
{
    DWORD dwNum = 0;

    CELL_MAP_ITER iter = m_ItemMap.begin();
    CELL_MAP_ITER iter_end = m_ItemMap.end();
    for( ; iter != iter_end; ++iter )
    {
        SINVENITEM* pInvenItem = iter->second;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
        if( pItem && pItem->BasicType() == emType )
			++dwNum;
    }

    return dwNum;
}

DWORD GLInventory::CountTurnItem( const SNATIVEID& sNID )
{
	DWORD dwNum = 0;

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;

		if( pInvenItem->sItemCustom.GetNativeID() == sNID )
			dwNum += pInvenItem->sItemCustom.wTurnNum;
	}

	return dwNum;
}

DWORD GLInventory::CountTurnItem( EMITEM_TYPE emType )
{
    DWORD dwNum = 0;

    CELL_MAP_ITER iter = m_ItemMap.begin();
    CELL_MAP_ITER iter_end = m_ItemMap.end();
    for( ; iter != iter_end; ++iter )
    {
        SINVENITEM* pInvenItem = iter->second;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->Id() );
        if( pItem && pItem->BasicType() == emType )
            dwNum += pInvenItem->sItemCustom.wTurnNum;
    }

    return dwNum;
}
DWORD GLInventory::CountTurnItem( EMITEM_TYPE emType, ITEM_VALUE typeValue, int _value )
{
	DWORD dwNum = 0;

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->Id() );
		if ( pItem == NULL )
			continue;

		if ( pItem->BasicType() != emType )
			continue;

		switch ( typeValue )
		{
		case ITEM_VAlUE_SUIT_REMODELNUM:
			if ( pItem->sSuitOp.wReModelNum != WORD(_value) )
				continue;
			break;
		default:
			continue;
		}

		dwNum += pInvenItem->sItemCustom.wTurnNum;
	}

	return dwNum;
}

DWORD GLInventory::CountDrugItem( EMITEM_DRUG emDrug )
{
	GASSERT( emDrug != ITEM_DRUG_NUNE );

	DWORD dwCount(0);
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
		if( pItem && pItem->sDrugOp.emDrug == emDrug )
			dwCount += pInvenItem->TurnNum();
	}

	return dwCount;
}

// DWORD GLInventory::CountPileItem( const SNATIVEID& sNID )
// {
// 	DWORD dwNum = 0;
// 
// 	CELL_MAP_ITER iter = m_ItemMap.begin();
// 	CELL_MAP_ITER iter_end = m_ItemMap.end();
// 	for( ; iter != iter_end; ++iter )
// 	{
// 		SINVENITEM* pInvenItem = iter->second;
// 		if( pInvenItem->sItemCustom.sNativeID != sNID )
// 			continue;
// 
// 		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.sNativeID );
// 		if( pItem->sDrugOp.wPileNum == 1 )
// 			dwNum++;
// 		else if( pItem->sDrugOp.wPileNum == pInvenItem->sItemCustom.wTurnNum )
// 			dwNum++;
// 	}
// 
// 	return dwNum;
// }

DWORD GLInventory::CountPileItem( const SNATIVEID& sNID )
{
    DWORD dwNum = 0;

    CELL_MAP_ITER iter = m_ItemMap.begin();
    CELL_MAP_ITER iter_end = m_ItemMap.end();
    for( ; iter != iter_end; ++iter )
    {
        SINVENITEM* pInvenItem = iter->second;
        if( pInvenItem->sItemCustom.GetNativeID() != sNID )
            continue;

        const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
        if( pItem->sDrugOp.wPileNum == 1 )
            dwNum++;
        else if( pInvenItem->sItemCustom.wTurnNum <= pItem->sDrugOp.wPileNum )
            dwNum += pInvenItem->sItemCustom.wTurnNum;
        else 
            dwNum += pItem->sDrugOp.wPileNum;
    }

    return dwNum;

}
SINVENITEM* GLInventory::FindItem( const SNATIVEID& sNID, int index )
{
	int nTempIndex = 0;

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;
		if( !pInvenItem )
			continue;

		if( pInvenItem->sItemCustom.GetNativeID() == sNID )
		{
			// 인덱스가 맞으면 리턴;
			if ( index == nTempIndex )
				return pInvenItem;

			nTempIndex++;
		}
	}

	return NULL;
}

SINVENITEM* GLInventory::FindItem( const EMITEM_TYPE emTYPE )
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;
		if( !pInvenItem )
			continue;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
		if( pItem && pItem->sBasicOp.emItemType == emTYPE )
			return pInvenItem;
	}

	return NULL;
}
SINVENITEM* GLInventory::FindItem( const EMITEM_TYPE emTYPE, const ITEM_VALUE _typeValue, int _value )
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;
		if ( pInvenItem == NULL )
			continue;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
		if ( pItem == NULL )
			continue;

		if ( pItem->sBasicOp.emItemType != emTYPE )
			continue;

		switch ( _typeValue )
		{
		case ITEM_VAlUE_SUIT_REMODELNUM:
			if ( pItem->sSuitOp.wReModelNum != WORD(_value) )
				continue;
			break;
		default:
			continue;
		}

		return pInvenItem;
	}
    return NULL;
}

SINVENITEM* GLInventory::FindItem(const EMITEM_TYPE emTYPE, const SNATIVEID& sNID)
{
	if( emTYPE >= ITEM_NSIZE )
        return FALSE;

	if( sNID != NATIVEID_NULL() )
	{		
		for( CELL_MAP_ITER iter=m_ItemMap.begin(); iter != m_ItemMap.end(); ++iter )
		{
			SINVENITEM* pInvenItem = iter->second;

			const SITEM* pItem = GLogicData::GetInstance().GetItem(pInvenItem->GetNativeID());
			if( pItem && pItem->sBasicOp.sNativeID == sNID )
                return pInvenItem;
		}		
	}

	for( CELL_MAP_ITER iter=m_ItemMap.begin(); iter != m_ItemMap.end(); ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;

		const SITEM* pItem = GLogicData::GetInstance().GetItem(pInvenItem->GetNativeID());
		if( pItem && pItem->sBasicOp.emItemType == emTYPE )
            return pInvenItem;
	}

	return NULL;
}

SINVENITEM* GLInventory::FindItem( const SITEMCUSTOM& rItemCustom )
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;
		if( !pInvenItem )
			continue;

		if( pInvenItem->sItemCustom == rItemCustom )
			return pInvenItem;
	}

	return NULL;
}

BOOL GLInventory::HaveEventItem()
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
		//if( pItem && pItem->sBasicOp.IsEVENT() )
		if ( pItem && ( pInvenItem->sItemCustom.IsEvent() != NULL ) )
			return TRUE;
	}

	return FALSE;
}

SINVENITEM* GLInventory::FindDrugItem( EMITEM_DRUG emDrug )
{
	GASSERT( emDrug != ITEM_DRUG_NUNE );

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
		if( pItem && pItem->sDrugOp.emDrug == emDrug )
			return pInvenItem;
	}

	return NULL;
}

SINVENITEM* GLInventory::FindItemBack( const WORD wBackX, const WORD wBackY )
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;

		if( pInvenItem->wBackX == wBackX && pInvenItem->wBackY == wBackY )
			return pInvenItem;
	}

	return NULL;
}

DWORD GLInventory::GetAmountDrugItem( EMITEM_DRUG emDrug )
{
	GASSERT( emDrug != ITEM_DRUG_NUNE );

	DWORD dwAmount = 0;

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
		if( pItem && pItem->sDrugOp.emDrug == emDrug )
			dwAmount += pInvenItem->sItemCustom.wTurnNum;
	}

	return dwAmount;
}

DWORD GLInventory::GetAmountEmpty( bool bAllLine )
{
	WORD nCellSX = m_wCellSX;
	WORD nCellSY = GetValidCellY( bAllLine );

	DWORD dwNum = 0;

    for( WORD cy=0; cy<nCellSY; cy++ )
    {
		for( WORD cx=0; cx<nCellSX; cx++ )
		{
//             if( m_btBarrier[ cx ][ cy ] == EInvenBarrierEmpty )
//                 ++dwNum;

			BYTE nType;
			if( m_Barrier.GetInvenState( cx, cy, nType ) )
			{
				if( nType == EInvenBarrierEmpty )
				{
					++dwNum;
				}
			}
	    }
    }

	return dwNum;
}

DWORD GLInventory::GetAmountEmptyPile( const SNATIVEID& sNID, bool bAllLine )
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem ( sNID );
    if ( !pItem )
    {
        return 0;
    }
    if ( pItem->ISPILE() )
    {
        const DWORD nEmptyInven = GetAmountEmpty( bAllLine );    //인벤의 빈공간
        const WORD wPileNum = pItem->sDrugOp.wPileNum;  //해당아이템의 최대겹침 수량
        DWORD nCheckCnt = 0; //넣을수 있는 최대 수량
        if ( nEmptyInven > 0)
        {
            nCheckCnt += nEmptyInven * wPileNum;    //인벤에 빈공간이 있다면 (최대겹침수량 * 빈공간)
        }
        const DWORD nCntInven = CountItem( sNID );   //해당 아이템이 차지한 공간수
        if ( nCntInven > 0 )
        {
            const DWORD nCntPile = CountPileItem( sNID );    //해당 아이템 소지 수량
            const DWORD nEmptyPile =  ( nCntInven * wPileNum) - nCntPile;  //기존에 있는 공간에 더 쌓을수 있는 개수
            nCheckCnt += nEmptyPile;
        }
        return nCheckCnt;
    }
    else
    {
        return GetAmountEmpty( bAllLine );
    }
}

bool GLInventory::GetCharResetItem( WORD &wPosX, WORD &wPosY )
{
	DWORD dwAmount( 0 );
	SNATIVEID sNID_TAR( false );

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;

		const SNATIVEID &sNID = pInvenItem->sItemCustom.GetNativeID();
		const SITEM* pItem = GLogicData::GetInstance().GetItem( sNID );
		if( !pItem )
			continue;

		if( pItem->sBasicOp.emItemType!=ITEM_SKP_STAT_RESET_A &&
			pItem->sBasicOp.emItemType!=ITEM_SKP_STAT_RESET_B )
		{
			continue;
		}

		if( pItem->sDrugOp.wPileNum == pInvenItem->sItemCustom.wTurnNum )
		{
			wPosX = pInvenItem->wPosX;
			wPosY = pInvenItem->wPosY;

			return true;
		}
	}

	return false;
}

bool GLInventory::GetPileItem( const SNATIVEID _sNID, WORD &wPosX, WORD &wPosY )
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;

		const SNATIVEID &sNID = pInvenItem->sItemCustom.GetNativeID();
		if( _sNID != sNID )
			continue;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( sNID );
		if( pItem && pItem->sDrugOp.wPileNum == pInvenItem->sItemCustom.wTurnNum )
		{
			wPosX = pInvenItem->wPosX;
			wPosY = pInvenItem->wPosY;

			return true;
		}
	}

	return false;
}

WORD GLInventory::GetValidCellY( bool bAllLine ) const
{
	if( bAllLine || !m_bLimit )
		return m_wCellSY;

	WORD wValidY = GLCONST_CHAR::wInvenDefaultLine + m_wADDLINE;
	if( wValidY > m_wCellSY )
		wValidY = m_wCellSY;

	return wValidY;
}

WORD GLInventory::GETAddLine() const
{
	return m_wADDLINE;
}

BOOL GLInventory::IsInsertable( const WORD wPosX, const WORD wPosY, bool bLOAD )
{
	if( wPosX >= m_wCellSX )
		return FALSE;

	WORD wVALIDY = GetValidCellY( bLOAD );
	if( wPosY >= wVALIDY )
		return FALSE;

// 	if( m_btBarrier[ wPosX ][ wPosY ] != EInvenBarrierEmpty )
//         return FALSE;

	BYTE nType;
	if( m_Barrier.GetInvenState( wPosX, wPosY, nType ) )
		if( nType == EInvenBarrierEmpty )
			return TRUE;

	return FALSE;
}

//! 초기 로드시에( Assign ) bLOAD 를 사용하면
//! 사용 허가된 line 이 아니더라도 넣어질수 있다. )
BOOL GLInventory::InsertItem( const SITEMCUSTOM &ItemCustom, const WORD wPosX, const WORD wPosY, bool bLOAD, bool bNew )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem( ItemCustom.GetNativeID() );
	if( !pItem )
		return FALSE;

	BOOL bOk = IsInsertable(wPosX, wPosY, bLOAD);
	if (!bOk)
		return FALSE;

	// 인벤 공간을 마크함.
	SetBarrier( EInvenBarrierUsed, wPosX, wPosY );

	//SINVENITEM* pInvenItem = new SINVENITEM;
	SINVENITEM* pInvenItem = GLItemMan::GetInstance().m_poolSINVENITEM.construct();
	pInvenItem->wPosX = wPosX;
	pInvenItem->wPosY = wPosY;
	pInvenItem->sItemCustom = ItemCustom;
	pInvenItem->bNew = bNew;
	m_ItemMap.insert(CELL_MAP_VALUE(CELL_KEY(wPosX, wPosY), pInvenItem));

	return TRUE;
}

SINVENITEM* GLInventory::InsertItemNew( const SITEMCUSTOM& ItemCustom, const WORD wPosX, const WORD wPosY, bool bLOAD /*= false*/ )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem( ItemCustom.GetNativeID() );
	if ( !pItem )
		return NULL;

	BOOL bOk = IsInsertable( wPosX, wPosY, bLOAD );
	if (!bOk)
		return NULL;

	// 인벤 공간을 마크함.
	SetBarrier( EInvenBarrierUsed, wPosX, wPosY );

	SINVENITEM* pInvenItem = GLItemMan::GetInstance().m_poolSINVENITEM.construct();
	pInvenItem->wPosX = wPosX;
	pInvenItem->wPosY = wPosY;
	pInvenItem->sItemCustom = ItemCustom;
	std::pair< CELL_MAP_ITER, bool > Success = m_ItemMap.insert( CELL_MAP_VALUE( CELL_KEY( wPosX, wPosY ), pInvenItem ) );
	if ( Success.second == true )
	{
		return pInvenItem;
	}
	else
	{
		return NULL;
	}
}

BOOL GLInventory::InsertItem(
    const SITEMCUSTOM& ItemCustom,
    WORD wPosX,
    WORD wPosY,
    WORD wBackX,
    WORD wBackY)
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem(ItemCustom.GetNativeID());
	if (!pItem)
		return FALSE;

	BOOL bOk = IsInsertable(wPosX, wPosY);
	if (!bOk)
		return FALSE;

	// 인벤 공간을 마크함.
	SetBarrier(EInvenBarrierUsed, wPosX, wPosY);

	//SINVENITEM* pInvenItem = new SINVENITEM;
	SINVENITEM* pInvenItem = GLItemMan::GetInstance().m_poolSINVENITEM.construct();
	pInvenItem->wPosX = wPosX;
	pInvenItem->wPosY = wPosY;
	pInvenItem->wBackX = wBackX;
	pInvenItem->wBackY = wBackY;
	pInvenItem->sItemCustom = ItemCustom;
	m_ItemMap.insert(CELL_MAP_VALUE(CELL_KEY(wPosX, wPosY), pInvenItem));
	
	return TRUE;
}

BOOL GLInventory::InsertItem(const SITEMCUSTOM& ItemCustom, bool Force)
{
    SINVENITEM pInvenItem;
    return InsertItemGetInvenItem(ItemCustom, pInvenItem, Force);
}

BOOL GLInventory::InsertItemGetPos(
    const SITEMCUSTOM& ItemCustom,
    WORD& wOutPosX,
    WORD& wOutPosY,
    bool Force)
{
    SINVENITEM pInvenItem;
    if (InsertItemGetInvenItem( ItemCustom, pInvenItem, Force ) == FALSE)
        return FALSE;

	wOutPosX = pInvenItem.wPosX;
    wOutPosY = pInvenItem.wPosY;

	return TRUE;
}

BOOL GLInventory::InsertItemGetInvenItem( const SITEMCUSTOM& ItemCustom, SINVENITEM& pInvenItem, bool Force /*=false*/ )
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem(ItemCustom.GetNativeID());
    if (!pItem)
        return FALSE;

    WORD wPosX, wPosY;
    BOOL bOk = FindInsertable(wPosX, wPosY, Force);
    if (!bOk)
        return FALSE;

    // 인벤 공간을 마크함
    SetBarrier(EInvenBarrierUsed, wPosX, wPosY);

    //SINVENITEM* pTmpInvenItem = new SINVENITEM;
	SINVENITEM* pTmpInvenItem = GLItemMan::GetInstance().m_poolSINVENITEM.construct();
    pTmpInvenItem->wPosX = wPosX;
    pTmpInvenItem->wPosY = wPosY;
    pTmpInvenItem->sItemCustom = ItemCustom;
    m_ItemMap.insert(CELL_MAP_VALUE(CELL_KEY(wPosX,wPosY), pTmpInvenItem));

    pInvenItem = *pTmpInvenItem;

    return TRUE;
}

BOOL GLInventory::InsertItemEx( const SITEMCUSTOM &ItemCustom )
{
    std::vector<SINVENITEM> vecItem;
	return InsertItemExGetInvenItem( ItemCustom, vecItem );
}

BOOL GLInventory::InsertItemExGetInvenItem( const SITEMCUSTOM &ItemCustom , std::vector<SINVENITEM>& vecItem )
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem( ItemCustom.GetNativeID() );
    if( !pItem )
        return FALSE;

    //	겹침 아이템일 경우.
    WORD wPILENUM = pItem->sDrugOp.wPileNum;
    SNATIVEID sNID = pItem->sBasicOp.sNativeID;

    if( wPILENUM <= 0)
        return FALSE;

    //	넣기 요청된 아이템수.( 잔여량. )
    WORD wREQINSRTNUM = ( ItemCustom.wTurnNum );

    if ( GetAmountEmptyPile( sNID, FALSE ) < ItemCustom.wTurnNum )
    {
        //들어갈 공간이 없다는 뜻
        return FALSE;
    }

    const GLInventory::CELL_MAP &ItemMap = *GetItemList();
    GLInventory::CELL_MAP_CITER iter = ItemMap.begin();
    for( ; iter!=ItemMap.end(); ++iter )    //인벤을 돌면서 기존아이템에 겹침을 수행
    {
        SINVENITEM &sINVENITEM = *iter->second;
        SITEMCUSTOM &sITEMCUSTOM = sINVENITEM.sItemCustom;
        if( sITEMCUSTOM.GetNativeID() != sNID )
            continue;
        if( sITEMCUSTOM.wTurnNum>=wPILENUM )    //겹침아이템이 꽉찬 상태
            continue;

        WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum; //기존 아이템 겹침 여유분.
        if( wREQINSRTNUM > wSURPLUSNUM )    //겹침 여유분 자리가 모자랄때 (일단 채울수 있는 만큼 채우고 다시 for문 돔)
        {   
            sITEMCUSTOM.wTurnNum = wPILENUM;    // 기존 아이템의 겹침을 변경.( 꽉채움 )
            wREQINSRTNUM -= wSURPLUSNUM;    // 잔여 수량 감소.
            vecItem.push_back( sINVENITEM );
        }
        else
        {
            sITEMCUSTOM.wTurnNum += wREQINSRTNUM;   // 기존 아이템에 겹치는 것 만으로도 새로 넣을 아이템이 충분히 들어감.
            wREQINSRTNUM = 0;
            vecItem.push_back( sINVENITEM );
            return TRUE;
        }
    }
    if ( wREQINSRTNUM > 0 )
    {
        //인벤을 돌아다니면서 겹쳐도 수량이 남는다면 새인벤공간에 집어넣음
        WORD wNeedInven = wREQINSRTNUM / wPILENUM; //필요한 최대겹침 인벤공간 수
        WORD wSPLITNUM = wREQINSRTNUM % wPILENUM; //겹치고 남은 마지막 인벤
        if ( wSPLITNUM > 0 )
        {
            wNeedInven += 1;    //겹치고 남은 나머지 아이템들 공간이 1개 더 필요
        }
        else
        {
            wSPLITNUM = wPILENUM;
        }
        for( WORD i=0; i<wNeedInven; ++i )
        {
            // 새로운 아이템을 넣어줌.
            SITEMCUSTOM sITEMCUSTOM = ItemCustom;
            if ( ( i + 1 ) == wNeedInven )  //마지막 겹치고 남은 나머지 아이템
            {
                sITEMCUSTOM.wTurnNum = wSPLITNUM;
            }
            else
            {
                sITEMCUSTOM.wTurnNum = wPILENUM;	//꽉찬 아이템.
            }

            //	인밴토리에 넣습니다.
            SINVENITEM pInvenItem;
            if ( InsertItemGetInvenItem( sITEMCUSTOM, pInvenItem ) == FALSE )
            {
                //인벤에 삽입이 실패(위에서 한번 검사를 했기때문에 여기로 들어오면 안됨)
                return FALSE;
            }
            vecItem.push_back( pInvenItem );
        }
    }
    return TRUE;
}

BOOL GLInventory::DeleteItem( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = GetItem( wPosX, wPosY );
	if( !pInvenItem )
		return FALSE;

	// 인벤 공간 마크를 풀어줌.
	ResetBarrier( EInvenBarrierUsed, wPosX, wPosY );

	CELL_MAP_ITER iter_erase = m_ItemMap.find( CELL_KEY(wPosX,wPosY) );
	if( iter_erase != m_ItemMap.end() )
		m_ItemMap.erase( iter_erase );

	//SAFE_DELETE( pInvenItem );
	GLItemMan::GetInstance().m_poolSINVENITEM.destroy( pInvenItem );

	return TRUE;
}

/*BOOL GLInventory::DeleteItem( WORD wPosX, WORD wPosY, LONGLONG GenNum )
{
    SINVENITEM* pInvenItem = GetItem( wPosX, wPosY );
    if (!pInvenItem)
        return FALSE;

    if (pInvenItem->sItemCustom.lnGenNum != GenNum)
        return FALSE;

    // 인벤 공간 마크를 풀어줌.
    ResetBarrier( EInvenBarrierUsed, wPosX, wPosY );

    CELL_MAP_ITER iter_erase = m_ItemMap.find( CELL_KEY(wPosX,wPosY) );
    if( iter_erase != m_ItemMap.end() )
        m_ItemMap.erase( iter_erase );

    //SAFE_DELETE( pInvenItem );
	GLItemMan::GetInstance().m_poolSINVENITEM.destroy( pInvenItem );

    return TRUE;
}*/

void GLInventory::DeleteItemAll()
{
	if( !m_ItemMap.empty() )
	{
		//std::for_each( m_ItemMap.begin(), m_ItemMap.end(), std_afunc::DeleteMapObject() );
		for ( CELL_MAP_ITER iter = m_ItemMap.begin(); iter!=m_ItemMap.end(); ++iter )
		{
			GLItemMan::GetInstance().m_poolSINVENITEM.destroy( (*iter).second );
		}
		m_ItemMap.clear();
	}

	//memset( m_btBarrier, EInvenBarrierEmpty, sizeof( m_btBarrier ) );
	m_Barrier.Clear();
}

void GLInventory::DeleteItem( const SNATIVEID& sNID, DWORD& dwDeletedCount )
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; )
	{
		SINVENITEM* pInvenItem = iter->second;
		if ( pInvenItem && pInvenItem->sItemCustom.GetNativeID() == sNID )
		{
			dwDeletedCount += pInvenItem->sItemCustom.wTurnNum;
			ResetBarrier( EInvenBarrierUsed, pInvenItem->wPosX, pInvenItem->wPosY );
			iter = m_ItemMap.erase( iter );
			//SAFE_DELETE( pInvenItem );
			GLItemMan::GetInstance().m_poolSINVENITEM.destroy( pInvenItem );
		}
		else
		{
			iter++;
		}
	}
}

SINVENITEM* GLInventory::GetItem(WORD wPosX, WORD wPosY)
{
	CELL_MAP_CITER iter = m_ItemMap.find(CELL_KEY(wPosX,wPosY));
	if( iter != m_ItemMap.end() )
        return iter->second;

	return NULL;
}

SINVENITEM* GLInventory::GetItem(WORD wPosX, WORD wPosY) const
{
    CELL_MAP_CITER iter = m_ItemMap.find(CELL_KEY(wPosX,wPosY));
    if( iter != m_ItemMap.end() )
        return iter->second;

	return NULL;
}

BOOL GLInventory::SETITEM_BYBUFFER( se::ByteStream &ByteStream )
{
	if( ByteStream.IsEmpty() )
		return TRUE;

	DeleteItemAll();

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + SINVENITEM_ARRAY[SIZE]
	DWORD dwVersion, dwSize, dwNum;
	
	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;
	
	SINVENITEM sInvenItem;
	for( DWORD i=0; i<dwNum; i++ )
	{
		// 향후 버전에서 이전 버전을 대입 연산으로 처리 할 부분.
		if( dwVersion==0x0100 )
		{
			SINVENITEM_SAVE_100 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
				sc::writeLogError(
                    sc::string::format(
                        "inventory save byte stream version %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion==0x0101 )
		{
			SINVENITEM_SAVE_101 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
				sc::writeLogError(
                    std::string(
                        "inventory SETITEM_BYBUFFER ReadBuffer"));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion==0x0102 )
		{
			SINVENITEM_SAVE_102 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
				sc::writeLogError(
                    sc::string::format(
                        "inventory SETITEM_BYBUFFER version %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion==0x0103 )
		{
			SINVENITEM_SAVE_103 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
				sc::writeLogError(
                    sc::string::format(
                        "inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion==0x0104 )
		{
			SINVENITEM_SAVE_104 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
				sc::writeLogError(
                    sc::string::format(
                        "inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion==0x0105 )
		{
			SINVENITEM_SAVE_105 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
				sc::writeLogError(
                    sc::string::format(
                        "inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion==0x0106 )
		{
			SINVENITEM_SAVE_106 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
				sc::writeLogError(
                    sc::string::format(
                        "inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion==0x0107 )
		{
			SINVENITEM_SAVE_107 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
                sc::writeLogError(
                    sc::string::format(
                        "inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion==0x0108 )
		{
			SINVENITEM_SAVE_108 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
                sc::writeLogError(
                    sc::string::format(
                        "inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion==0x0109 )
		{
			SINVENITEM_SAVE_109 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
                sc::writeLogError(
                    sc::string::format(
                        "inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion==0x0110 )
		{
			SINVENITEM_SAVE_110 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
                sc::writeLogError(
                    sc::string::format(
                        "inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion==0x0111)
		{
			SINVENITEM_SAVE_111 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
                sc::writeLogError(
                    sc::string::format(
                        "inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if( dwVersion==0x0112)
		{
			SINVENITEM_SAVE_112 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
                sc::writeLogError(
                    sc::string::format(
                        "inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
        else if( dwVersion==0x0113)
        {
            SINVENITEM_SAVE_113 sOld;
            GASSERT(dwSize==sizeof(sOld));
            BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
            if( !bOk )
            {
                sc::writeLogError(
                    sc::string::format(
                    "inventory SETITEM_BYBUFFER %1%", dwVersion));
                return FALSE;	//ERROR
            }

            sInvenItem.sSaveData.Assign( sOld );
            InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

        }
		else if( dwVersion==0x0114)
		{
			SINVENITEM_SAVE_114 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if( dwVersion==0x0115)
		{
			SINVENITEM_SAVE_115 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
        else if( dwVersion==0x0116)
        {
            SINVENITEM_SAVE_116 sOld;
            GASSERT(dwSize==sizeof(sOld));
            BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
            if( !bOk )
            {
                sc::writeLogError(
                    sc::string::format(
                    "inventory SETITEM_BYBUFFER %1%", dwVersion));
                return FALSE;	//ERROR
            }

            sInvenItem.sSaveData.Assign( sOld );
            InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

        }
		else if( dwVersion==0x0117)
		{
			SINVENITEM_SAVE_117 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if( dwVersion==0x0118)
		{
			SINVENITEM_SAVE_118 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if( dwVersion==0x0119)
		{
			SINVENITEM_SAVE_119 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sOld, sizeof(sOld) );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"inventory SETITEM_BYBUFFER %1%", dwVersion));
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if( dwVersion==SINVENITEM_SAVE::VERSION )
		{
			GASSERT(dwSize==sizeof(SINVENITEM_SAVE));
			BOOL bOk = ByteStream.ReadBuffer((LPBYTE)&sInvenItem.sSaveData, sizeof(SINVENITEM_SAVE) );
			if( !bOk )
			{
				//GASSERT( "inventory save byte stream 처리 도중 오류가 발생하였습니다." && 0 );
                sc::writeLogError(
                    sc::string::format(
                        "SETITEM_BYBUFFER - error has occured during read byte-stream of inventory(%1%)", dwVersion));
				return FALSE;	//ERROR
			}

			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else
		{
			// 알수 없는 버전의 inventory save structure 입니다.
			//
			//GASSERT( "알수 없는 버전의 inventory save structure 입니다." && 0 );
            sc::writeLogError(sc::string::format("SETITEM_BYBUFFER - wrong version(%1%) of inventory", dwVersion));
			return FALSE;
		}
	}

	return TRUE;
}

BOOL GLInventory::SETITEM_BYBUFFER_FOR_MIGRATION( se::ByteStream &ByteStream )
{
	if( ByteStream.IsEmpty() )
		return TRUE;

	DeleteItemAll();

	// DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + SINVENITEM_ARRAY[SIZE]
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	SINVENITEM sInvenItem;
	for( DWORD i = 0; i < dwNum; i++ )
	{
		// 향후 버전에서 이전 버전을 대입 연산으로 처리 할 부분.
		if( dwVersion == 0x0100 )
		{
			SINVENITEM_SAVE_100 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory save byte stream version %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion == 0x0101 )
		{
			SINVENITEM_SAVE_101 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					std::string(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER ReadBuffer"));
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion == 0x0102 )
		{
			SINVENITEM_SAVE_102 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER version %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion == 0x0103 )
		{
			SINVENITEM_SAVE_103 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion == 0x0104 )
		{
			SINVENITEM_SAVE_104 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion == 0x0105 )
		{
			SINVENITEM_SAVE_105 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion == 0x0106 )
		{
			SINVENITEM_SAVE_106 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion == 0x0107 )
		{
			SINVENITEM_SAVE_107 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion == 0x0108 )
		{
			SINVENITEM_SAVE_108 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion == 0x0109 )
		{
			SINVENITEM_SAVE_109 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion == 0x0110 )
		{
			SINVENITEM_SAVE_110 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if( dwVersion == 0x0111)
		{
			SINVENITEM_SAVE_111 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if( dwVersion == 0x0112)
		{
			SINVENITEM_SAVE_112 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if( dwVersion == 0x0113)
		{
			SINVENITEM_SAVE_113 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if( dwVersion == 0x0114)
		{
			SINVENITEM_SAVE_114 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if( dwVersion == 0x0115)
		{
			SINVENITEM_SAVE_115 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if( dwVersion == 0x0116)
		{
			SINVENITEM_SAVE_116 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if( dwVersion == 0x0117)
		{
			SINVENITEM_SAVE_117 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if( dwVersion == 0x0118)
		{
			SINVENITEM_SAVE_118 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if( dwVersion == 0x0119)
		{
			SINVENITEM_SAVE_119 sOld;
			GASSERT( dwSize == sizeof( sOld ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sOld, sizeof( sOld )  );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION inventory SETITEM_BYBUFFER %1%", dwVersion ) );
				return FALSE;
			}

			sOld.sItemCustom.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sOld.sItemCustom.nidDISGUISE )
			{
				sOld.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			sInvenItem.sSaveData.Assign( sOld );
			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if( dwVersion == SINVENITEM_SAVE::VERSION )
		{
			// 아이템 버전이 올라가도 xxx_migration(MIGRATION) 함수들은 수정할 필요 없음
			GASSERT( dwSize == sizeof( SINVENITEM_SAVE ) );
			BOOL bOk = ByteStream.ReadBuffer( ( LPBYTE )&sInvenItem.sSaveData, sizeof( SINVENITEM_SAVE ) );
			if( !bOk )
			{
				sc::writeLogError(
					sc::string::format(
					"SETITEM_BYBUFFER_FOR_MIGRATION - error has occured during read byte-stream of inventory(%1%)", dwVersion ) );
				return FALSE;
			}

			SNATIVEID ItemID = sInvenItem.sItemCustom.GetRealNativeID();
			ItemID.wMainID += 3000;
			sInvenItem.sItemCustom.SetNativeID( ItemID );
			if ( NATIVEID_NULL() != sInvenItem.sItemCustom.nidDISGUISE )
			{
				sInvenItem.sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			InsertItem( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else
		{
			sc::writeLogError( sc::string::format( "SETITEM_BYBUFFER_FOR_MIGRATION - wrong version(%1%) of inventory", dwVersion ) );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL GLInventory::ValidateVehicleRandomOption()
{
	BOOL Result = TRUE;
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; iter++ )
	{
		SINVENITEM* pInvenItem = iter->second;
		if ( !pInvenItem )
		{
			continue;
		}

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->GetNativeID() );
		if ( !pItem )
		{
			continue;
		}

		if ( pItem->sBasicOp.emItemType != ITEM_VEHICLE )
		{
			continue;
		}

		// 인첸트 초기화
		pInvenItem->sItemCustom.cDAMAGE = 0;
		pInvenItem->sItemCustom.cDEFENSE = 0;

		if ( !pInvenItem->sItemCustom.IsOpItem() )
		{
			continue;
		}

		RandomOpt option;
		pInvenItem->sItemCustom.randomOption = option;
		Result = FALSE;

		sc::writeLogError( sc::string::format( 
			"ValidateVehicleRandomOption mid %1% sid %2%", 
			pInvenItem->sItemCustom.GetNativeID().wMainID, 
			pInvenItem->sItemCustom.GetNativeID().wSubID ) );
	}
	return Result;
}

BOOL GLInventory::ValidateWrappedItem( std::vector< SITEMCUSTOM >& vecPostItems, std::vector< SINVENITEM* >& vecUnWrappedItems )
{
	// 태국 전용
	// http://172.16.2.87/redmine/issues/2734
	CTime Time( CTime::GetCurrentTime() );
	BOOL Result = TRUE;
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; iter++ )
	{
		SINVENITEM* pInvenItem = iter->second;
		if ( !pInvenItem )
		{
			continue;
		}

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->GetNativeID() );
		if ( !pItem )
		{
			continue;
		}

		WORD TurnNum = pInvenItem->sItemCustom.wTurnNum;
		// wTurnNum 이 2개 이상인 것만 체크한다.
		if ( TurnNum < 2 )
		{
			continue;
		}

		// IN_011_043 / IN_011_044  / IN_011_045 / IN_011_061 / IN_011_062 / IN_011_063
		// GetNativeID : 포장되어 있어도 원래 아이템 아이디가 리턴된다.
		SNATIVEID ItemID = pInvenItem->sItemCustom.GetNativeID();
		if ( 
			SNATIVEID( 11, 43 ) == ItemID ||
			SNATIVEID( 11, 44 ) == ItemID ||
			SNATIVEID( 11, 45 ) == ItemID ||
			SNATIVEID( 11, 61 ) == ItemID ||
			SNATIVEID( 11, 62 ) == ItemID ||
			SNATIVEID( 11, 63 ) == ItemID
			)
		{
			// 포장되어 있다면 포장지 1장 우편 전송할 아이템 벡터에 삽입
			if ( pInvenItem->sItemCustom.IsBoxWrapped() )
			{
				// 포장지 아이템 생성, 국가 마다 다르다. 아이템 아이디 주의
				// IN_420_040 : 노란색의 포장지
				// IN_420_042 : 빨간색의 포장지
				SNATIVEID Wrapper( 420, 40 );
				const SITEM* pItemWrapper = GLogicData::GetInstance().GetItem( Wrapper );
				if ( pItemWrapper && pItemWrapper->sBasicOp.emItemType == ITEM_BOX_WRAPPER )
				{
					SITEMCUSTOM PostItem( Wrapper );
					PostItem.wTurnNum = 1;
					PostItem.BornTimeSet( Time.GetTime() );
					PostItem.GenTypeSet( EMGEN_SHOP );
					sc::SeqUniqueGUID::Instance()->getGUID( PostItem.guid );
					PostItem.GenerateBasicStat( false );
					PostItem.GenerateLinkSkill();
					PostItem.GenerateAddOption();
					PostItem.SetDbState( db::DB_INSERT );
					vecPostItems.push_back( PostItem );

					// 포장 풀기
					pInvenItem->sItemCustom.BoxUnwrapping();
					vecUnWrappedItems.push_back( pInvenItem );

					sc::writeLogInfo( sc::string::format( 
						"ValidateWrappedItem wrapper item for post send mid %1% sid %2%", 
						Wrapper.wMainID, 
						Wrapper.wSubID ) );

					Result = FALSE;
				}
				else
				{
					sc::writeLogError( sc::string::format( 
						"ValidateWrappedItem wrapper item is not ITEM_BOX_WRAPPER %1% sid %2%", 
						Wrapper.wMainID, 
						Wrapper.wSubID ) );

					return FALSE;
				}
			}

			// 아이템 1개만 남기고 나머지는 우편 전송할 아이템 벡터에 삽입
			WORD LoopMax = TurnNum - 1;
			for ( WORD loop = 0; loop < LoopMax; loop++ )
			{
				// 시효성이라던지 그외 속성 그대로 넣어주려고 SITEMCUSTOM 그대로 넣어준다.
				SITEMCUSTOM PostItem( pInvenItem->sItemCustom );
				PostItem.wTurnNum = 1;
				sc::SeqUniqueGUID::Instance()->getGUID( PostItem.guid );
				PostItem.GenerateBasicStat( false );
				PostItem.GenerateLinkSkill();
				PostItem.GenerateAddOption();
				PostItem.SetDbState( db::DB_INSERT, true );
				vecPostItems.push_back( PostItem );

				sc::writeLogInfo( sc::string::format( 
					"ValidateWrappedItem item for post send mid %1% sid %2%", 
					PostItem.GetNativeID().wMainID, 
					PostItem.GetNativeID().wSubID ) );

				Result = FALSE;
			}
			
			// 수량 1로 변경
			pInvenItem->sItemCustom.wTurnNum = 1;
		}
	}
	return Result;
}

BOOL GLInventory::ValidateRandomOption( std::vector< SITEMCUSTOM >& vecDeleteItems )
{
	BOOL Result = TRUE;
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; )
	{
		SINVENITEM* pInvenItem = iter->second;
		if ( !pInvenItem )
		{
			iter++;
			continue;
		}

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->GetNativeID() );
		if ( !pItem )
		{
			iter++;
			continue;
		}

		if ( !pInvenItem->sItemCustom.IsOpItem() )
		{
			iter++;
			continue;
		}

		Result = FALSE;

		sc::writeLogInfo( sc::string::format( 
			"ValidateRandomOption item delete mid %1% sid %2%", 
			pInvenItem->GetNativeID().wMainID, 
			pInvenItem->GetNativeID().wSubID ) );

		vecDeleteItems.push_back( pInvenItem->sItemCustom );

		ResetBarrier( EInvenBarrierUsed, pInvenItem->wPosX, pInvenItem->wPosY );
		iter = m_ItemMap.erase( iter );
		GLItemMan::GetInstance().m_poolSINVENITEM.destroy( pInvenItem );
	}
	return Result;
}

BOOL GLInventory::SET_ITEM_BY_VECTOR( const std::vector< SINVENITEM_SAVE >& vecItem )
{
	DeleteItemAll();

	size_t nItemNum = vecItem.size();
	for( size_t i = 0; i < nItemNum; ++i )
	{
		const SINVENITEM_SAVE& Item = vecItem[i];
		InsertItem( Item.sItemCustom, Item.wPosX, Item.wPosY, true );
	}

	return TRUE;
}

BOOL GLInventory::GET_ITEM_BY_VECTOR( std::vector< SINVENITEM_SAVE >& vecItem )
{
	CELL_MAP_CITER iter = m_ItemMap.begin();
	CELL_MAP_CITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = iter->second;
		vecItem.push_back( pInvenItem->sSaveData );
	}

	return TRUE;
}

DWORD GLInventory::GetItemDataSize()
{
	DWORD		dwDataSize = 0;
	dwDataSize += sizeof( SINVENITEM_SAVE::VERSION );
	dwDataSize += sizeof( DWORD );		// sizeof(SINVENITEM_SAVE)

	dwDataSize += sizeof( DWORD );		// m_ItemMap.size();
	dwDataSize += sizeof( SINVENITEM_SAVE ) * m_ItemMap.size();

	return dwDataSize;
}

/**
 * 함수 수정시 연관성 있는 GetItemDataSize도 수정필요!!!
 */
BOOL GLInventory::GetItemData( se::SerializeWritableData& data )
{
	data << SINVENITEM_SAVE::VERSION
		 << (DWORD)sizeof(SINVENITEM_SAVE)
		 << (DWORD)m_ItemMap.size();

	CELL_MAP_CITER		iter	 = m_ItemMap.begin();
	CELL_MAP_CITER		iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM*		pInvenItem = iter->second;
		data.Write( &pInvenItem->sSaveData, sizeof( SINVENITEM_SAVE ) );
	}

	return TRUE;
}

/**
 * 함수 수정시 연관성 있는 GetItemDataSize도 수정필요!!!
 */
BOOL GLInventory::GETITEM_BYBUFFER( se::ByteStream &ByteStream ) const
{
	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + SINVENITEM_ARRAY[SIZE]
	ByteStream << SINVENITEM_SAVE::VERSION;
	ByteStream << (DWORD)sizeof(SINVENITEM_SAVE);

	DWORD dwSize = (DWORD) m_ItemMap.size();
	ByteStream << dwSize;

	CELL_MAP_CITER iter = m_ItemMap.begin();
	CELL_MAP_CITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; ++iter )
	{
		SINVENITEM *pInvenItem = iter->second;

		ByteStream.WriteBuffer( (LPBYTE)&pInvenItem->sSaveData, (DWORD)sizeof( SINVENITEM_SAVE ) );
	}
	
	return TRUE;
}

DWORD GLInventory::GetItemStorageDataSize()
{
	DWORD		dwDataSize = 0;
	
	dwDataSize += sizeof( SINVENITEM_SAVE::VERSION );
	dwDataSize += sizeof( DWORD );		//sizeof(SINVENITEM_SAVE);
	dwDataSize += sizeof( DWORD );		//m_ItemMap.size();
	dwDataSize += sizeof( SINVENITEM_SAVE ) * m_ItemMap.size();

	return dwDataSize;
}

BOOL GLInventory::GetItemStorageData( se::SerializeWritableData& data )
{
	data << SINVENITEM_SAVE::VERSION
		 << (DWORD)sizeof(SINVENITEM_SAVE)
		 << (DWORD)m_ItemMap.size();

	CELL_MAP_CITER iter = m_ItemMap.begin();
	CELL_MAP_CITER iter_end = m_ItemMap.end();
	for ( ; iter != iter_end; ++iter)
	{
		SINVENITEM *pInvenItem = iter->second;
		data.Write( &pInvenItem->sSaveData, sizeof( SINVENITEM_SAVE ) );
	}

	return TRUE;
}

BOOL GLInventory::GETITEM_BYBUFFER_FORSTORAGE(se::ByteStream& ByteStream) const
{
	ByteStream << SINVENITEM_SAVE::VERSION;
	ByteStream << (DWORD) sizeof(SINVENITEM_SAVE);
	
	DWORD dwSize = (DWORD) m_ItemMap.size();
	ByteStream << dwSize;

	CELL_MAP_CITER iter = m_ItemMap.begin();
	CELL_MAP_CITER iter_end = m_ItemMap.end();
	for ( ; iter != iter_end; ++iter)
	{
		SINVENITEM *pInvenItem = iter->second;
		ByteStream.WriteBuffer((LPBYTE) &pInvenItem->sSaveData, (DWORD) sizeof(SINVENITEM_SAVE));
	}

	return TRUE;
}

void GLInventory::OptimalPile()
{
    CELL_MAP tempItemMap = m_ItemMap;
    RecusiveOptimalPile(tempItemMap);
    Aligning();
}

void GLInventory::OptimalPileServer( std::vector< DELETED_ITEM >& DeletedItems )
{
	CELL_MAP tempItemMap = m_ItemMap;
	RecusiveOptimalPileServer( tempItemMap, DeletedItems );
	Aligning();
}

void GLInventory::RecusiveOptimalPile(CELL_MAP& invenMap)
{
    typedef SINVENITEM* Data;
    typedef std::queue<Data> Container;
    typedef std::queue<Data> ContainerIt;

    DWORD wItemNum = 0;
    WORD wMaxPileNum = 0;
    Container CollectItems;

    /**
        겹칠 아이템과 총 갯수를 모은다.        

     */
    for (CELL_MAP_CITER it = invenMap.begin(); it != invenMap.end(); )
    {
        bool bErase = false;
        const SITEMCUSTOM& item = it->second->sItemCustom;
        const SITEM* pInvenData = GLogicData::GetInstance().GetItem(item.GetNativeID());

        if (IsPileItem(pInvenData, item))
        {
            if (CollectItems.empty() || 
                CollectItems.front()->GetNativeID() == item.GetNativeID())
            {
                CollectItems.push(it->second);
                wMaxPileNum = pInvenData->sDrugOp.wPileNum;
                wItemNum += item.wTurnNum;
                bErase = true;
            }
        }
        else
        {
            bErase = true;
        }

        if (bErase)
        {
            invenMap.erase(it++);
        }
        else
        {
            ++it;
        }
    }

    /**
        모은 아이템들을
        최대 겹친 갯수대로 분배 한다.

     */
    while (wItemNum > 0)
    {
        Data& PilingItem = CollectItems.front();

        if (wItemNum > wMaxPileNum)
        {
            PilingItem->sItemCustom.wTurnNum = wMaxPileNum;
            wItemNum -= wMaxPileNum;
        }
        else
        {
            PilingItem->sItemCustom.wTurnNum = static_cast< WORD >( wItemNum );
            wItemNum = 0;
        }

        CollectItems.pop();
    }

    /**
        분배 후 남은 것들은
        모두 삭제 한다.

     */
    while (CollectItems.empty() == false)
    {
        DeleteItem(CollectItems.front()->wPosX, CollectItems.front()->wPosY);
        CollectItems.pop();
    }

    /**
        모든 겹침 아이템을
        처리할 때까지 계속 한다.

     */
    if (wMaxPileNum > 0)
    {
        RecusiveOptimalPile(invenMap);
    }
}

void GLInventory::RecusiveOptimalPileServer( CELL_MAP& invenMap, std::vector< DELETED_ITEM >& DeletedItems )
{
    typedef SINVENITEM* Data;
    typedef std::queue<Data> Container;
    typedef std::queue<Data> ContainerIt;

    DWORD wItemNum = 0;
    WORD wMaxPileNum = 0;
    Container CollectItems;

    /**
        겹칠 아이템과 총 갯수를 모은다.        

     */
    for (CELL_MAP_CITER it = invenMap.begin(); it != invenMap.end(); )
    {
        bool bErase = false;
        const SITEMCUSTOM& item = it->second->sItemCustom;
        const SITEM* pInvenData = GLogicData::GetInstance().GetItem(item.GetNativeID());

        if (IsPileItem(pInvenData, item))
        {
            if (CollectItems.empty() || 
                CollectItems.front()->GetNativeID() == item.GetNativeID())
            {
                CollectItems.push(it->second);
                wMaxPileNum = pInvenData->sDrugOp.wPileNum;
                wItemNum += item.wTurnNum;
                bErase = true;
            }
        }
        else
        {
            bErase = true;
        }

        if (bErase)
        {
            invenMap.erase(it++);
        }
        else
        {
            ++it;
        }
    }

    /**
        모은 아이템들을
        최대 겹친 갯수대로 분배 한다.

     */
    while (wItemNum > 0)
    {
        Data& PilingItem = CollectItems.front();

        if (wItemNum > wMaxPileNum)
        {
            PilingItem->sItemCustom.wTurnNum = wMaxPileNum;
            wItemNum -= wMaxPileNum;
        }
        else
        {
            PilingItem->sItemCustom.wTurnNum = static_cast< WORD >( wItemNum );
            wItemNum = 0;
        }

        CollectItems.pop();
    }

    /**
        분배 후 남은 것들은
        모두 삭제 한다.

     */
    while (CollectItems.empty() == false)
    {
		DELETED_ITEM DeletedItem;
		DeletedItem.guid = CollectItems.front()->sItemCustom.guid;
		DeletedItem.DbState =  CollectItems.front()->sItemCustom.GetDbState();
		DeletedItems.push_back( DeletedItem );
        DeleteItem(CollectItems.front()->wPosX, CollectItems.front()->wPosY);
        CollectItems.pop();
    }

    /**
        모든 겹침 아이템을
        처리할 때까지 계속 한다.

     */
    if (wMaxPileNum > 0)
    {
        RecusiveOptimalPileServer(invenMap, DeletedItems);
    }
}

void GLInventory::Aligning()
{
    GLInventory tempInven(m_wCellSX, m_wCellSY);
    tempInven.SetAddLine(GETAddLine(), IsLimit());

    for (CELL_MAP_CITER it = m_ItemMap.begin(); it != m_ItemMap.end(); ++it)
    {
        tempInven.InsertItem(it->second->sItemCustom);
    }

    Assign(tempInven);
}

bool GLInventory::HasPileItem()
{
    CELL_MAP tempItemMap = m_ItemMap;
    return RecusiveHasPileItem(tempItemMap);
}

bool GLInventory::RecusiveHasPileItem(CELL_MAP& invenMap)
{
    int nPileCount = 0;
    SNATIVEID pileitem(NATIVEID_NULL());

    for (CELL_MAP_CITER it = invenMap.begin(); it != invenMap.end(); )
    {
        bool bErase = false;
        const SITEMCUSTOM& item = it->second->sItemCustom;
        const SITEM* pInvenData = GLogicData::GetInstance().GetItem(item.GetNativeID());

        if (IsPileItem(pInvenData, item))
        {
            if (pileitem == NATIVEID_NULL())
            {
                pileitem = item.GetNativeID();
                bErase = true;
            }
            else if (pileitem == item.GetNativeID())
            {
                bErase = true;
            }

            if (bErase && 
                (item.wTurnNum < pInvenData->sDrugOp.wPileNum || 
                (item.wTurnNum == pInvenData->sDrugOp.wPileNum && nPileCount > 0)) )
            {
                ++nPileCount;
            }
        }
        else
        {
            bErase = true;
        }

        if (nPileCount > 1)
        {
            return true;
        }

        if (bErase)
        {
            invenMap.erase(it++);
        }
        else
        {
            ++it;
        }
    }

    return (invenMap.empty() ? false : RecusiveHasPileItem(invenMap));
}

bool GLInventory::IsPileItem(const SITEM* pInvenData, const SITEMCUSTOM& sItem)
{
    if (pInvenData == 0)
    {
        return false;
    }

    // 1개 이상 겹칠 수 있어야 한다.
    if (pInvenData->sDrugOp.wPileNum <= 1)
    {
        return false;
    }

    // 최대 겹침 보다 더 겹쳐져 있다면(DB를 직접 수정했다면)
    // 겹침 아이템 에서 제외 시킨다.
    if (sItem.wTurnNum > pInvenData->sDrugOp.wPileNum)
    {
        return false;
    }

    return true;
}

DWORD GLInventory::SimulateDelSlotCnt( const SNATIVEID _sNID, WORD wDelCount )
{
	WORD dwEmptySlot  = 0;
	WORD dwDelCnt	  = wDelCount;

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();

	DWORD dwTotal = CountPileItem(_sNID);

	while( iter != iter_end )
	{
		SINVENITEM* pInvenItem = iter->second;

		const SNATIVEID &sNID = pInvenItem->sItemCustom.GetNativeID();

		if( _sNID == sNID )
		{
			const SITEM* pItem = GLogicData::GetInstance().GetItem( sNID );

			if( pItem && dwTotal >= wDelCount && dwDelCnt != 0 )
			{				
				if( pInvenItem->sItemCustom.wTurnNum <= dwDelCnt )
				{
					dwDelCnt -= pInvenItem->sItemCustom.wTurnNum;

					++dwEmptySlot;
				}	
				else
				if( pInvenItem->sItemCustom.wTurnNum > dwDelCnt )
				{
					dwDelCnt = 0;
				}
			}
		}
		else
		if( sNID == NATIVEID_NULL() )
		{
			++dwEmptySlot;
		}
		
		++iter;
	}
	
	return dwEmptySlot;
}

BOOL GLInventory::GetInvalidInventoryLineItem( std::vector< SINVENITEM* >& vecInvenItems, bool bUsePremium )
{
	BOOL Result = FALSE;
	WORD ValidCellY = GetValidCellY( false );
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for( ; iter != iter_end; iter++ )
	{
		SINVENITEM* pInvenItem = iter->second;
		if ( !pInvenItem )
		{
			continue;
		}

		// 부등호 주의, 프리미엄 라인을 사용하다가 만료된 만약의 경우를 위해서 프리미엄관련 계산 추가함
		if ( bUsePremium )
		{
			if ( pInvenItem->wPosY >= ValidCellY )
			{
				vecInvenItems.push_back( pInvenItem );
				Result = TRUE;
			}
		}
		else
		{
			if ( pInvenItem->wPosY > ValidCellY )
			{
				vecInvenItems.push_back( pInvenItem );
				Result = TRUE;
			}
		}
	}

	return Result;
}
