#include "../pch.h"
#include "GLInventorySale.h"

void GLInventorySale::push_exchangeData( WORD wCol, WORD wRow, const sExList* const pData )
{
	if( pData == NULL ) return;
	
	int iIndex = wRow*6 + wCol;

	m_ExChangeList[iIndex] = *pData;
}

void GLInventorySale::Assign( const GLInventory &Inven )
{
	GLInventory::Assign(Inven);

	GLInventory* pTemp = const_cast<GLInventory*>(&Inven);

	GLInventorySale* pInven = dynamic_cast<GLInventorySale*>(pTemp);

	if( pInven )
	{
		m_vSaleFuncType = pInven->m_vSaleFuncType;

		m_ExChangeList.clear();

		map_ExList_iter _iter = pInven->m_ExChangeList.begin();

		while( _iter != pInven->m_ExChangeList.end())
		{
			m_ExChangeList.insert( map_ExList_Value(_iter->first, _iter->second ));

			++_iter;
		}
	}
}

EEXTYPE GLInventorySale::GetExChangeType( const SNATIVEID& sId, WORD wCol, WORD wRow )
{
	int ikey = wRow*6 + wCol;

	map_ExList_iter _iter = m_ExChangeList.find( ikey );

	if( _iter == m_ExChangeList.end() || _iter->second.sNativeID != sId ) return EX_NONE;

	return _iter->second.exType;
}

EEXTYPE GLInventorySale::GetExchangeList( const SNATIVEID& sId, WORD wCol, WORD wRow, std::vector<sExNeed>& rList )
{
	int ikey = wRow*6 + wCol;

	map_ExList_iter _iter = m_ExChangeList.find( ikey );

	if( _iter == m_ExChangeList.end() || _iter->second.sNativeID != sId ) return EX_NONE;

	EEXTYPE eType = _iter->second.exType;

	sExList& exList = _iter->second;

	std::vector<sExNeed>::iterator _vIter = exList.vNeedItem.begin();

	switch( eType )
	{
	case EX_ITEM_TYPE  :
		{
			while(_vIter != exList.vNeedItem.end())
			{
				rList.push_back((*_vIter));

				++_vIter;
			}
		}
		break;
	case EX_POINT_TYPE :
		{
			while(_vIter != exList.vNeedItem.end())
			{
				rList.push_back((*_vIter));

				++_vIter;
			}
		}
		break;
	case EX_LVP_TYPE   :
		{
			while(_vIter != exList.vNeedItem.end())
			{
				rList.push_back((*_vIter));
				++_vIter;
			}
		}
		break;
	}

	return eType;
}

EEXTYPE GLInventorySale::IsBuyableItemList(IN VECBUYINVENITEM& vecItemlist, GLInventory* pUserinven
					   , __int64 nUserRanPoint, __int64 nUserLvPoint)
{
	std::map<SNATIVEID, sExNeed> mapAccumNeedItem;
	__int64 nNeedRanPoint(0), nNeedLivingPoint(0);

	VECBUYINVENITEM_ITR itr = vecItemlist.begin()
		, itrEnd = vecItemlist.end();
	for ( ; itr != itrEnd; ++itr )
	{
		const stBuyInvenItem& sBuyitem = *itr;
		const int nKey = sBuyitem.wPosY*6+sBuyitem.wPosX;
		map_ExList_iter iterExList = m_ExChangeList.find( nKey );
		if( iterExList == m_ExChangeList.end() ) 
			continue;

		sExList& exList = iterExList->second;
		std::vector<sExNeed>::iterator itrNeedItem = exList.vNeedItem.begin();
		switch (exList.exType)
		{
		case EX_ITEM_TYPE :
			{	
				DWORD dwCount = 0;

				std::map< SNATIVEID, sExNeed >::iterator itrAccumFindNeedItem;
				while(itrNeedItem != exList.vNeedItem.end())
				{
					sExNeed sNeedItem = *itrNeedItem;

					sNeedItem.iNeedCount = sNeedItem.iNeedCount*sBuyitem.wBuyNum;
					itrAccumFindNeedItem = mapAccumNeedItem.find( itrNeedItem->sNativeID );

					if( itrAccumFindNeedItem != mapAccumNeedItem.end() ) 
						itrAccumFindNeedItem->second.iNeedCount += sNeedItem.iNeedCount;
					else
						mapAccumNeedItem[itrNeedItem->sNativeID] = sNeedItem;

					++itrNeedItem;
				}

				// 아이템 체크;
				std::map<SNATIVEID, sExNeed>::iterator iterNeedItemCount = mapAccumNeedItem.begin();
				while( iterNeedItemCount != mapAccumNeedItem.end() )
				{
					const DWORD dwCount = pUserinven->CountPileItem( iterNeedItemCount->first );

					if( iterNeedItemCount->second.iNeedCount > dwCount )	
						return EX_ITEM_TYPE;

					++iterNeedItemCount;
				}
			}
			break;
		case EX_POINT_TYPE :
			{
				while(itrNeedItem != exList.vNeedItem.end())
				{
					nNeedRanPoint += itrNeedItem->iNeedCount*sBuyitem.wBuyNum;
					++itrNeedItem;
				}

				if( nNeedRanPoint > nUserRanPoint )
					return EX_POINT_TYPE;
			}
			break;
		case EX_LVP_TYPE   :
			{
				__int64 iCount = 0;

				while(itrNeedItem != exList.vNeedItem.end())
				{
					nNeedLivingPoint += itrNeedItem->iNeedCount*sBuyitem.wBuyNum;
					++itrNeedItem;
				}

				if( nNeedLivingPoint > nUserLvPoint )
					return EX_LVP_TYPE;
			}
			break;
		}
	}

	return EX_NONE;
}

bool GLInventorySale::IsNeedConditionHave( GLInventory* pUserinven, SNATIVEID& sId, WORD wCol, WORD wRow, __int64 iRanPoint64, __int64 iLvPoint64, WORD wCount )
{
	int ikey = wRow*6 + wCol;

	map_ExList_iter _iter = m_ExChangeList.find( ikey );

	if( _iter == m_ExChangeList.end() || _iter->second.sNativeID != sId ) return false;

	EEXTYPE eType = _iter->second.exType;
	
	sExList& exList = _iter->second;

	std::vector<sExNeed>::iterator _vIter = exList.vNeedItem.begin();

	switch( eType )
	{
	case EX_ITEM_TYPE  :
		{
			DWORD dwCount = 0;

			std::map< SNATIVEID, sExNeed > mTemp;
			std::map< SNATIVEID, sExNeed >::iterator miter;

			sExNeed sTemp;

			while(_vIter != exList.vNeedItem.end())
			{
				sTemp = *_vIter;

				sTemp.iNeedCount = sTemp.iNeedCount*wCount;

				miter = mTemp.find( _vIter->sNativeID );

				if( miter != mTemp.end() ) {
					miter->second.iNeedCount += sTemp.iNeedCount;
				}
				else {
					mTemp[_vIter->sNativeID] = sTemp;
				}

				++_vIter;
			}

			miter = mTemp.begin();

			while(miter != mTemp.end())
			{
				dwCount = pUserinven->CountPileItem( miter->first );

				if( miter->second.iNeedCount > dwCount )	return false;

				++miter;
			}
		}
		break;
	case EX_POINT_TYPE :
		{
			__int64 iCount = 0;

			while(_vIter != exList.vNeedItem.end())
			{
				iCount = _vIter->iNeedCount*wCount;

				if( iCount > iRanPoint64 )	return false;

				++_vIter;
			}
		}
		break;
	case EX_LVP_TYPE   :
		{
			__int64 iCount = 0;

			while(_vIter != exList.vNeedItem.end())
			{
				iCount = _vIter->iNeedCount*wCount;

				if( iCount > iLvPoint64 )	return false;

				++_vIter;
			}
		}
		break;
	}
	// 필요아이템 총갯수	

	return true;

}

bool GLInventorySale::IsNeedItemCondition( std::vector<sExNeed>& rList, GLInventory* pUserinven, SNATIVEID& sId, WORD wCol, WORD wRow )
{
	int ikey = wRow*6 + wCol;

	map_ExList_iter _iter = m_ExChangeList.find( ikey );

	if( _iter == m_ExChangeList.end() || _iter->second.sNativeID != sId ) return false;

	EEXTYPE eType = _iter->second.exType;

	sExList& exList = _iter->second;

	std::vector<sExNeed>::iterator _vIter = exList.vNeedItem.begin();

	switch( eType )
	{
	case EX_ITEM_TYPE  :
		{			
			DWORD dwCount = 0;

			std::map< SNATIVEID, sExNeed > mTemp;
			std::map< SNATIVEID, sExNeed >::iterator miter;

			while(_vIter != exList.vNeedItem.end())
			{
				miter = mTemp.find( _vIter->sNativeID );

				if( miter != mTemp.end() ) {
					miter->second.iNeedCount += _vIter->iNeedCount;
				}
				else {
					mTemp[_vIter->sNativeID] = (*_vIter);
				}

				rList.push_back(*_vIter);

				++_vIter;
			}

			miter = mTemp.begin();

			while(miter != mTemp.end())
			{
				dwCount = pUserinven->CountPileItem( miter->first );

				if( miter->second.iNeedCount > dwCount )	return false;

				++miter;
			}
		}
		break;
	case EX_POINT_TYPE :
	case EX_LVP_TYPE   :
		return false;
	}

	return true;
}

bool GLInventorySale::IsNeedRanPointCondition( std::vector<sExNeed>& rList, GLInventory* pUserinven, SNATIVEID& sId, WORD wCol, WORD wRow, __int64 iRanPoint64 )
{
	int ikey = wRow*6 + wCol;

	map_ExList_iter _iter = m_ExChangeList.find( ikey );

	if( _iter == m_ExChangeList.end() || _iter->second.sNativeID != sId ) return false;

	EEXTYPE eType = _iter->second.exType;

	sExList& exList = _iter->second;

	std::vector<sExNeed>::iterator _vIter = exList.vNeedItem.begin();

	bool bResult = true;

	switch( eType )
	{
	case EX_POINT_TYPE :
		{			
			while(_vIter != exList.vNeedItem.end())
			{
				if( _vIter->iNeedCount > iRanPoint64 )	bResult = false;

				rList.push_back(*_vIter);

				++_vIter;
			}
		}
		break;
	case EX_ITEM_TYPE  :	
	case EX_LVP_TYPE   :
		return false;
	}

	return bResult;
}

bool GLInventorySale::IsNeedLvPointCondition( std::vector<sExNeed>& rList, GLInventory* pUserinven, SNATIVEID& sId, WORD wCol, WORD wRow, __int64 iLvPoint64 )
{
	int ikey = wRow*6 + wCol;

	map_ExList_iter _iter = m_ExChangeList.find( ikey );

	if( _iter == m_ExChangeList.end() || _iter->second.sNativeID != sId ) return false;

	EEXTYPE eType = _iter->second.exType;

	sExList& exList = _iter->second;

	std::vector<sExNeed>::iterator _vIter = exList.vNeedItem.begin();

	bool bResult = true;

	switch( eType )
	{
	case EX_LVP_TYPE   :
		{
			while(_vIter != exList.vNeedItem.end())
			{
				if( _vIter->iNeedCount > iLvPoint64 )	bResult = false;

				rList.push_back(*_vIter);

				++_vIter;
			}
		}
		break;
	case EX_POINT_TYPE :
	case EX_ITEM_TYPE  :	
		return false;
	}

	return bResult;
}

SNATIVEID GLInventorySale::FindItemID( WORD wCol, WORD wRow )
{
	int ikey = wRow*6 + wCol;

	map_ExList_iter _iter = m_ExChangeList.find( ikey );

	if( _iter != m_ExChangeList.end() )
	{
		return _iter->second.sNativeID;
	}

	return NATIVEID_NULL();
}

bool GLInventorySale::IsUserExInsertEnable( GLInventory* pUserinven, SNATIVEID& sId, WORD wCol, WORD wRow, __int64 iRanPoint64, __int64 iLvPoint64, WORD wCount )
{
	int ikey = wRow*6 + wCol;

	map_ExList_iter _iter = m_ExChangeList.find( ikey );

	if( _iter == m_ExChangeList.end() || _iter->second.sNativeID != sId ) return false;

	EEXTYPE eType = _iter->second.exType;

	sExList& exList = _iter->second;

	BYTE iCount = exList.iCount;

	std::vector<sExNeed>::iterator _vIter = exList.vNeedItem.begin();

	const SITEM* pItem = GLogicData::GetInstance().GetItem(sId);

	if( pItem == NULL ) return false;

	switch( eType )
	{
	case EX_ITEM_TYPE  :
		{
			//if( pItem->ISPILE() )
			{
				iCount = iCount * wCount;
			}
			// 기본적인 공간은 있다.
			int iSlotCount = pUserinven->GetAmountEmptyPile(sId, false);

			if( iSlotCount >= iCount ) return true;

			// 공간이 없을떄. 재료를 빼면 넣을수 있는지 확인한다.
			DWORD dwSlotCount = 0;

			while(_vIter != exList.vNeedItem.end())
			{
				SITEM* pExItem = GLogicData::GetInstance().GetItem(_vIter->sNativeID);
				
				dwSlotCount += pUserinven->SimulateDelSlotCnt( _vIter->sNativeID, static_cast<WORD>(_vIter->iNeedCount*wCount ) );
				
				++_vIter;
			}

			dwSlotCount = dwSlotCount * pItem->sDrugOp.GetPileNum() + iSlotCount;

			if( dwSlotCount >= iCount ) return true;
		}
		break;
	case EX_POINT_TYPE :
	case EX_LVP_TYPE   :
		{
			BOOL bITEM_SPACE(FALSE);

			if ( pItem->ISPILE() )
			{
				//	겹침 아이템일 경우.
				WORD wPILENUM = pItem->sDrugOp.wPileNum;
				WORD wREQINSRTNUM = ( iCount * wCount );
				bITEM_SPACE = pUserinven->ValidPileInsrt ( wREQINSRTNUM, sId, wPILENUM );
			}
			else
			{
				//	일반 아이템의 경우.
				WORD wInsertPosX(0), wInsertPosY(0);
				bITEM_SPACE = pUserinven->FindInsertable ( wInsertPosX, wInsertPosY );
			}

			if ( bITEM_SPACE ) return true;
		}
		break;
	}

	return false;
}

bool GLInventorySale::GetIsNeedConditionHave( std::vector<sExNeed>& rList, GLInventory* pUserinven, SNATIVEID& sId, WORD wCol, WORD wRow, __int64 iRanPoint64, __int64 iLvPoint64, WORD wCount )
{
	int ikey = wRow*6 + wCol;

	map_ExList_iter _iter = m_ExChangeList.find( ikey );

	if( _iter == m_ExChangeList.end() || _iter->second.sNativeID != sId ) return false;

	EEXTYPE eType = _iter->second.exType;

	sExList& exList = _iter->second;

	std::vector<sExNeed>::iterator _vIter = exList.vNeedItem.begin();

	switch( eType )
	{
	case EX_ITEM_TYPE  :
		{
			DWORD dwCount = 0;

			std::map< SNATIVEID, sExNeed > mTemp;
			std::map< SNATIVEID, sExNeed >::iterator miter;

			sExNeed sTemp;

			while(_vIter != exList.vNeedItem.end())
			{
				sTemp = *_vIter;

				sTemp.iNeedCount = sTemp.iNeedCount*wCount;

				miter = mTemp.find( _vIter->sNativeID );

				if( miter != mTemp.end() ) {
					miter->second.iNeedCount += sTemp.iNeedCount;
				}
				else {
					mTemp[_vIter->sNativeID] = sTemp;
				}

				rList.push_back(sTemp);

				++_vIter;
			}

			miter = mTemp.begin();

			while(miter != mTemp.end())
			{
				dwCount = pUserinven->CountPileItem( miter->first );
				
				if( miter->second.iNeedCount > dwCount )	return false;

				++miter;
			}
		}
		break;
	case EX_POINT_TYPE :
		{
			sExNeed sTemp;

			while(_vIter != exList.vNeedItem.end())
			{
				sTemp = *_vIter;

				sTemp.iNeedCount = sTemp.iNeedCount*wCount;

				if( sTemp.iNeedCount > iRanPoint64 )	return false;

				rList.push_back(sTemp);

				++_vIter;
			}
		}
		break;
	case EX_LVP_TYPE   :
		{
			sExNeed sTemp;

			while(_vIter != exList.vNeedItem.end())
			{
				sTemp = *_vIter;

				sTemp.iNeedCount = sTemp.iNeedCount*wCount;

				if( sTemp.iNeedCount > iLvPoint64 )	return false;

				rList.push_back(sTemp);

				++_vIter;
			}
		}
		break;
	}
	// 필요아이템 총갯수	

	return true;
}